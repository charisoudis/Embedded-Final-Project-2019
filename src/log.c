#include "log.h"

/// Append end of session message and closes log file pointer.
/// \param executionTime
/// \param messagesStats
void log_tearDown(uint executionTime, MessagesStats* messagesStats)
{
    // End new session
    fprintf( logFilePointer, "\n/*\n"
                             "|--------------------------------------------------------------------------\n"
                             "| end: NEW SESSION\n"
                             "|--------------------------------------------------------------------------\n"
                             "|\n"
                             "| Duration            : %u secs\n"
                             "| Devices Connected   : %d\n"
                             "|\n"
                             "| Messages Produced   : %u ( avg. delay = %.03f min )\n"
                             "| Messages Received   : %u\n"
                             "| Messages Transmitted: %u\n"
                             "|\n"
                             "*/\n\n\n",
                             executionTime, 0,
                             messagesStats->produced, messagesStats->producedDelayAvg, messagesStats->received, messagesStats->transmitted );

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
                             "| Client  : %d\n"
                             "| FileName: %s\n"
                             "|\n"
                             "*/\n\n", nowAsString, CLIENT_AEM, fileName );
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
}

/// Logs error with errno found in status after where/when information.
/// \param functionName
/// \param actionName
/// \param status
void log_error(const char* functionName, const char* actionName, const int* status )
{
    char nowAsString[50];
    timestamp2ftime( (uint64) time(NULL), "%FT%TZ", nowAsString );

    fprintf( logFilePointer, "[ERR]| %s | %s | %s |\n\t%s\n", nowAsString, functionName, actionName, strerror( *status ) );
}