#ifndef FINAL_LOG_H
#define FINAL_LOG_H

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <types.h>

/// \brief Logs message after where/when information.
/// \param message
/// \param functionName
/// \param actionName
void log_info(const char* message, const char* functionName, const char* actionName);

/// \brief Logs error with errno found in status after where/when information.
/// \param functionName
/// \param actionName
/// \param status
void log_error(const char* functionName, const char* actionName, const int* status );

/// \brief Logs given message ( prints similar to utils.h > inspect() ) after where/when information.
/// \param functionName
/// \param message
void log_message(const char* functionName, Message message );

/// \brief Append end of session message and closes log file pointer.
/// \param executionTimeRequested
/// \param executionTimeActual
/// \param messagesStats
void log_tearDown(uint executionTimeRequested, double executionTimeActual, const MessagesStats* messagesStats);

/// \brief Creates / Opens file and add the new session message.
/// \param fileName
void log_tearUp(const char *fileName);

#endif //FINAL_LOG_H
