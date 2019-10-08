#include "conf.h"
#include "utils.h"
#include "log.h"
#include "server.h"
#include <arpa/inet.h>
#include <stdbool.h>

//------------------------------------------------------------------------------------------------

extern uint32_t CLIENT_AEM;
extern struct timeval CLIENT_AEM_CONN_START_LIST[CLIENT_AEM_LIST_LENGTH][MAX_CONNECTIONS_WITH_SAME_CLIENT];
extern struct timeval CLIENT_AEM_CONN_END_LIST[CLIENT_AEM_LIST_LENGTH][MAX_CONNECTIONS_WITH_SAME_CLIENT];
extern uint8_t CLIENT_AEM_CONN_N_LIST[CLIENT_AEM_LIST_LENGTH];

extern pthread_mutex_t messagesBufferLock, activeDevicesLock, availableThreadsLock, messagesStatsLock;
extern MessagesStats messagesStats;

extern pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
extern uint8_t communicationThreadsAvailable;

extern Message MESSAGES_BUFFER[ MESSAGES_SIZE ];

//------------------------------------------------------------------------------------------------

/// \brief Constructs IPv4 address from given AEM.
/// \param aem uint32_t
/// \return ip string
const char* aem2ip(uint32_t aem)
{
    static char ip[INET_ADDRSTRLEN];
    sprintf( ip, "10.0.%02d.%02d", (int) aem / 100, aem % 100 );

    return ip;
}

/// \brief Perform binary search in $haystack array for $needle and return index of $needle or -1.
/// \param haystack
/// \param N size of $haystack
/// \param needle
/// \return index [0, N-1] if found, -1 else
int32_t binary_search_index(const uint32_t *haystack, size_t N, uint32_t needle)
{
    size_t first;
    size_t last;
    size_t middle;

    first = 0;
    last = N - 1;
    middle = ( first + last ) / 2;

    do
    {
        if ( haystack[middle] < needle )
        {
            first = middle + 1;
        }
        else if ( haystack[middle] == needle )
        {
            // Found, index is $middle
            return ( int32_t ) middle;
        }
        else
        {
            last = middle - 1;
        }

        middle = ( first + last ) / 2;
    }
    while ( first <= last );

    // Not found
    return -1;
}

/// \brief Un-serializes message-as-a-string, re-creating initial message.
/// \param message the result message ( passes as a pointer )
/// \param glue the connective character(s); acts as the separator between successive message fields
/// \param messageSerialized string containing all message fields glued together using $glue
/// \return a message struct of type message_t
void explode(Message *message, const char *glue, char *messageSerialized)
{
    char *messageCopy = strdup( messageSerialized );

    // Start exploding string
    message->sender = (uint32_t) strtol( strsep( &messageCopy, glue ), (char **)NULL, STRSEP_BASE_10 );
    message->recipient = (uint32_t) strtol( strsep( &messageCopy, glue ), (char **)NULL, STRSEP_BASE_10 );
    message->created_at = (uint64_t) strtoll(strsep(&messageCopy, glue ), (char **)NULL, STRSEP_BASE_10 );

    memcpy( message->body, strsep( &messageCopy, glue ), MESSAGE_BODY_LEN );
    free( messageCopy );

    // Set message's metadata
    message->transmitted = 0;
    message->transmitted_to_recipient = 0;
    for ( uint32_t device_i = 0; device_i < CLIENT_AEM_LIST_LENGTH; device_i++ )
        message->transmitted_devices[device_i] = 0;
}

/// \brief Generates a new message from this client towards $recipient with $body as content.
/// \param message result message ( passed as pointer )
/// \param recipient message's recipient
/// \param body message's body
void generateMessage(Message *message, uint32_t recipient, const char * body)
{
    message->sender = CLIENT_AEM;
    message->recipient = recipient;
    message->created_at = (uint64_t) time(NULL );

    memcpy( message->body, body, MESSAGE_BODY_LEN );

    message->transmitted = 0;
    for ( uint32_t device_i = 0; device_i < CLIENT_AEM_LIST_LENGTH; device_i++ )
        message->transmitted_devices[device_i] = 0;
}

/// \brief Generates a new random message composed of:
///     - random recipient  ( 4 randomly generated digits: {1-9}{0-9}{0-9}{0-9} )
///     - random body       ( 256 randomly generated ascii characters )
///     - CLIENT_AEM as sender
///     - creation time_of_day as created_at timestamp
/// \param message result message ( passed as pointer )
void generateRandomMessage(Message *message)
{
    uint32_t recipient;
    char body[MESSAGE_BODY_LEN];

    srand( (unsigned int) time(NULL) );

    //  - random recipient
    do
    {
        recipient = ( !strcmp( "range", CLIENT_AEM_SOURCE ) ) ?
                    (uint32_t) (rand() % (CLIENT_AEM_RANGE_MAX + 1 - CLIENT_AEM_RANGE_MIN ) + CLIENT_AEM_RANGE_MIN):
                    CLIENT_AEM_LIST[( rand() % CLIENT_AEM_LIST_LENGTH )];
    }
    while( recipient == CLIENT_AEM );

    //  - random body
    char ch;
    for ( int byte_i = 0; byte_i < MESSAGE_BODY_LEN - 1; byte_i ++ )
    {
        do { ch = (char) ( rand() % (MESSAGE_BODY_ASCII_MAX - MESSAGE_BODY_ASCII_MIN + 1) + MESSAGE_BODY_ASCII_MIN); }
        while( '_' == ch || '\\' == ch || '"' == ch );
        body[byte_i] = ch;
    }
    body[MESSAGE_BODY_LEN - 1] = '\0';

    generateMessage( message, recipient, body );
}

/// \brief Serializes a message ( of message_t type ) into a 277-characters string.
/// \param glue the connective character(s); to be placed between successive message fields
/// \param message the message to be serialized
/// \param messageSerialized a string containing all message fields glued together using $glue
void implode(const char *glue, const Message message, char *messageSerialized)
{
    // Begin copying fields and adding glue
    //  - sender{glue}recipient{glue}created_at{glue}body
    // sprintf(messageSerialized, MESSAGE_SERIALIZED_LEN, "%04d%s%04d%s%010ld%s%s",
    snprintf(messageSerialized, MESSAGE_SERIALIZED_LEN, "%04d%s%04d%s%010llu%s%s",
             message.sender, glue,
             message.recipient, glue,
             message.created_at, glue,
             message.body
    );
}

/// \brief Get a string with CSV of transmitted devices of given $message
/// \param message
/// \return
const char* getTransmittedDevicesString( const Message* message )
{
    static char transmittedDevicesString[CLIENT_AEM_LIST_LENGTH * 6];
    uint32_t writePosition;
    uint32_t aem_i;

    for ( writePosition = 0, aem_i = 0; aem_i < CLIENT_AEM_LIST_LENGTH; aem_i++ )
    {
        if ( 1 == message->transmitted_devices[ aem_i ] )
        {
            snprintf( transmittedDevicesString + writePosition, 6, "%04d,", CLIENT_AEM_LIST[aem_i] );
            writePosition += 5;
        }
    }

    *( transmittedDevicesString + writePosition - 1 ) = '\0';

    return transmittedDevicesString;
}

/// \brief Log ( to file pointer ) message's fields.
/// \param message
/// \param metadata show/hide metadata information from message
void inspect(const Message message, bool metadata, FILE *fp)
{
    // Print main fields
    fprintf( fp, "message = {\n\tsender = %04d,\n\trecipient = %04d,\n\tcreated_at = %lu ( %s ),\n\tbody = %s\n",
            message.sender, message.recipient, message.created_at,
            timestamp2ftime( message.created_at, "%a, %d %b %Y @ %T" ), message.body
    );

    // Print metadata
    if ( metadata )
    {
        if ( 0 == message.transmitted )
            fprintf( fp, "\t---\n\ttransmitted = FALSE\n\ttransmitted_devices = -\n" );
        else
        {
            const char* transmittedDevicesString = getTransmittedDevicesString( &message );
            fprintf( fp, "\t---\n\ttransmitted = TRUE\n\ttransmitted_devices = %s\n", transmittedDevicesString );
        }
    }

    fprintf( fp, "}\n" );
}

/// \brief Extracts AEM from given IPv4 address.
/// \param ip string ( resulting from inet_ntop() )
/// \return aem uint32_t
uint32_t ip2aem(const char *ip)
{
    char *ipCopy = strdup( ip );
    void *ipCopyPointer = ( void * ) ipCopy;
    unsigned char ipParts[4];
    char *glue = ".";
    uint32_t aem;

    // Explode IP string
    ipParts[0] = (unsigned char) strtol( strsep( &ipCopy, glue ), (char **)NULL, STRSEP_BASE_10 );
    ipParts[1] = (unsigned char) strtol( strsep( &ipCopy, glue ), (char **)NULL, STRSEP_BASE_10 );
    ipParts[2] = (unsigned char) strtol( strsep( &ipCopy, glue ), (char **)NULL, STRSEP_BASE_10 );
    ipParts[3] = (unsigned char) strtol( strsep( &ipCopy, glue ), (char **)NULL, STRSEP_BASE_10 );

    // Compose AEM
    aem = (uint32_t) ( ipParts[2] * 100 );
    aem += (uint32_t) ipParts[3];

    free( ipCopyPointer );
    return aem;
}

/// \brief Check if two messages have exactly the same values in ALL of their fields.
/// \param message1
/// \param message2
/// \return
bool isMessageEqual(Message message1, Message message2)
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

/// \brief Check if two messages have exactly the same values in ALL of their fields.
/// \param message1
/// \param message2
/// \return
bool isMessageEqualInbox(InboxMessage message1, InboxMessage message2)
{
    if ( message1.sender != message2.sender )
        return false;
    if ( message1.created_at != message2.created_at )
        return false;
    if ( 0 != strcmp( message1.body, message2.body ) )
        return false;

    return true;
}

/// Resolves AEM index (in $CLIENT_AEM_LIST array) of given $device, if not already resolved.
/// \param device
/// \return
inline int32_t resolveAemIndex( Device device )
{
    if ( -1 == device.aemIndex || ( 0 == device.aemIndex && device.AEM != CLIENT_AEM_LIST[0] ) )
        return binary_search_index( CLIENT_AEM_LIST, CLIENT_AEM_LIST_LENGTH, device.AEM );

    return device.aemIndex;
}

/// \brief Tries to connect via socket to given AEM (creating respective IP address) & port.
/// \param aem
/// \param port
/// \return -1 on error, opened socket's file descriptor on success
int socket_connect( uint32_t aem, uint16_t port )
{
    int socket_fd;
    struct sockaddr_in serverAddress;
    const char *ip;

    if ( CLIENT_AEM == aem || devices_exists_aem( aem ) )
        return -1;

    ip = aem2ip( aem );
    fprintf( stdout, "\tsocket_connect(): ip = \"%s\"\n", ip );

    socket_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( socket_fd < 0 )
    {
        perror("\tsocket_connect(): socket() failed" );
        error( socket_fd,"socket_connect(): socket failed" );
    }

    // Set "server" address
    bzero((char *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons( port );
    serverAddress.sin_addr.s_addr = inet_addr( ip );
    
    return connect( socket_fd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr) ) >= 0 ?
            socket_fd : -1;
}

/// \brief Convert given UNIX timestamp to a formatted datetime string with given $format.
/// \param timestamp UNIX timestamp ( uint64_t )
/// \param format strftime-compatible format
/// \return the resulting datetime string
const char* timestamp2ftime( const uint64_t timestamp, const char *format )
{
//    // Format datetime stings in Greek
//    setlocale( LC_TIME, "el_GR.UTF-8" );

    static char* returnString;
    struct tm* tmp;

    returnString = malloc( 50 * sizeof( char ) );
    tmp = localtime((const time_t *) &( timestamp ));
    if ( tmp == NULL )
    {
        perror( "\ttimestamp2ftime(): localtime() error" );
        exit( EXIT_FAILURE );
    }
    if ( 0 == strftime( returnString, UINT32_MAX, format, tmp ) )
    {
        fprintf( stderr, "\ttimestamp2ftime(): strftime() error" );
        exit( EXIT_FAILURE );
    }

    return returnString;
}