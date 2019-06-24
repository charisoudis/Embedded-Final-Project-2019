#ifndef FINAL_SERVER_H
#define FINAL_SERVER_H

#include "utils.h"

#ifndef MESSAGES_PUSH_OVERRIDE_POLICY
    #define MESSAGES_PUSH_OVERRIDE_SENT_ONLY 0
    #define MESSAGES_PUSH_OVERRIDE_BLIND 1
    #define MESSAGES_PUSH_OVERRIDE_POLICY MESSAGES_PUSH_OVERRIDE_BLIND
#endif

#ifndef MESSAGES_SIZE
    #define MESSAGES_SIZE 2000
#endif

Message messages[ MESSAGES_SIZE ];

/* messagesHead is allowed to exceed MESSAGES_SIZE; if so, then mod-MESSAGES_SIZE arithmetic is used to calculate
 * actual head position */
uint16_t messagesHead;

/// \brief Push $message to $messages circle buffer. Updates $messageHead acc. to selected override policy.
/// \param message
void messages_push(Message message);

/// \brief Main server loop. Calls communication_thread() on each new connection.
void server_listen();

#endif //FINAL_SERVER_H
