#ifndef FINAL_LOG_H
#define FINAL_LOG_H

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <types.h>

/// \brief Logs the start of a new event in session.json file
/// \param type
/// \param server
/// \param client
void log_event_start( const char* type, uint32_t server, uint32_t client );

/// \brief Logs $message to session.json file
/// \param action
/// \param message
void log_event_message( const char* action, const Message* message );

/// \brief Logs datetime syncing to session.json file
/// \param previous_now
/// \param new_now
void log_event_message_datetime( uint64_t previous_now, uint64_t new_now );

/// \brief Logs the end of a new event in session.json file
void log_event_stop(void);

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

/// \brief Append end of session message and closes log file pointer
/// \param executionTimeActual
/// \param messagesStats
void log_tearDown( double executionTimeActual, const MessagesStats *messagesStats );

/// \brief Creates / Opens file and add the new session message
/// \param logFileName
/// \param jsonFileName
void log_tearUp(const char *logFileName, const char *jsonFileName);

/// \brief Removes last character from session.json file
void removeTrailingCommaFromJson(void);

#endif //FINAL_LOG_H
