#include "conf.h"
#include "server.h"
#include "log.h"
#include "utils.h"
#include "communication.h"
#include <arpa/inet.h>

//------------------------------------------------------------------------------------------------

extern MessagesStats messagesStats;
extern pthread_mutex_t messagesBufferLock, availableThreadsLock;

extern pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
extern uint8_t communicationThreadsAvailable;

extern uint32_t CLIENT_AEM;

//------------------------------------------------------------------------------------------------

/* messagesHead is in range: [0, $MESSAGES_SIZE - 1] */
messages_head_t messagesHead;
messages_head_t inboxHead;
Message MESSAGES_BUFFER[ MESSAGES_SIZE ];
InboxMessage INBOX[ INBOX_SIZE ];

// Active flag for each AEM
bool CLIENT_AEM_ACTIVE_LIST[ CLIENT_AEM_LIST_LENGTH ] = {false};


/// \brief Check if $device exists $activeDevices FIFO queue.
/// \param device
/// \return uint8 0 if FALSE, 1 if TRUE
bool devices_exists(Device device)
{
    device.aemIndex = resolveAemIndex( device );
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
    device.aemIndex = resolveAemIndex( device );
    if ( device.aemIndex > -1 )
        CLIENT_AEM_ACTIVE_LIST[ device.aemIndex ] = 1;
}

/// \brief Remove $device from $activeDevices FIFO queue.
/// \param device
void devices_remove(Device device)
{
    device.aemIndex = resolveAemIndex( device );
    if ( device.aemIndex > -1 )
        CLIENT_AEM_ACTIVE_LIST[ device.aemIndex ] = 0;
}

/// Push message to $INBOX buffer, checking for existence.
/// \param message
/// \param device used to keep stats of the first device that gave us our message
void inbox_push(Message *message, Device *device)
{
    // Cast Message to InboxMessage
    InboxMessage inboxMessage = {
            .sender = message->sender,
            .created_at = message->created_at,
            .saved_at = (uint64_t) time( NULL ),
            .first_sender = device->AEM
    };
    strcpy( inboxMessage.body, message->body );

    // Check if message exists
    for (uint16_t inbox_message_i = 0; inbox_message_i < inboxHead; inbox_message_i++ )
    {
        if ( 1 == isMessageEqualInbox( inboxMessage, INBOX[inbox_message_i] ) )
            return;

        if ( INBOX[inbox_message_i].created_at == 0 )
            break;
    }

    // Place message at buffer's head
    memcpy((void *) ( INBOX + inboxHead ), (void *) &inboxMessage, sizeof( InboxMessage ) );

    // Increment head
    inboxHead++;

    // Update stats
    messagesStats.received_for_me++;
}

/// \brief Push $message to $messages circle buffer. Updates $messageHead acc. to selected override policy.
/// \param message
void messages_push(Message *message)
{
    // Find where to place new message
    if ( 0 == strcmp( "sent_only", MESSAGES_PUSH_OVERRIDE_POLICY ) )
    {
        messages_head_t messagesHeadOriginal = messagesHead;

        // start searching for a hole from buffer's head
        do
        {
            if (0 == MESSAGES_BUFFER[messagesHead].created_at ) break;    // found empty message: hole
            if ( MESSAGES_BUFFER[messagesHead].transmitted ) break;       // found message that was transmitted: "hole"
        }
        while ( ++messagesHead < MESSAGES_SIZE );

        // if reached buffer's end, rewind at start and search until original buffer's head
        if ( MESSAGES_SIZE == messagesHead )
        {
            messagesHead = 0;

            while ( messagesHead < messagesHeadOriginal )
            {
                if (0 == MESSAGES_BUFFER[messagesHead].created_at ) break;
                if ( MESSAGES_BUFFER[messagesHead].transmitted ) break;

                messagesHead++;
            }

            // after this loop, if no break executed, messagesHead === messagesHeadOriginal
        }
    }

    // Place message at buffer's head
    memcpy((void *) (MESSAGES_BUFFER + messagesHead ), (void *) message, sizeof( Message ) );

    // Increment head
    if ( ++messagesHead == MESSAGES_SIZE )
    {
        messagesHead = 0;
    }
}

/// \brief Main server loop. Calls communication_thread() on each new connection.
void listening_worker()
{
    int server_socket_fd;
    int client_socket_fd;
    int status;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;

    // Create the server ( parent ) socket
    server_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket_fd < 0)
        error(server_socket_fd, "ERROR opening socket");

    // Build the server's Internet address
    bzero((char *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr( aem2ip( CLIENT_AEM ) );
    serverAddress.sin_port = htons((unsigned short)SOCKET_PORT );

    // setsockopt: Handy debugging trick that lets us rerun the server immediately after we kill it;
    // otherwise we have to wait about 20 secs. Eliminates "ERROR on binding: Address already in use" error.
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
                .aemIndex = binary_search_index( CLIENT_AEM_LIST, CLIENT_AEM_LIST_LENGTH, clientAem )
        };
        CommunicationWorkerArgs args = {
                .connected_socket_fd = (int32_t) client_socket_fd,
                .server = true
        };
        memcpy( &args.connected_device, &device, sizeof( Device ) );

        //  - open thread
        if ( communicationThreadsAvailable > 0 )
        {
            // Update available treads count
            pthread_mutex_lock( &availableThreadsLock );
                pthread_t communicationThread = communicationThreads[ COMMUNICATION_WORKERS_MAX - communicationThreadsAvailable ];
                communicationThreadsAvailable--;
            pthread_mutex_unlock( &availableThreadsLock );

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
    }
}
