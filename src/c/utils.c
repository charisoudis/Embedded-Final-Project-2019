#include "utils.h"

/// \brief Un-serializes message-as-a-string, re-creating initial message.
/// \param glue the connective character(s); acts as the separator between successive message fields
/// \param messageSerialized string containing all message fields glued together using $glue
/// \return a message struct of type message_t
Message explode(const char * glue, MessageSerialized messageSerialized)
{
    Message message;
    MessageSerialized messageCopy = strdup( messageSerialized );
    void *messageCopyPointer = ( void * ) messageCopy;

    // Start exploding string
    message.sender = (uint32_t) strtol( strsep( &messageCopy, glue ), (char **)NULL, 10 );
    message.recipient = (uint32_t) strtol( strsep( &messageCopy, glue ), (char **)NULL, 10 );
    message.created_at = (uint64_t) strtoll( strsep( &messageCopy, glue ), (char **)NULL, 10 );

    memcpy( message.body, strsep( &messageCopy, glue ), 256 );
    free( messageCopyPointer );

    // Set message's metadata
    message.transmitted = false;

    return message;
}

/// Convert HEX string to an array of bytes representing MAC address.
/// \param hex HEX string ( successive bytes should be glued together using ':' )
/// \param mac MAC address as array of bytes ( 'byte' is 'unsigned char' in C )
void hex2mac(const char * hex, unsigned char * mac)
{
    char *hexCopy = strdup( hex ), *token;
    char glue = ':';
    size_t b = 0;

    while ( (token = strsep(&hexCopy, &glue) ) )
        mac[b++] = (unsigned char) strtol(token, (char **)NULL, 16 );
}

/// \brief Serializes a message ( of message_t type ) into a 277-characters string.
/// \param glue the connective character(s); to be placed between successive message fields
/// \param message the message to be serialized
/// \param messageSerialized a string containing all message fields glued together using $glue
void implode(const char * glue, const Message message, MessageSerialized messageSerialized)
{
    // Begin copying fields and adding glue
    //  - sender{glue}recipient{glue}created_at{glue}body
    snprintf(messageSerialized, 277, "%04d%s%04d%s%010ld%s%s",
             message.sender, glue,
             message.recipient, glue,
             message.created_at, glue,
             message.body
    );
}

/// \brief Log ( to stdout ) message's fields.
/// \param message
/// \param metadata show/hide metadata information from message
void inspect(const Message message, bool metadata)
{
    // Parse timestamp
    char created_at_full[50];
    timestamp2ftime( message.created_at, "%a, %d %b %Y @ %T", created_at_full );

    // Print main fields
    fprintf( stdout, "message = {\n\tsender = %04d,\n\trecipient = %04d,\n\tcreated_at = %010ld ( %s ),\n\tbody = %s\n",
            message.sender, message.recipient, message.created_at, created_at_full, message.body
    );

    // Print metadata
    if ( metadata )
    {
        char hex[18];

        message.transmitted ?
            mac2hex( message.transmitted_device.mac, hex ):
            snprintf( hex, 18, "--:--:--:--:--:--" );

        fprintf( stdout, "\t---\n\ttransmitted = %d\n\ttransmitted_device = %s\n", message.transmitted, hex );
    }

    fprintf( stdout, "}\n\n" );
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

/// Convert MAC address from byte array to string ( adding ':' between successive bytes )
/// \param mac mac address as array of bytes ( 'byte' is 'unsigned char' in C )
/// \param hex pointer to the HEX string of the MAC address
void mac2hex(const unsigned char *mac, char *hex)
{
    sprintf(hex, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
    hex[18] = '\0';
}

/// \brief Convert given UNIX timestamp to a formatted datetime string with given $format.
/// \param timestamp UNIX timestamp ( uint64_t )
/// \param format strftime-compatible format
/// \param string the resulting datetime string
inline void timestamp2ftime( const uint64_t timestamp, const char *format, char *string )
{
    struct tm *tmp = localtime((const time_t *) &( timestamp ));
    if ( tmp == NULL )
    {
        perror( "\ttimestamp2ftime(): localtime() error" );
        exit( EXIT_FAILURE );
    }
    if ( 0 == strftime( string, UINT32_MAX, format, tmp ) )
    {
        fprintf( stderr, "\ttimestamp2ftime(): strftime() error" );
        exit( EXIT_FAILURE );
    }
}