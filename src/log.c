#include <conf.h>
#include <log.h>
#include <utils.h>

extern uint32_t CLIENT_AEM;
static FILE *logFilePointer;


/// Logs error with errno found in status after where/when information.
/// \param functionName
/// \param actionName
/// \param status
void log_error(const char* functionName, const char* actionName, const int* status )
{
    char nowAsString[50];
    timestamp2ftime( (uint64) time(NULL), "%FT%TZ", nowAsString );

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
    char nowAsString[50];
    timestamp2ftime( (uint64) time(NULL), "%FT%TZ", nowAsString );

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
    if ( ALSO_LOG_TO_STDOUT )
    {
        inspect(message, 1, stdout);
        fprintf(stdout, "RAW>>>");
    }
}

/// Append end of session message and closes log file pointer.
/// \param executionTimeRequested
/// \param messagesStats
void log_tearDown(const uint executionTimeRequested, const double executionTimeActual, const MessagesStats *messagesStats) {
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

    char nowAsString[50];
    timestamp2ftime( (uint64) time( NULL ), "%a, %d %b %Y @ %T", nowAsString );

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