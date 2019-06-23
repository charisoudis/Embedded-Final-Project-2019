#ifndef FINAL_SERVER_H
#define FINAL_SERVER_H

#include "utils.h"

#ifndef MESSAGES_PUSH_OVERRIDE_POLICY
    #define MESSAGES_PUSH_OVERRIDE_SENT_ONLY 0
    #define MESSAGES_PUSH_OVERRIDE_BLIND 1
    #define MESSAGES_PUSH_OVERRIDE_POLICY MESSAGES_PUSH_OVERRIDE_SENT_ONLY
#endif

#ifndef MESSAGES_SIZE
    #define MESSAGES_SIZE 2000
#endif

Message messages[ MESSAGES_SIZE ];

/* messagesHead is allowed to exceed MESSAGES_SIZE; if so, then mod-MESSAGES_SIZE arithmetic is used to calculate
 * actual head position */
uint16_t messagesHead;

///// \brief Get an array of pointer to messages whose recipient is $recipient.
///// \param recipient the recipient for whom messages will be retrieved out of $messages buffer
///// \return an array of pointers to $messages buffer
//Message **getPendingMessages(uint32_t recipient);
//
///// \brief Count messages in $messages buffer that have not been sent $recipient before.
///// \param receiver the recipient to check against
///// \return TRUE if there exist messages for $recipient, FALSE otherwise
//bool countPendingMessages(Device receiver);

/// \brief Push $message to $messages circle buffer. Updates $messageHead acc. to selected override policy.
/// \param message
void messages_push(Message message);

/// \brief Main server loop. Calls communication_thread() on each new connection.
void server_listen();

#endif //FINAL_SERVER_H
