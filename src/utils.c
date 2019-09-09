#include <conf.h>
#include <utils.h>
#include <log.h>
#include <server.h>

extern uint32_t CLIENT_AEM;

extern pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
extern uint8_t communicationThreadsAvailable;

extern pthread_mutex_t availableThreadsLock, messagesBufferLock, activeDevicesLock, messagesStatsLock;
extern MessagesStats messagesStats;

extern Message messages[ MESSAGES_SIZE ];


//------------------------------------------------------------------------------------------------


/// \brief Handle communication staff with connected device ( POSIX thread compatible function ).
/// \param thread_args pointer to communicate_args_t type
void communication_worker(void *thread_args)
{
    CommunicationWorkerArgs *args = (CommunicationWorkerArgs *) thread_args;
    uint8_t deviceExists;
    int status;

    char logMessage[100];

    // Check if there is an active connection with given device
    //----- CRITICAL SECTION
    pthread_mutex_lock( &activeDevicesLock );

    deviceExists = devices_exists( args->connected_device );
    if ( 0 == deviceExists )
    {
        // Update active devices
        devices_push( args->connected_device );
    }

    pthread_mutex_unlock( &activeDevicesLock );
    //-----:end

    // If no active connection with given device exists
    if ( 0 == deviceExists )
    {
        // Receiver Thread ( in a new thread )
        if ( communicationThreadsAvailable > 2 )
        {
            pthread_t receiverThread;

            //----- CRITICAL SECTION
            pthread_mutex_lock( &availableThreadsLock );

            receiverThread = communicationThreads[ COMMUNICATION_WORKERS_MAX - communicationThreadsAvailable ];
            communicationThreadsAvailable--;

            pthread_mutex_unlock( &availableThreadsLock );
            //-----:end

            status = pthread_create( &receiverThread, NULL, (void *) communication_receiver_worker, &thread_args );
            if ( status != 0 )
                error( status, "\tcommunication_worker(): pthread_create() failed" );

            // Transmitter Thread ( main thread )
            communication_transmitter_worker( args->connected_device, args->connected_socket_fd );

            // Wait receiver thread
            status = pthread_join( receiverThread, NULL );
            if ( status != 0 )
                error( status, "\tcommunication_worker(): pthread_join() failed" );

            // Update number of threads
            //----- CRITICAL SECTION
            pthread_mutex_lock( &availableThreadsLock );

            communicationThreadsAvailable++;

            pthread_mutex_unlock( &availableThreadsLock );
            //-----:end
        }
        else
        {
            // Serial Execution
            communication_transmitter_worker( args->connected_device, args->connected_socket_fd );
            communication_receiver_worker( thread_args );
        }

        // Update active devices
        //----- CRITICAL SECTION
        pthread_mutex_lock( &activeDevicesLock );

        devices_remove( args->connected_device );

        pthread_mutex_unlock( &activeDevicesLock );
        //-----:end
    }
    else
    {
        sprintf( logMessage, "Active connection with device found: AEM = %04d. Skipping...", args->connected_device.AEM );
        log_error( logMessage, "communication_worker()", 0 );
    }

    // Close Socket
    close( args->connected_socket_fd );

    // Update number of threads ( since, if this function is called in a new thread, then that thread was detached )
    if ( 1 == args->concurrent )
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

    messageSerialized = (char *) malloc( 277 );
    next_loop: while ( read( args->connected_socket_fd , messageSerialized, 277 ) == 277 )
    {
        // Reconstruct message
        message = explode( "_", messageSerialized );

        // Check for duplicates
        for ( uint16_t message_i = 0; message_i < MESSAGES_SIZE; message_i++ )
        {
            if ( 1 == isMessageEqual( message, messages[message_i] ) )
                goto next_loop;

            if ( messages[message_i].created_at == 0 )
                break;
        }

        // Store in $messages buffer
        //----- CRITICAL SECTION
        pthread_mutex_lock( &messagesBufferLock );

        messages_push( message );

        pthread_mutex_unlock( &messagesBufferLock );
        //-----:end

        // Log received message
        log_message( "communication_receiver_worker()", message );

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

    char logMessage[100];

    messageSerialized = (char *) malloc( 277 );
    for ( uint16_t message_i = 0; message_i < MESSAGES_SIZE; message_i++ )
    {
        if ( messages[message_i].created_at > 0 && 0 == isDeviceEqual( receiverDevice, messages[message_i].transmitted_device ) )
        {
            // Serialize
            implode( "_", messages[message_i], messageSerialized );

            // Log
            sprintf( logMessage, "sending message: \"%s\"", messageSerialized );
            log_info( logMessage, "communication_transmitter_worker()", "socket.h > send()" );

            // Transmit
            send( socket_fd , messageSerialized , 277, 0 );

            // Update Status in $messages buffer
            //----- CRITICAL SECTION
            pthread_mutex_lock( &messagesBufferLock );

            messages[message_i].transmitted = 1;
            memcpy( &messages[message_i].transmitted_device, &receiverDevice, sizeof( Device ) );

            pthread_mutex_unlock( &messagesBufferLock );
            //-----:end

            // Update stats
            //----- CRITICAL SECTION
            pthread_mutex_lock( &messagesStatsLock );

            messagesStats.produced++;

            pthread_mutex_unlock( &messagesStatsLock );
            //-----:end
        }
    }

    // Free resources
    free( messageSerialized );
}

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
    message.created_at = (uint64) strtoll( strsep( &messageCopy, glue ), (char **)NULL, 10 );

    memcpy( message.body, strsep( &messageCopy, glue ), 256 );
    free( messageCopyPointer );

    // Set message's metadata
    message.transmitted = 0;

    return message;
}

/// \brief Generates a new message from this client towards $recipient with $body as content.
/// \param recipient message's recipient
/// \param body message's body
/// \return newly generated message of type message_t
Message generateMessage(uint32_t recipient, const char * body)
{
    Message message;

    message.sender = CLIENT_AEM;
    message.recipient = recipient;
    message.created_at = (uint64) time( NULL );

    memcpy( message.body, body, 256 );

    message.transmitted = 0;

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

    //  - random recipient ( using sodium )
    recipient = (uint32_t) (rand() % (CLIENT_AEM_RANGE_MAX + 1 - CLIENT_AEM_RANGE_MIN ) + CLIENT_AEM_RANGE_MIN);

    //  - random body ( using sodium )
    for ( int byte_i = 0; byte_i < 255; byte_i ++ )
    {
        body[byte_i] = (char) ( rand() % (95 - 32 + 1) + 32);
    }
    body[255] = '\0';

    Message message = generateMessage( recipient, body );

    //  - random transmission status
//    message.transmitted = (uint8_t) (rand() % 2 == 1 ? 1 : 0);
    message.transmitted = 0;
    if ( message.transmitted == 1 )
    {
        for( uint8_t byte_i = 0; byte_i < 6; byte_i++ )
            message.transmitted_device.mac[byte_i] = (unsigned char) (rand() % 255);
    }

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

/// \brief Log ( to file pointer ) message's fields.
/// \param message
/// \param metadata show/hide metadata information from message
void inspect(const Message message, uint8_t metadata, FILE *fp)
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
        char hex[18];

        message.transmitted == 1 ?
            mac2hex( message.transmitted_device.mac, hex ):
            snprintf( hex, 18, "--:--:--:--:--:--" );

        fprintf( fp, "\t---\n\ttransmitted = %d\n\ttransmitted_device = %s\n", message.transmitted, hex );
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
    ipParts[0] = (unsigned char) strtol( strsep( &ipCopy, glue ), (char **)NULL, 10 );
    ipParts[1] = (unsigned char) strtol( strsep( &ipCopy, glue ), (char **)NULL, 10 );
    ipParts[2] = (unsigned char) strtol( strsep( &ipCopy, glue ), (char **)NULL, 10 );
    ipParts[3] = (unsigned char) strtol( strsep( &ipCopy, glue ), (char **)NULL, 10 );

    // Compose AEM
    aem = (uint32_t) ( ipParts[2] * 100 );
    aem += (uint32_t) ipParts[3];

    free( ipCopyPointer );
    return aem;
}

/// \brief Check if two devices have exactly the same values in ALL of their fields.
/// \param device1
/// \param device2
/// \return
uint8_t isDeviceEqual(Device device1, Device device2)
{
    return (uint8_t) (device1.AEM == device2.AEM ? 1 : 0);
}

/// \brief Check if two messages have exactly the same values in ALL of their fields.
/// \param message1
/// \param message2
/// \return
uint8_t isMessageEqual(Message message1, Message message2)
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

/// Convert MAC address from byte array to string ( adding ':' between successive bytes )
/// \param mac mac address as array of bytes ( 'byte' is 'unsigned char' in C )
/// \param hex pointer to the HEX string of the MAC address
void mac2hex(const unsigned char *mac, char *hex)
{
    sprintf(hex, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
    hex[18] = '\0';
}

extern struct sockaddr_in socket_connect__serv_addr;

/// \brief Tries to connect via socket to given IP address & port.
/// \param ip the IP address to open socket to
/// \return -1 on error, opened socket's file descriptor on success
int socket_connect(const char * ip)
{
    int socket_fd = 0, status;
    char ipp[INET_ADDRSTRLEN];

    status = socket( AF_INET, SOCK_STREAM, 0 );
    if ( status < 0 )
        error( status, "\tsocket_connect(): socket() failed" );

    socket_fd = status;

    // Convert IPv4 and IPv6 addresses from text to binary form
    sprintf( ipp, "%s", ip );
    status = inet_pton( AF_INET, ipp, &( socket_connect__serv_addr.sin_addr ) );
    if ( status < 0 )
        error( status, "\tsocket_connect(): inet_pton() failed" );
    else if ( 0 == status )
    {
        return -1;
    }

    return ( 0 == connect( socket_fd, (struct sockaddr *)&socket_connect__serv_addr, sizeof(socket_connect__serv_addr) ) ) ?
        socket_fd : -1;
}

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