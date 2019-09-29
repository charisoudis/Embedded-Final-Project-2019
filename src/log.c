#include <conf.h>
#include <log.h>
#include <utils.h>
#include <sys/time.h>

//------------------------------------------------------------------------------------------------

extern uint32_t CLIENT_AEM;
extern struct timeval CLIENT_AEM_CONN_START_LIST[CLIENT_AEM_LIST_LENGTH][MAX_CONNECTIONS_WITH_SAME_CLIENT];
extern struct timeval CLIENT_AEM_CONN_END_LIST[CLIENT_AEM_LIST_LENGTH][MAX_CONNECTIONS_WITH_SAME_CLIENT];
extern uint8_t CLIENT_AEM_CONN_N_LIST[CLIENT_AEM_LIST_LENGTH];
extern uint32_t executionTimeRequested;

//------------------------------------------------------------------------------------------------

static FILE *logFilePointer;
static FILE *jsonFilePointer;

struct timeval lastEventStart, lastEventStop;

/// \brief Logs the start of a new event in session.json file
/// \param type
/// \param server
/// \param client
void log_event_start( const char* type, uint32_t server, uint32_t client )
{
    gettimeofday( &lastEventStart, NULL );

    removeTrailingCommaFromJson();
    fprintf( jsonFilePointer, "{\"occured_at\": \"%s\", \"type\": \"%s\", \"server\": \"%u\", \"client\": \"%u\", \"messages\": [",
            timestamp2ftime( (uint64_t) time(NULL), "%H:%M:%S" ), type,
            server, client );
}

/// \brief Logs $message to session.json file
/// \param action
/// \param message
void log_event_message( const char* action, const Message* message )
{
    fprintf( jsonFilePointer, "{\"saved_at\": \"%s\", \"action\": \"%s\", \"sender\": \"%u\", \"recipient\": \"%u\", \"created_at\": \"%s\", \"body\": \"%s\", \"transmitted\": \"%s\", \"transmitted_devices\": \"%s\"},",
             timestamp2ftime( (uint64_t) time(NULL), "%FT%TZ" ), action,
             message->sender, message->recipient, timestamp2ftime( message->created_at, "%FT%TZ" ), message->body,
             message->transmitted == 1 ? "TRUE" : "FALSE", getTransmittedDevicesString( message ) );
}

/// \brief Logs datetime syncing to session.json file
/// \param previous_now
/// \param new_now
void log_event_message_datetime( uint64_t previous_now, uint64_t new_now )
{
    fprintf( jsonFilePointer, "{\"saved_at\": \"%s\", \"action\": \"%s\", \"previous_now\": \"%s\", \"new_now\": \"%s\"},",
             timestamp2ftime( (uint64_t) time(NULL), "%FT%TZ" ), "datetime",
             timestamp2ftime( previous_now, "%FT%TZ" ), timestamp2ftime( new_now, "%FT%TZ" ) );
}

/// \brief Logs the end of a new event in session.json file
void log_event_stop(void)
{
    gettimeofday( &lastEventStop, NULL );

    double duration = (double) ( lastEventStop.tv_sec - lastEventStart.tv_sec ) * 1000 +
            (double) ( lastEventStop.tv_usec - lastEventStart.tv_usec ) / 1000;

    removeTrailingCommaFromJson();
    fprintf( jsonFilePointer, "], \"duration\": \"%f ms\"},", duration );
}

/// Logs error with errno found in status after where/when information.
/// \param functionName
/// \param actionName
/// \param status
void log_error(const char* functionName, const char* actionName, const int* status )
{
    const char* nowAsString = timestamp2ftime( (uint64_t) time(NULL), "%FT%TZ" );

    fprintf( logFilePointer, "[ERR]| %s | %s | %s |\n\t%s\n", nowAsString, functionName, actionName, strerror( *status ) );
    if ( ALSO_LOG_TO_STDOUT )
    {
        fprintf( stdout, "[ERR]| %s | %s | %s |\n\t%s\n", nowAsString, functionName, actionName, strerror( *status ) );
    }

}

/// Logs message after where/when information.
/// \param message
/// \param functionName
/// \param actionName
void log_info(const char* message, const char* functionName, const char* actionName)
{
    const char* nowAsString = timestamp2ftime( (uint64_t) time(NULL), "%FT%TZ" );

    fprintf( logFilePointer, "[INF]| %s | %s | %s |\n\t%s\n", nowAsString, functionName, actionName, message );
    if ( ALSO_LOG_TO_STDOUT )
    {
        fprintf( stdout, "[INF]| %s | %s | %s |\n\t%s\n", nowAsString, functionName, actionName, message );
    }
}

/// \brief Logs given message ( prints similar to utils.h > inspect() ) after where/when information.
/// \param functionName
void log_message(const char* functionName, const Message message )
{
    log_info( "Message Inspection\n<<<RAW", functionName, "-" );
    inspect( message, 1, logFilePointer );
    fprintf( logFilePointer, "RAW>>>" );
}

/// Append end of session message and closes log file pointer.
/// \param executionTimeRequested
/// \param messagesStats
void log_tearDown(const double executionTimeActual, const MessagesStats *messagesStats) {
    // End new session
    fprintf( logFilePointer, "\n/*\n"
                             "|--------------------------------------------------------------------------\n"
                             "| end: NEW SESSION\n"
                             "|--------------------------------------------------------------------------\n"
                             "|\n"
                             "| Duration Actual     : %lf secs\n"
                             "| Duration Requested  : %u secs\n"
                             "| Devices Connected   : %d\n"
                             "|\n"
                             "| Messages Produced   : %u ( avg. delay = %.03f min )\n"
                             "| Messages Received   : %u\n"
                             "| Messages Transmitted: %u\n"
                             "|\n"
                             "*/\n\n\n",
             executionTimeActual, executionTimeRequested, 0,
             messagesStats->produced, messagesStats->producedDelayAvg, messagesStats->received, messagesStats->transmitted );

    if ( ALSO_LOG_TO_STDOUT )
    {
        fprintf(stdout, "\n/*\n"
                        "|--------------------------------------------------------------------------\n"
                        "| end: NEW SESSION\n"
                        "|--------------------------------------------------------------------------\n"
                        "|\n"
                        "| Duration Actual     : %lf secs\n"
                        "| Duration Requested  : %u secs\n"
                        "| Devices Connected   : %d\n"
                        "|\n"
                        "| Messages Produced   : %u ( avg. delay = %.03f min )\n"
                        "| Messages Received   : %u\n"
                        "| Messages Transmitted: %u\n"
                        "|\n"
                        "*/\n\n\n",
                executionTimeActual, executionTimeRequested, 0,
                messagesStats->produced, messagesStats->producedDelayAvg, messagesStats->received,
                messagesStats->transmitted);
    }

    // Close file pointer
    fclose( logFilePointer );

    removeTrailingCommaFromJson();
    fprintf( jsonFilePointer, "], \"duration\": \"%f s\", \"end\": \"%s\"}",
            executionTimeActual, timestamp2ftime( (uint64_t) time(NULL), "%FT%TZ" ) );

    // Close json file pointer
    fclose( jsonFilePointer );

    // Inspect all messages that are in $messages buffer's final state
    inspect_messages( true );

    // Inspect connections
    fprintf( stdout, "\n\n-------------------- start: DEVICES INSPECTION --------------------\n" );
    for ( uint32_t device_i = 0; device_i < CLIENT_AEM_LIST_LENGTH; device_i++ )
    {
        uint32_t aem = CLIENT_AEM_LIST[device_i];
        fprintf( stdout, "\t- %04d\n", aem );

        for ( uint8_t n = 0; n < CLIENT_AEM_CONN_N_LIST[device_i]; n++ )
        {
            fprintf( stdout, "\t\t start: %lf | end %lf ( duration: %lf )\n",
                CLIENT_AEM_CONN_START_LIST[device_i][n].tv_sec + CLIENT_AEM_CONN_START_LIST[device_i][n].tv_usec * 1e-6,
                CLIENT_AEM_CONN_END_LIST[device_i][n].tv_sec + CLIENT_AEM_CONN_END_LIST[device_i][n].tv_usec * 1e-6,
                (double)( CLIENT_AEM_CONN_END_LIST[device_i][n].tv_sec - CLIENT_AEM_CONN_START_LIST[device_i][n].tv_sec ) +
                (double)( CLIENT_AEM_CONN_END_LIST[device_i][n].tv_usec - CLIENT_AEM_CONN_START_LIST[device_i][n].tv_usec ) * 1e-6
            );
        }
    }
    fprintf( stdout, "\n-------------------- end: DEVICES INSPECTION --------------------\n\n" );
}

/// Creates / Opens file and add the new session messages
/// \param logFileName
/// \param jsonFileName
void log_tearUp(const char *logFileName, const char *jsonFileName)
{
    // Check if log file exists
    //  - yes: new session
    //  - no : new file + new session
    logFilePointer = (access(logFileName, F_OK ) == -1 ) ?
                     fopen(logFileName, "w" ) :
                     fopen(logFileName, "a" );

    // Check if session.json file exists
    if ( access( jsonFileName, F_OK ) != -1 )
        remove( jsonFileName );
    jsonFilePointer = fopen( jsonFileName, "w+" );

    const char* nowAsString = timestamp2ftime( (uint64_t) time(NULL), "%FT%TZ" );

    // Start new session
    fprintf(logFilePointer, "/*\n"
                             "|--------------------------------------------------------------------------\n"
                             "| start: NEW SESSION\n"
                             "|--------------------------------------------------------------------------\n"
                             "|\n"
                             "| Date    : %s\n"
                             "| Client  : %s\n"
                             "| FileName: %s\n"
                             "|\n"
                             "*/\n\n", nowAsString, aem2ip( CLIENT_AEM ), logFileName );

    if ( ALSO_LOG_TO_STDOUT )
    {
        fprintf(stdout, "/*\n"
                             "|--------------------------------------------------------------------------\n"
                             "| start: NEW SESSION\n"
                             "|--------------------------------------------------------------------------\n"
                             "|\n"
                             "| Date    : %s\n"
                             "| Client  : %s\n"
                             "| FileName: %s\n"
                             "|\n"
                             "*/\n\n", nowAsString, aem2ip( CLIENT_AEM ), logFileName );
    }

    // JSON file start
    fprintf( jsonFilePointer, "{\"start\": \"%s\", \"requested_duration\":\"%u secs\", \"events\": [,", nowAsString, executionTimeRequested );
}

/// \brief Removes last character from session.json file
void removeTrailingCommaFromJson(void)
{
    fseeko( jsonFilePointer, -1, SEEK_END );
    ftruncate( fileno( jsonFilePointer ), ftello( jsonFilePointer ) );
}