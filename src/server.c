#include "conf.h"
#include "server.h"
#include "log.h"
#include "utils.h"
#include "communication.h"
#include <arpa/inet.h>

//------------------------------------------------------------------------------------------------

extern MessagesStats messagesStats;
extern pthread_mutex_t messagesBufferLock, availableThreadsLock, logLock;

extern pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
extern uint8_t communicationThreadsAvailable;

extern uint32_t CLIENT_AEM;

//------------------------------------------------------------------------------------------------

/* messagesHead is in range: [0, $MESSAGES_SIZE - 1] */
messages_head_t messagesHead;
Message messages[ MESSAGES_SIZE ];

// Active flag for each AEM
bool CLIENT_AEM_ACTIVE_LIST[CLIENT_AEM_LIST_LENGTH] = {false};


/// \brief Check if $device exists $activeDevices FIFO queue.
/// \param device
/// \return uint8 0 if FALSE, 1 if TRUE
bool devices_exists(Device device)
{
    if ( -1 == device.aemIndex || ( 0 == device.aemIndex && device.AEM != CLIENT_AEM_ACTIVE_LIST[0] ) )
        device.aemIndex = binary_search_index( CLIENT_AEM_LIST, CLIENT_AEM_LIST_LENGTH, device.AEM );

    return ( device.aemIndex > -1 ) ?
        ( CLIENT_AEM_ACTIVE_LIST[ device.aemIndex ] == 1 ? true : false ):
        false;
}

/// \brief Check if a device with $aem exists in $activeDevices.
/// \param aem
/// \return uint8 0 if FALSE, 1 if TRUE
bool devices_exists_aem(uint32_t aem)
{
    Device testDevice = {.AEM = aem};
    return devices_exists( testDevice );
}

/// \brief Push $device to activeDevices FIFO queue.
/// \param device
void devices_push(Device device)
{
    if ( -1 == device.aemIndex || ( 0 == device.aemIndex && device.AEM != CLIENT_AEM_ACTIVE_LIST[0] ) )
        device.aemIndex = binary_search_index( CLIENT_AEM_LIST, CLIENT_AEM_LIST_LENGTH, device.AEM );

    if ( device.aemIndex > -1 )
        CLIENT_AEM_ACTIVE_LIST[ device.aemIndex ] = 1;
}

/// \brief Remove $device from $activeDevices FIFO queue.
/// \param device
void devices_remove(Device device)
{
    if ( -1 == device.aemIndex || ( 0 == device.aemIndex && device.AEM != CLIENT_AEM_ACTIVE_LIST[0] ) )
        device.aemIndex = binary_search_index( CLIENT_AEM_LIST, CLIENT_AEM_LIST_LENGTH, device.AEM );

    if ( device.aemIndex > -1 )
        CLIENT_AEM_ACTIVE_LIST[ device.aemIndex ] = 0;
}

/// \brief Push $message to $messages circle buffer. Updates $messageHead acc. to selected override policy.
/// \param message
void messages_push(Message message)
{
    // Find where to place new message
    if ( !strcmp( "sent_only", MESSAGES_PUSH_OVERRIDE_POLICY ) )
    {
        messages_head_t messagesHeadOriginal = messagesHead;

        // start searching for a hole from buffer's head
        do
        {
            if ( 0 == messages[messagesHead].created_at ) break;    // found empty message: hole
            if ( messages[messagesHead].transmitted ) break;   // found message that was transmitted: "hole"
        }
        while ( ++messagesHead < MESSAGES_SIZE );

        // if reached buffer's end, rewind at start and search until original buffer's head
        if ( MESSAGES_SIZE == messagesHead )
        {
            messagesHead = 0;

            while ( messagesHead < messagesHeadOriginal )
            {
                if ( 0 == messages[messagesHead].created_at ) break;
                if ( messages[messagesHead].transmitted ) break;

                messagesHead++;
            }

            // after this loop, if no break executed, messagesHead === messagesHeadOriginal
        }
    }

    // Place message at buffer's head
    memcpy( (void *) ( messages + messagesHead ), (void *) &message, sizeof( Message ) );

    // Increment head
    if ( ++messagesHead == MESSAGES_SIZE )
    {
        messagesHead = 0;
    }

//    pthread_mutex_lock( &logLock );
//        inspect_messages( true );
//    pthread_mutex_unlock( &logLock );
}

/// \brief Main server loop. Calls communication_thread() on each new connection.
void listening_worker()
{
    int server_socket_fd;
    int client_socket_fd;
    int status;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    char logMessage[LOG_MESSAGE_MAX_LEN];

    // Create the server ( parent ) socket
    server_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket_fd < 0)
        error(server_socket_fd, "ERROR opening socket");

    // Build the server's Internet address
    bzero((char *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr( aem2ip( CLIENT_AEM ) );
    serverAddress.sin_port = htons((unsigned short)SOCKET_PORT);

    /* setsockopt: Handy debugging trick that lets
     * us rerun the server immediately after we kill it;
     * otherwise we have to wait about 20 secs.
     * Eliminates "ERROR on binding: Address already in use" error.
     */
    status = 1;
    if ( setsockopt( server_socket_fd, SOL_SOCKET, SO_REUSEPORT, (const void *)&status, sizeof(int) ) < 0 )
        perror("setsockopt ( SO_REUSEPORT )");

    struct linger lin;
    lin.l_onoff = 0;
    lin.l_linger = 0;
    if ( setsockopt( server_socket_fd, SOL_SOCKET, SO_LINGER, (const void *)&lin, sizeof(struct linger) ) < 0 )
        perror("setsockopt ( SO_LINGER )");

    // Associate the parent socket with a port
    if ( ( status = bind( server_socket_fd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in) ) < 0 ) )
        error(status, "ERROR on binding");

    listen( server_socket_fd, SOCKET_LISTEN_QUEUE_LEN );

//    pthread_mutex_lock( &logLock );
//        log_info( "Started listening loop! Waiting in accept()...", "listening_worker()", "-" );
//    pthread_mutex_unlock( &logLock );
    while (1)
    {
        client_socket_fd = accept(server_socket_fd, (struct sockaddr *) &clientAddress, &(socklen_t){ sizeof( struct sockaddr_in ) } );
        if (client_socket_fd < 0)
            error(client_socket_fd, "ERROR on accept");

        // Connected > OffLoad to communication worker
        //  - get client address
        char ip[INET_ADDRSTRLEN];
        inet_ntop( AF_INET, &( clientAddress.sin_addr ), ip, INET_ADDRSTRLEN );

        //  - format arguments
        uint32_t clientAem = ip2aem(ip );
        Device device = {
                .AEM = clientAem,
                .aemIndex = binary_search_index(CLIENT_AEM_LIST, CLIENT_AEM_LIST_LENGTH, clientAem )
        };
        CommunicationWorkerArgs args = {
                .connected_socket_fd = (uint16_t) client_socket_fd,
                .server = true
        };
        memcpy( &args.connected_device, &device, sizeof( Device ) );

        // Log
//        pthread_mutex_lock( &logLock );
//            sprintf( logMessage, "Connected: AEM = %04d ( index = %02d )", device.AEM, device.aemIndex );
//            log_info( logMessage, "listening_worker()", "socket.h > accept()" );
//        pthread_mutex_unlock( &logLock );

        //  - open thread
        if ( communicationThreadsAvailable > 0 )
        {
            //----- CRITICAL SECTION
            pthread_mutex_lock( &availableThreadsLock );

            pthread_t communicationThread = communicationThreads[ COMMUNICATION_WORKERS_MAX - communicationThreadsAvailable ];
            communicationThreadsAvailable--;

            pthread_mutex_unlock( &availableThreadsLock );
            //-----:end

            args.concurrent = true;

            status = pthread_create( &communicationThread, NULL, (void *) communication_worker, &args );
            if ( status != 0 )
                error( status, "\tserver_listen(): pthread_create() failed" );

            status = pthread_detach( communicationThread );
            if ( status != 0 )
                error( status, "\tserver_listen(): pthread_detach() failed" );
        }
        else
        {
            // run in main thread
            args.concurrent = false;
            communication_worker( &args );
        }

        // Log
//        pthread_mutex_lock( &logLock );
//        sprintf( logMessage, "Finished: AEM = %04d", device.AEM );
//        log_info( logMessage, "listening_worker()", "socket.h > accept()" );
//        pthread_mutex_unlock( &logLock );
    }
}
