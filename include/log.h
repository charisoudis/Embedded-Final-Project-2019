#ifndef FINAL_LOG_H
#define FINAL_LOG_H

#include <stdio.h>
#include <unistd.h>
#include "client.h"
#include "utils.h"

typedef struct messages_stats_t {

    // Total
    uint16_t produced;
    uint16_t received;
    uint16_t transmitted;

    // Time
    float producedDelayAvg;

} MessagesStats;

static FILE *logFilePointer;

/// Append end of session message and closes log file pointer.
/// \param executionTime
/// \param messagesStats
void log_tearDown(uint executionTime, MessagesStats* messagesStats);

/// Creates / Opens file and add the new session message.
/// \param fileName
void log_tearUp(const char *fileName);

/// Logs message after where/when information.
/// \param message
/// \param functionName
/// \param actionName
void log_info(const char* message, const char* functionName, const char* actionName);

/// Logs error with errno found in status after where/when information.
/// \param functionName
/// \param actionName
/// \param status
void log_error(const char* functionName, const char* actionName, const int* status );

#endif //FINAL_LOG_H
