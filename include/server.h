#ifndef FINAL_SERVER_H
#define FINAL_SERVER_H

#include "utils.h"

Message messages[2000];

///// \brief Get an array of pointer to messages whose recipient is $recipient.
///// \param recipient the recipient for whom messages will be retrieved out of $messages buffer
///// \return an array of pointers to $messages buffer
//Message **getPendingMessages(uint32_t recipient);
//
///// \brief Check if $message buffer has messages intended to be received by $recipient.
///// \param recipient the recipient to check against
///// \return TRUE if there exist messages for $recipient, FALSE otherwise
//bool hasPendingMessages(uint32_t recipient);

/// \brief Thread for handling communication staff ( POSIX thread compatible function ).
/// \param args pointer to communication_thread_args_t type
void communication_thread( void *args );

/// \brief Main server loop. Starts polling
/// \param max_secs
/// \return
bool loop( uint32_t max_secs );

/// \brief Receives a single message from $transmitter.
/// \param receiver the connected device
/// \param message the message to be received from $transmitter
/// \return TRUE on success, FALSE otherwise
bool receive(Device transmitter, Message message);

/// \brief Receives all pending messages of $transmitter and appends them to $messages circular buffer.
/// \param transmitter the connected device
/// \return the number of messages received
uint16_t receiveAll(Device transmitter);

/// \brief Transmits a single message to $receiver.
/// \param receiver the connected device
/// \param message the message to be transmitted to $receiver
/// \return TRUE on success, FALSE otherwise
bool transmit(Device receiver, Message message);

/// \brief Transmits all pending messages for $receiver ( having in recipient field device's AEM ).
/// \param receiver the connected device
/// \return the number of messages transmitted
uint16_t transmitAll(Device receiver);

#endif //FINAL_SERVER_H
