#include "server.h"

extern pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
extern uint8_t communicationThreadsAvailable;

extern pthread_mutex_t availableThreadsLock;
extern pthread_mutex_t messagesBufferLock;

/// \brief Push $message to $messages circle buffer. Updates $messageHead acc. to selected override policy.
/// \param message
void messages_push(Message message)
{
    switch ( MESSAGES_PUSH_OVERRIDE_POLICY )
    {
        case MESSAGES_PUSH_OVERRIDE_SENT_ONLY:
        {
            // If buffer filled once, then override only sent
            if ( messagesHead > MESSAGES_SIZE )
            {
                uint16_t messagesHeadModSize = (uint16_t) (messagesHead % MESSAGES_SIZE);

                while (
                    /* while message[i] is not transmitted */
                    0 == messages[ messagesHeadModSize++ ].transmitted && ++messagesHead
                    && messagesHeadModSize < MESSAGES_SIZE
                );
                if ( messagesHeadModSize == MESSAGES_SIZE )
                {
                    // if reached end of buffer and did not succeed in finding a free spot then override message at
                    // initial messagesHead's position ( to avoid possible endless loop )
                    goto default_case;
                }
                else
                {
                    messagesHead--;
                    messagesHeadModSize--;
                }

//                fprintf( stdout, "\t\tmessages_push(MESSAGES_PUSH_OVERRIDE_SENT_ONLY): messagesHeadModSize = %d\n", messagesHeadModSize );

                // push to head
                memcpy( (void *) ( messages + messagesHeadModSize ), (void *) &message, sizeof( Message ) );

                break;
            }
        }
        case MESSAGES_PUSH_OVERRIDE_BLIND:
        default:
        default_case:
        {
            uint16_t messagesHeadModSize = (uint16_t) (messagesHead % MESSAGES_SIZE);

            // push to head
            memcpy( (void *) ( messages + messagesHeadModSize ), (void *) &message, sizeof( Message ) );

            // update head
            messagesHead++;
        }
    }
}

/// \brief Main server loop. Calls communication_thread() on each new connection.
void listening_worker()
{
    sleep( 100 );

    int status, server_fd, socket_fd;
    struct sockaddr_in serverAddress, clientAddress;
    char ip[INET_ADDRSTRLEN];
    socklen_t clientAddressLength;
    pthread_t communicationThread;

    status = socket(AF_INET, SOCK_STREAM, 0);
    if ( status < 0 )
        error( status, "\tserver_listen(): socket() failed" );
    server_fd = status;

    // Set server address
    status = 1;     // forcefully attaching socket to selected socket port
    status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &status, sizeof(status) );
    if ( status < 0 )
        error( status, "\tserver_listen(): setsockopt() failed" );

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons( SOCKET_PORT );

    // Bind socket
    status = bind( server_fd, (struct sockaddr *) &serverAddress, sizeof(serverAddress) );
    if ( status < 0 )
        error( status, "\tserver_listen(): bind() failed" );

    // Listen
    status = listen( server_fd, 5 );
    if ( status < 0 )
        error( status, "\tserver_listen(): listen() failed" );

    // Main server loop
    clientAddressLength = sizeof( clientAddress );
    do
    {
        // Accept a new socket connection
        status = accept( server_fd, (struct sockaddr *) &clientAddress, &clientAddressLength );
        if ( status < 0 )
            error( status, "\tserver_listen(): accept() failed" );
        socket_fd = status;

        // Connected > OffLoad to communication worker
        //  - get client address
        inet_ntop( AF_INET, &( clientAddress.sin_addr ), ip, INET_ADDRSTRLEN );

        //  - format arguments
        Device device = {.AEM = ip2aem( ip )};
        CommunicationWorkerArgs args = {.connected_device = device, .connected_socket_fd = (uint16_t) socket_fd};

        //  - open thread
        if ( communicationThreadsAvailable > 0 )
        {
            //----- CRITICAL
            pthread_mutex_lock( &availableThreadsLock );

            communicationThread = communicationThreads[ COMMUNICATION_WORKERS_MAX - communicationThreadsAvailable ];
            communicationThreadsAvailable--;

            pthread_mutex_unlock( &availableThreadsLock );
            //-----:end

            status = pthread_create( &communicationThread, NULL, (void *) communication_worker, &args );
            if ( status != 0 )
                error( status, "\tserver_listen(): pthread_create() failed" );

            status = pthread_detach( communicationThread );
            if ( status != 0 )
                error( status, "\tserver_listen(): pthread_detach() failed" );
        }
        else
        {
            close( socket_fd );
        }
    }
    while (1);
}
