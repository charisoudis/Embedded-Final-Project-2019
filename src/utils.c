#include <conf.h>
#include <utils.h>
#include <log.h>
#include <server.h>

//------------------------------------------------------------------------------------------------


extern uint32_t CLIENT_AEM;

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
uint32_t binary_search_index(const int32_t *haystack, size_t N, int32_t needle)
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
            return middle;
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
    bool deviceExists;

    char logMessage[LOG_MESSAGE_MAX_LEN];

    // Check if there is an active connection with given device
    //----- CRITICAL SECTION
    pthread_mutex_lock( &activeDevicesLock );

    deviceExists = devices_exists( args->connected_device );
    if ( !deviceExists )
    {
        // Update active devices
        devices_push( args->connected_device );
    }

    pthread_mutex_unlock( &activeDevicesLock );
    //-----:end

    // If no active connection with given device exists
    if ( !deviceExists )
    {
        // If device is server, act as transmitter, else act as receiver.
        args->server ?
            communication_transmitter_worker( args->connected_device, args->connected_socket_fd ):
            communication_receiver_worker( thread_args );

        // Update active devices
        //----- CRITICAL SECTION
        pthread_mutex_lock( &activeDevicesLock );

        devices_remove( args->connected_device );

        pthread_mutex_unlock( &activeDevicesLock );
        //-----:end
    }
    else
    {
        //----- CRITICAL SECTION
        pthread_mutex_lock( &logLock );
        sprintf( logMessage, "Active connection with device found: AEM = %04d. Skipping...", args->connected_device.AEM );
        log_error( logMessage, "communication_worker()", 0 );
        pthread_mutex_unlock( &logLock );
        //-----:end
    }

    // Close Socket
    args->server ?
        close( args->connected_socket_fd ):
        shutdown( args->connected_socket_fd, SHUT_RDWR );

    // Update number of threads ( since, if this function is called in a new thread, then that thread was detached )
    if ( args->concurrent )
    {
        //----- CRITICAL SECTION
        pthread_mutex_lock( &availableThreadsLock );

        communicationThreadsAvailable++;

        pthread_mutex_unlock( &availableThreadsLock );
        //-----:end
    }
}

/// \brief Receiver sub-worker of communication worker ( POSIX thread compatible function ).
/// \param thread_args pointer to communicate_args_t type
void communication_receiver_worker(void *thread_args)
{
    CommunicationWorkerArgs *args = (CommunicationWorkerArgs *) thread_args;

    Message message;
    MessageSerialized messageSerialized;
    char logMessage[LOG_MESSAGE_MAX_LEN];

    messageSerialized = (char *) malloc( MESSAGE_SERIALIZED_LEN );
    next_loop:
    while ( read( args->connected_socket_fd , messageSerialized, MESSAGE_SERIALIZED_LEN ) == MESSAGE_SERIALIZED_LEN )
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
        message.transmitted_devices[ args->connected_device.aemIndex ] = true;

        // Store in $messages buffer
        //----- CRITICAL SECTION
        pthread_mutex_lock( &messagesBufferLock );

        messages_push( message );

        pthread_mutex_unlock( &messagesBufferLock );
        //-----:end

        // Log received message
//        log_message( "communication_receiver_worker()", message );

        // Update stats
        //----- CRITICAL SECTION
        pthread_mutex_lock( &messagesStatsLock );

        messagesStats.received++;

        pthread_mutex_unlock( &messagesStatsLock );
        //-----:end
    }

    // Free resources
    free( messageSerialized );
}

/// \brief Transmitter sub-worker of communication worker ( POSIX thread compatible function ).
/// \param receiverDevice connected device that will receive messages
/// \param socket_fd socket file descriptor with connected device
inline void communication_transmitter_worker(Device receiverDevice, int socket_fd)
{
    MessageSerialized messageSerialized;

    char logMessage[LOG_MESSAGE_MAX_LEN];

    messageSerialized = (char *) malloc( MESSAGE_SERIALIZED_LEN );
    for ( uint16_t message_i = 0; message_i < MESSAGES_SIZE; message_i++ )
    {
        if ( messages[message_i].created_at > 0
//            && !str_exists_aem( messages[message_i].transmitted_device_aem_string, receiverDevice.AEM )
        )
        {
            // Serialize
            implode( "_", messages[message_i], messageSerialized );

            // Log
            //----- CRITICAL SECTION
            pthread_mutex_lock( &logLock );
            sprintf( logMessage, "sending message: \"%s\"", messageSerialized );
            log_info( logMessage, "communication_transmitter_worker()", "socket.h > send()" );
            pthread_mutex_unlock( &logLock );
            //-----:end

            // Transmit
            send( socket_fd , messageSerialized , MESSAGE_SERIALIZED_LEN, 0 );

            // Update Status in $messages buffer
            //----- CRITICAL SECTION
            pthread_mutex_lock( &messagesBufferLock );

            messages[message_i].transmitted = true;
//            str_append_aem( messages[message_i].transmitted_device_aem_string, receiverDevice.AEM, "_" );

            pthread_mutex_unlock( &messagesBufferLock );
            //-----:end

            // Update stats
            //----- CRITICAL SECTION
            pthread_mutex_lock( &messagesStatsLock );

            messagesStats.transmitted++;

            pthread_mutex_unlock( &messagesStatsLock );
            //-----:end
        }
    }

    // Free resources
    free( messageSerialized );
}

/// \brief Un-serializes message-as-a-string, re-creating initial message.
/// \param message the result message ( passes as a pointer )
/// \param glue the connective character(s); acts as the separator between successive message fields
/// \param messageSerialized string containing all message fields glued together using $glue
/// \return a message struct of type message_t
void explode(Message *message, const char * glue, MessageSerialized messageSerialized)
{
    MessageSerialized messageCopy = strdup( messageSerialized );
    void *messageCopyPointer = ( void * ) messageCopy;

    // Start exploding string
    message->sender = (uint32_t) strtol( strsep( &messageCopy, glue ), (char **)NULL, STRSEP_BASE_10 );
    message->recipient = (uint32_t) strtol( strsep( &messageCopy, glue ), (char **)NULL, STRSEP_BASE_10 );
    message->created_at = (uint64) strtoll( strsep( &messageCopy, glue ), (char **)NULL, STRSEP_BASE_10 );

    memcpy( message->body, strsep( &messageCopy, glue ), MESSAGE_BODY_LEN );
    free( messageCopyPointer );

    // Set message's metadata
    message->transmitted = 0;
//    strcpy( message->transmitted_device_aem_string, "" );
}

/// \brief Generates a new message from this client towards $recipient with $body as content.
/// \param message result message ( passed as pointer )
/// \param recipient message's recipient
/// \param body message's body
void generateMessage(Message *message, uint32_t recipient, const char * body)
{
    // Fill
    message->sender = CLIENT_AEM;
    message->recipient = recipient;
    message->created_at = (uint64) time( NULL );

    memcpy( message->body, body, MESSAGE_BODY_LEN );

    message->transmitted = 0;
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

    srand( time( NULL ) );

    //  - random recipient
    recipient = ( CLIENT_AEM_SOURCE == CLIENT_AEM_SOURCE_RANGE ) ?
        (uint32_t) (rand() % (CLIENT_AEM_RANGE_MAX + 1 - CLIENT_AEM_RANGE_MIN ) + CLIENT_AEM_RANGE_MIN):
        CLIENT_AEM_LIST[( rand() % CLIENT_AEM_LIST_LENGTH )];

    //  - random body
    for ( int byte_i = 0; byte_i < MESSAGE_BODY_LEN - 1; byte_i ++ )
    {
        char ch;
        do
        {
            ch = (char) ( rand() % (95 - 32 + 1) + 32);
        }
        while( '_' == ch );

        body[byte_i] = ch;
    }
    body[MESSAGE_BODY_LEN - 1] = '\0';

    generateMessage( message, recipient, body );

    //  - transmission status
    message->transmitted = 0;
//    strncpy( message->transmitted_device_aem_string, "", sizeof( message->transmitted_device_aem_string ) );
}

/// \brief Serializes a message ( of message_t type ) into a 277-characters string.
/// \param glue the connective character(s); to be placed between successive message fields
/// \param message the message to be serialized
/// \param messageSerialized a string containing all message fields glued together using $glue
void implode(const char * glue, const Message message, MessageSerialized messageSerialized)
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
    char created_at_full[50];
    timestamp2ftime( message.created_at, "%a, %d %b %Y @ %T", created_at_full );

    // Print main fields
    fprintf( fp, "message = {\n\tsender = %04d,\n\trecipient = %04d,\n\tcreated_at = %lu ( %s ),\n\tbody = %s\n",
            message.sender, message.recipient, message.created_at, created_at_full, message.body
    );

    // Print metadata
    if ( metadata )
    {
//        fprintf( fp, "\t---\n\ttransmitted = %d\n\ttransmitted_devices = %s\n",
//                message.transmitted, message.transmitted == 1 ? message.transmitted_device_aem_string : "" );
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
        return 0;
    if ( message1.recipient != message2.recipient )
        return 0;
    if ( message1.created_at != message2.created_at )
        return 0;
    if ( 0 != strcmp( message1.body, message2.body ) )
        return 0;

    return 1;
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