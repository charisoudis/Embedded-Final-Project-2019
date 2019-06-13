#include "utils.h"

/// \brief Un-serializes message-as-a-string and recreates message struct.
/// \param glue the connective character; acts as the separator between successive message fields
/// \param messageSerialized string containing all message fields glued together using $glue
/// \return a message struct of type message_t
Message explode(const char glue, MessageSerialized messageSerialized)
{
    Message message;
    MessageSerialized messageCopy = strdup( messageSerialized );

    // Start exploding string
    message.sender = (uint32_t) strtol( strsep( &messageCopy, &glue ), (char **)NULL, 10 );
    message.recipient = (uint32_t) strtol( strsep( &messageCopy, &glue ), (char **)NULL, 10 );
    message.created_at = (uint64_t) strtoll( strsep( &messageCopy, &glue ), (char **)NULL, 10 );
    memcpy( message.body, strsep( &messageCopy, &glue ), 256 );

    return message;
}

/// \brief Serializes a message ( of message_t type ) into a 277-characters string.
/// \param glue the connective character; to be placed between successive message fields
/// \param message the message to be serialized
/// \return a string containing all message fields glued together using $glue
MessageSerialized implode(const char glue, const Message message)
{
    MessageSerialized messageSerialized;
    messageSerialized = (char *)malloc( 277 );

    // Begin copying fields and adding glue
    //  - sender{glue}recipient{glue}created_at{glue}body
    snprintf(messageSerialized, 277, "%04d%c%04d%c%010ld%c%s",
             message.sender, glue,
             message.recipient, glue,
             message.created_at, glue,
             message.body
    );

    return messageSerialized;
}

/// \brief Log ( to stdout ) message's fields.
/// \param message
void inspect(Message message)
{
    fprintf( stdout, "message = {\n\tsender = %04d,\n\trecipient = %04d,\n\tcreated_at = %010ld,\n\tbody = %s\n}\n\n",
            message.sender, message.recipient, message.created_at, message.body
    );
}

/// \brief Check if two messages have exactly the same values in ALL of their fields.
/// \param message1
/// \param message2
/// \return
bool isEqual(Message message1, Message message2)
{
    if ( message1.sender != message2.sender )
        return false;
    if ( message1.recipient != message2.recipient )
        return false;
    if ( message1.created_at != message2.created_at )
        return false;
    if ( 0 != strcmp( message1.body, message2.body ) )
        return false;

    return true;
}