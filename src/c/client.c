#include "client.h"

/// \brief Generates a new message from this client towards $recipient with $body as content.
/// \param recipient message's recipient
/// \param body message's body
/// \return newly generated message of type message_t
Message generateMessage(uint32_t recipient, const char * body)
{
    Message message;

    message.sender = CLIENT_AEM;
    message.recipient = recipient;
    message.created_at = (uint64_t) time(NULL);
    memcpy( message.body, body, 256 );

    message.transmitted = false;

    return message;
}

/// \brief Generates a new random message composed of:
///     - random recipient  ( 4 randomly generated digits: {1-9}{0-9}{0-9}{0-9} )
///     - random body       ( 256 randomly generated ascii characters )
///     - CLIENT_AEM as sender
///     - creation time_of_day as created_at timestamp
/// \return newly generated message of type message_t
Message generateRandomMessage()
{
    uint32_t recipient;
    char body[256];
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
    static uint8_t charsetLength = 69;

    // Initial Random Number Generator seeding
    srand( (unsigned int) time( NULL ) );

    //  - random recipient
    recipient = (uint32_t) (rand() % (9999 - 1000 + 1) + 1000);

    //  - random body ( c: https://codereview.stackexchange.com/a/29276 )
    for ( size_t n = 0; n < 255; n++ )
    {
        int key = rand() % charsetLength;
        body[ n ] = charset[ key ];
    }
    body[255] = '\0';

    Message message = generateMessage( recipient, body );

    //  - random transmission status
    message.transmitted = (bool) ( time(NULL) & 1);
    if ( message.transmitted )
    {
        message.transmitted_device.mac[0] = (unsigned char) ( rand() % 256 );
        message.transmitted_device.mac[1] = (unsigned char) ( rand() % 256 );
        message.transmitted_device.mac[2] = (unsigned char) ( rand() % 256 );
        message.transmitted_device.mac[3] = (unsigned char) ( rand() % 256 );
        message.transmitted_device.mac[4] = (unsigned char) ( rand() % 256 );
        message.transmitted_device.mac[5] = (unsigned char) ( rand() % 256 );
    }

    return message;
}

/// \brief Fetches a message from server.
/// \param message the message with this CLIENT_AEM as recipient field
/// \return TRUE on success, FALSE otherwise
bool fetch(Message message)
{
    // TODO

    return false;
}

/// \brief Forwards given message to server for transmission ( message's recipient ).
/// \param message the message to be sent
/// \return TRUE on success, FALSE otherwise
bool forward(Message message)
{
    // TODO

    return false;
}