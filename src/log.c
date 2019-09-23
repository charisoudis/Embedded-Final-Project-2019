#include <conf.h>
#include <log.h>
#include <utils.h>

//------------------------------------------------------------------------------------------------

extern uint32_t CLIENT_AEM;
extern struct timeval CLIENT_AEM_CONN_START_LIST[CLIENT_AEM_LIST_LENGTH][MAX_CONNECTIONS_WITH_SAME_CLIENT];
extern struct timeval CLIENT_AEM_CONN_END_LIST[CLIENT_AEM_LIST_LENGTH][MAX_CONNECTIONS_WITH_SAME_CLIENT];
extern uint8_t CLIENT_AEM_CONN_N_LIST[CLIENT_AEM_LIST_LENGTH];

//------------------------------------------------------------------------------------------------

static FILE *logFilePointer;

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
void log_tearDown(const uint32_t executionTimeRequested, const double executionTimeActual, const MessagesStats *messagesStats) {
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

/// Creates / Opens file and add the new session message.
/// \param fileName
void log_tearUp(const char *fileName)
{
    // Check if file exists
    //  - yes: new session
    //  - no : new file + new session
    logFilePointer = ( access( fileName, F_OK ) == -1 ) ?
                     fopen( fileName, "w" ):
                     fopen( fileName, "a" );

    const char* nowAsString = timestamp2ftime( (uint64_t) time(NULL), "%FT%TZ" );

    // Start new session
    fprintf( logFilePointer, "/*\n"
                             "|--------------------------------------------------------------------------\n"
                             "| start: NEW SESSION\n"
                             "|--------------------------------------------------------------------------\n"
                             "|\n"
                             "| Date    : %s\n"
                             "| Client  : %s\n"
                             "| FileName: %s\n"
                             "|\n"
                             "*/\n\n", nowAsString, aem2ip( CLIENT_AEM ), fileName );

    if ( ALSO_LOG_TO_STDOUT )
    {
        fprintf( stdout, "/*\n"
                             "|--------------------------------------------------------------------------\n"
                             "| start: NEW SESSION\n"
                             "|--------------------------------------------------------------------------\n"
                             "|\n"
                             "| Date    : %s\n"
                             "| Client  : %s\n"
                             "| FileName: %s\n"
                             "|\n"
                             "*/\n\n", nowAsString, aem2ip( CLIENT_AEM ), fileName );
    }
}