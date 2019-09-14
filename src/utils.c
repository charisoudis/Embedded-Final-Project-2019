#include <conf.h>
#include <utils.h>
#include <log.h>
#include <server.h>
#include <stdbool.h>
#include <sys/time.h>

//------------------------------------------------------------------------------------------------

extern uint32_t CLIENT_AEM;
extern struct timeval CLIENT_AEM_CONN_START_LIST[CLIENT_AEM_LIST_LENGTH][MAX_CONNECTIONS_WITH_SAME_CLIENT];
extern struct timeval CLIENT_AEM_CONN_END_LIST[CLIENT_AEM_LIST_LENGTH][MAX_CONNECTIONS_WITH_SAME_CLIENT];
extern uint8_t CLIENT_AEM_CONN_N_LIST[CLIENT_AEM_LIST_LENGTH];

extern pthread_mutex_t messagesBufferLock, activeDevicesLock, availableThreadsLock, messagesStatsLock, logLock;
extern MessagesStats messagesStats;

extern pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
extern uint8_t communicationThreadsAvailable;

extern Message messages[ MESSAGES_SIZE ];

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

/// \brief Handle communication staff with connected device ( POSIX thread compatible function ).
/// \param thread_args pointer to communicate_args_t type
void communication_worker(void *thread_args)
{
    CommunicationWorkerArgs *args = (CommunicationWorkerArgs *) thread_args;
    char logMessage[LOG_MESSAGE_MAX_LEN];
    bool deviceExists;

    // Check if there is an active connection with given device
    // Update active devices
    pthread_mutex_lock( &activeDevicesLock );
        deviceExists = devices_exists( args->connected_device );
        if ( !deviceExists )
            devices_push( args->connected_device );
    pthread_mutex_unlock( &activeDevicesLock );

    // If no active connection with given device exists
    if ( !deviceExists && CLIENT_AEM_CONN_N_LIST[ args->connected_device.aemIndex ] <= MAX_CONNECTIONS_WITH_SAME_CLIENT )
    {
        gettimeofday( &(CLIENT_AEM_CONN_START_LIST[args->connected_device.aemIndex][CLIENT_AEM_CONN_N_LIST[ args->connected_device.aemIndex ]]), NULL );

        // If device is server, act as transmitter, else act as receiver.
        //  - forward communication
        if ( args->server )
        {
            communication_transmitter_worker( args->connected_socket_fd, args->connected_device );
            shutdown( args->connected_socket_fd, SHUT_WR );

            communication_receiver_worker( args->connected_socket_fd, args->connected_device );
            shutdown( args->connected_socket_fd, SHUT_RD );
        }
        //  - reverse communication
        else
        {
            communication_receiver_worker( args->connected_socket_fd, args->connected_device );
            shutdown( args->connected_socket_fd, SHUT_RD );

            communication_transmitter_worker( args->connected_socket_fd, args->connected_device );
            shutdown( args->connected_socket_fd, SHUT_WR );
        }

        // Update connection time stats
        gettimeofday( &(CLIENT_AEM_CONN_END_LIST[args->connected_device.aemIndex][CLIENT_AEM_CONN_N_LIST[ args->connected_device.aemIndex ]]), NULL );

        // Update connection time stats
        CLIENT_AEM_CONN_N_LIST[ args->connected_device.aemIndex ]++;

        // Update active devices
        pthread_mutex_lock( &activeDevicesLock );
            devices_remove( args->connected_device );
        pthread_mutex_unlock( &activeDevicesLock );
    }
    else
    {
        pthread_mutex_lock( &logLock );
            sprintf( logMessage, deviceExists ?
                "Active connection with device found: AEM = %04d. Skipping...":
                "Max no. of connections with device reached: AEM = %04d. Skipping...", args->connected_device.AEM );
            log_error( logMessage, "communication_worker()", 0 );
        pthread_mutex_unlock( &logLock );
    }

    // Close Socket
    close( args->connected_socket_fd );

    // Update number of threads ( since, if this function is called in a new thread, then that thread was detached )
    if ( args->concurrent )
    {
        pthread_mutex_lock( &availableThreadsLock );
            communicationThreadsAvailable++;
        pthread_mutex_unlock( &availableThreadsLock );
    }
}

/// \brief Receiver sub-worker of communication worker ( POSIX thread compatible function ).
/// \param connectedSocket socket file descriptor with connected device
/// \param connectedDevice connected device that will send messages
void communication_receiver_worker(int32_t connectedSocket, Device connectedDevice)
{
    Message message;
    char messageSerialized[MESSAGE_SERIALIZED_LEN];
    char logMessage[LOG_MESSAGE_MAX_LEN];

    next_loop:
    while ( read( connectedSocket, messageSerialized, MESSAGE_SERIALIZED_LEN ) == MESSAGE_SERIALIZED_LEN )
    {
        //----- CRITICAL SECTION
        pthread_mutex_lock( &logLock );
        sprintf( logMessage, "received message: \"%s\"", messageSerialized );
        log_info( logMessage, "communication_transmitter_worker()", "socket.h > send()" );
        pthread_mutex_unlock( &logLock );
        //-----:end

        // Reconstruct message
        explode( &message, "_", messageSerialized );

        // Check for duplicates
        for ( uint16_t message_i = 0; message_i < MESSAGES_SIZE; message_i++ )
        {
            if ( 1 == isMessageEqual( message, messages[message_i] ) )
                goto next_loop;

            if ( messages[message_i].created_at == 0 )
                break;
        }

        // Update message's transmitted devices to include sender ( so as not to send back )
        message.transmitted_devices[ connectedDevice.aemIndex ] = 1;

        // Store in $messages buffer
        pthread_mutex_lock( &messagesBufferLock );
            messages_push( message );
        pthread_mutex_unlock( &messagesBufferLock );

        // Log received message
        log_message( "communication_receiver_worker()", message );

        // Update stats
        pthread_mutex_lock( &messagesStatsLock );
            messagesStats.received++;
        pthread_mutex_unlock( &messagesStatsLock );
    }
}

/// \brief Transmitter sub-worker of communication worker ( POSIX thread compatible function ).
/// \param connectedSocket socket file descriptor with connected device
/// \param connectedDevice connected device that will receive messages
void communication_transmitter_worker(int32_t connectedSocket, Device connectedDevice)
{
    char messageSerialized[MESSAGE_SERIALIZED_LEN];
    char logMessage[LOG_MESSAGE_MAX_LEN];

    for ( uint16_t message_i = 0; message_i < MESSAGES_SIZE; message_i++ )
    {
        if (-1 == connectedDevice.aemIndex )
        {
            error(-1, "connectedDevice.aemIndex equals -1. Exiting...");
        }

        if ( messages[message_i].created_at > 0
            && 0 == messages[message_i].transmitted_devices[ connectedDevice.aemIndex ]
        )
        {
            // Serialize
            implode( "_", messages[message_i], messageSerialized );

            // Log
            pthread_mutex_lock( &logLock );
                sprintf( logMessage, "sending message: \"%s\"", messageSerialized );
                log_info( logMessage, "communication_transmitter_worker()", "socket.h > send()" );
            pthread_mutex_unlock( &logLock );

            // Transmit
            send(connectedSocket , messageSerialized , MESSAGE_SERIALIZED_LEN, 0 );

            // Update Status in $messages buffer
            pthread_mutex_lock( &messagesBufferLock );
                messages[message_i].transmitted = 1;
                messages[message_i].transmitted_devices[ connectedDevice.aemIndex ] = 1;
            pthread_mutex_unlock( &messagesBufferLock );

            // Update stats
            pthread_mutex_lock( &messagesStatsLock );
                messagesStats.transmitted++;
            pthread_mutex_unlock( &messagesStatsLock );
        }
    }
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
    recipient = ( !strcmp( "range", CLIENT_AEM_SOURCE ) ) ?
        (uint32_t) (rand() % (CLIENT_AEM_RANGE_MAX + 1 - CLIENT_AEM_RANGE_MIN ) + CLIENT_AEM_RANGE_MIN):
        CLIENT_AEM_LIST[( rand() % CLIENT_AEM_LIST_LENGTH )];

    //  - random body
    char ch;
    for ( int byte_i = 0; byte_i < MESSAGE_BODY_LEN - 1; byte_i ++ )
    {
        do { ch = (char) ( rand() % (MESSAGE_BODY_ASCII_MAX - MESSAGE_BODY_ASCII_MIN + 1) + MESSAGE_BODY_ASCII_MIN); } while( '_' == ch );
        body[byte_i] = ch;
    }
    body[MESSAGE_BODY_LEN - 1] = '\0';

    generateMessage( message, recipient, body );
}

/// \brief Serializes a message ( of message_t type ) into a 277-characters string.
/// \param glue the connective character(s); to be placed between successive message fields
/// \param message the message to be serialized
/// \param messageSerialized a string containing all message fields glued together using $glue
void implode(const char * glue, const Message message, char *messageSerialized)
{
    // Begin copying fields and adding glue
    //  - sender{glue}recipient{glue}created_at{glue}body
    snprintf(messageSerialized, MESSAGE_SERIALIZED_LEN, "%04d%s%04d%s%010ld%s%s",
             message.sender, glue,
             message.recipient, glue,
             message.created_at, glue,
             message.body
    );
}

/// \brief Log ( to file pointer ) message's fields.
/// \param message
/// \param metadata show/hide metadata information from message
void inspect(const Message message, bool metadata, FILE *fp)
{
    // Parse timestamp
    char created_at_full[STRFTIME_STR_LEN];
    timestamp2ftime( message.created_at, "%a, %d %b %Y @ %T", created_at_full );

    // Print main fields
    fprintf( fp, "message = {\n\tsender = %04d,\n\trecipient = %04d,\n\tcreated_at = %lu ( %s ),\n\tbody = %s\n",
            message.sender, message.recipient, message.created_at, created_at_full, message.body
    );

    // Print metadata
    if ( metadata )
    {
        if ( 0 == message.transmitted )
            fprintf( fp, "\t---\n\ttransmitted = FALSE\n\ttransmitted_devices = -\n" );
        else
        {
            char transmittedDevicesString[CLIENT_AEM_LIST_LENGTH * 6];
            uint32_t writePosition;
            uint32_t aem_i;

            for ( writePosition = 0, aem_i = 0; aem_i < CLIENT_AEM_LIST_LENGTH; aem_i++ )
            {
                if ( 1 == message.transmitted_devices[ aem_i ] )
                {
                    snprintf( transmittedDevicesString + writePosition, 6, "%04d,", CLIENT_AEM_LIST[aem_i] );
                    writePosition += 5;
                }
            }

            *( transmittedDevicesString + writePosition - 1 ) = '\0';
            fprintf( fp, "\t---\n\ttransmitted = TRUE\n\ttransmitted_devices = %s\n", transmittedDevicesString );
        }
    }

    fprintf( fp, "}\n" );
}

/// \brief Inspect all messages in $messages buffer.
/// \param inspect_each
void inspect_messages(bool inspect_each)
{
    fprintf( stdout, "\n\ninspection:start\n" );
    for ( uint16_t message_i = 0; message_i < MESSAGES_SIZE; message_i++ )
    {
        if ( messages[message_i].created_at > 0 )
        {
            fprintf( stdout, "\t%02d) %04d --> %04d ( time: %ld ) \n",
                     message_i, messages[message_i].sender,
                     messages[message_i].recipient, messages[message_i].created_at
            );

            if ( inspect_each )
            {
                inspect(messages[message_i], true, stdout );
            }
        }
    }
    fprintf( stdout, "\n\ninspection:end\n" );
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

/// \brief Tries to connect via socket to given IP address & port.
/// \param ip the IP address to open socket to
/// \return -1 on error, opened socket's file descriptor on success
int socket_connect(const char *ip)
{
    uint32_t aem = ip2aem( ip );
    if ( CLIENT_AEM == aem || devices_exists_aem( aem ) )
        return -1;

    int socket_fd;
    int status;
    struct sockaddr_in serverAddress;

    status = socket( AF_INET, SOCK_STREAM, 0 );
    if ( status < 0 )
        error( status, "\tsocket_connect(): socket() failed" );

    socket_fd = status;

    // Set "server" address
    memset( &serverAddress, 0, sizeof(serverAddress) );
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons( SOCKET_PORT );
    serverAddress.sin_addr.s_addr = inet_addr( ip );

    return ( connect( socket_fd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr) ) >= 0 ) ?
        socket_fd : -1;
}

///// \brief Append $new string to $base string ( supp. that $base has been pre-malloc'ed to fit both ).
///// \param base
///// \param new
//void str_append( char *base, char *new )
//{
//    sprintf( base, "%s%s", base, new );
//}
//
///// \brief Append $aem to $base string ( supp. that $base has been pre-malloc'ed to fit both ).
///// \param base
///// \param aem
///// \param sep separator character between successive AEMs
//void str_append_aem( char *base, uint32_t aem, const char *sep )
//{
//    char aemStringWithSep[6];
//    sprintf(aemStringWithSep, "%04d%c", aem, sep[0] );
//
//    return str_append(base, aemStringWithSep );
//}
//
///// \brief Check if $needle exists ( is substring ) in $haystack.
///// \param haystack
///// \param needle
///// \return 0 ( false ) / 1 ( true )
//bool str_exists(const char *haystack, const char *needle)
//{
//    char *p = strstr( haystack, needle );
//    return (p) ? 1 : 0;
//}
//
///// Check if $aem exists in $haystack string.
///// \param haystack
///// \param aem
///// \return
//bool str_exists_aem(const char *haystack, uint32_t aem)
//{
//    char aemString[5];
//    sprintf( aemString, "%04d", aem );
//
//    return str_exists( haystack, aemString );
//}
//
///// \brief Removes $toRemove substring from $str haystack.
///// \param str
///// \param toRemove
//void str_remove(char *str, const char *toRemove)
//{
//    if (NULL == (str = strstr(str, toRemove)))
//    {
//        // no match.
//        printf("No match in %s\n", str);
//        return;
//    }
//
//    // str points to toRemove in str now.
//    const size_t remLen = strlen(toRemove);
//    char *copyEnd;
//    char *copyFrom = str + remLen;
//    while (NULL != (copyEnd = strstr(copyFrom, toRemove)))
//    {
//        //printf("match at %3ld in %s\n", copyEnd - str, str);
//        memmove(str, copyFrom, copyEnd - copyFrom);
//        str += copyEnd - copyFrom;
//        copyFrom = copyEnd + remLen;
//    }
//    memmove(str, copyFrom, 1 + strlen(copyFrom));
//}

/// \brief Convert given UNIX timestamp to a formatted datetime string with given $format.
/// \param timestamp UNIX timestamp ( uint64 )
/// \param format strftime-compatible format
/// \param string the resulting datetime string
inline void timestamp2ftime( const unsigned long timestamp, const char *format, char *string )
{
//    // Format datetime stings in Greek
//    setlocale( LC_TIME, "el_GR.UTF-8" );

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