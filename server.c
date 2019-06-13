#include "server.h"

/// \brief Get an array of pointer to messages whose recipient is $recipient.
/// \param recipient the recipient for whom messages will be retrieved out of $messages buffer
/// \return an array of pointers to $messages buffer
Message **getPendingMessages(uint32_t recipient)
{
    // TODO

    return NULL;
}

/// \brief Check if $message buffer has messages intended to be received by $recipient.
/// \param recipient the recipient to check against
/// \return TRUE if there exist messages for $recipient, FALSE otherwise
bool hasPendingMessages(uint32_t recipient)
{
    // TODO

    return false;
}