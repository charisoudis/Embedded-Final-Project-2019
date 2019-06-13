#ifndef FINAL_CLIENT_H
#define FINAL_CLIENT_H

#include <stdbool.h>
#include <time.h>
#include "utils.h"

#ifndef CLIENT_AEM
#define CLIENT_AEM 9026
#endif

/// \brief Generates a new message from this client towards $recipient with $body as content.
/// \param recipient message's recipient
/// \param body message's body
/// \return newly generated message of type message_t
Message generateMessage(uint32_t recipient, const char * body);

/// \brief Generates a new random message composed of:
///     - random recipient  ( 4 randomly generated digits: {1-9}{0-9}{0-9}{0-9} )
///     - random body       ( 256 randomly generated ascii characters )
///     - CLIENT_AEM as sender
///     - creation time_of_day as created_at timestamp
/// \return newly generated message of type message_t
Message generateRandomMessage();

/// \brief Sends given message to message's recipient.
/// \param message the message to be sent
/// \return TRUE on success, FALSE otherwise
bool send(Message message);

#endif //FINAL_CLIENT_H
