#include "server.h"

extern pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
extern uint8_t communicationThreadsAvailable;

extern pthread_mutex_t availableThreadsLock;
extern pthread_mutex_t messagesBufferLock;

extern ActiveDevicesQueue activeDevicesQueue;   // FIFO


//------------------------------------------------------------------------------------------------


/* messagesHead is in range: [0, $MESSAGES_SIZE - 1] */
messages_head_t messagesHead;

Message messages[ MESSAGES_SIZE ];


/// Check if $device exists $activeDevices FIFO queue.
/// \param device
uint8_t devices_exists(Device device)
{
    for (devices_head_t device_i = 0; device_i < ACTIVE_SOCKET_CONNECTIONS_MAX; ++device_i)
    {
        if ( 1 == isDeviceEqual( device, activeDevicesQueue.devices[device_i] ) )
            return 1;
    }

    return 0;
}

/// Push $device to activeDevices FIFO queue.
/// \param device
void devices_push(Device device)
{
    // Place device at activeDevices queue's tail
    memcpy( (void *) ( activeDevicesQueue.devices + activeDevicesQueue.tail ), (void *) &device, sizeof( Device ) );

    // Increment tail
    if ( ++activeDevicesQueue.tail == ACTIVE_SOCKET_CONNECTIONS_MAX )
    {
        activeDevicesQueue.tail = 0;
    }

    if ( activeDevicesQueue.tail == activeDevicesQueue.head )
    {
        activeDevicesQueue.tail = (devices_head_t) -1;
    }
}

/// Remove $device from $activeDevices FIFO queue.
/// \param device
void devices_remove(Device device)
{
    // Remove device from queue's head
    if ( 0 == isDeviceEqual( device, activeDevicesQueue.devices[activeDevicesQueue.head] ) )
    {
        devices_head_t device_i;

        // find device's index
        for ( device_i = 0; device_i < ACTIVE_SOCKET_CONNECTIONS_MAX; device_i++ )
            if ( 1 == isDeviceEqual( device, activeDevicesQueue.devices[device_i] ) ) break;

        // Device that has to be removed is not located on queue's head ( as it should be supp. equal communication time )
        // move all devices to the right and virtually place device in head
        if ( activeDevicesQueue.head < activeDevicesQueue.tail )
        {
            if ( activeDevicesQueue.head < device_i && device_i <= activeDevicesQueue.tail )
            {
                // move all previous one position to the right
                do
                {
                    // devices[device_i] <-- devices[device_i - 1]
                    memcpy( activeDevicesQueue.devices + device_i, activeDevicesQueue.devices + device_i - 1, sizeof( Device ) );
                }
                while( --device_i >= activeDevicesQueue.head );
            }
            else
            {
                error( -1, "\tdevices_remove(): device_i is is not in range: [head + 1, tail]" );
            }
        }
        else if ( activeDevicesQueue.head > activeDevicesQueue.tail )
        {
            if ( device_i <= activeDevicesQueue.head && device_i > activeDevicesQueue.tail )
            {
                error( -1, "\tdevices_remove(): device_i is is not in one of ranges: [head + 1, N] OR [0, tail]" );
            }
            else
            {
                // move all previous one position to the right
                //  - 1 ... device_i
                do
                {
                    // devices[device_i] <-- devices[device_i - 1]
                    memcpy( activeDevicesQueue.devices + device_i, activeDevicesQueue.devices + device_i - 1, sizeof( Device ) );
                }
                while( --device_i > 0 );

                //  - 0 <-- N
                device_i = ACTIVE_SOCKET_CONNECTIONS_MAX - 1;
                memcpy( activeDevicesQueue.devices, activeDevicesQueue.devices + device_i, sizeof( Device ) );

                //  - head ... device_i
                do
                {
                    // devices[device_i] <-- devices[device_i - 1]
                    memcpy( activeDevicesQueue.devices + device_i, activeDevicesQueue.devices + device_i - 1, sizeof( Device ) );
                }
                while( --device_i >= activeDevicesQueue.head );
            }
        }
        else
        {
            error( -1, "\tdevices_remove(): queue is empty ( head == tail )" );
        }

        for (devices_head_t i = (devices_head_t) (activeDevicesQueue.head + 1); i < ACTIVE_SOCKET_CONNECTIONS_MAX; ++i)
        {
            if ( 1 == isDeviceEqual( device, activeDevicesQueue.devices[i] ) )
            {
                break;
            }
        }
    }
    else
    {
        activeDevicesQueue.devices[activeDevicesQueue.head].AEM = 0;
    }

    if ( ++activeDevicesQueue.head == ACTIVE_SOCKET_CONNECTIONS_MAX )
    {
        activeDevicesQueue.head = 0;
    }
}

/// \brief Push $message to $messages circle buffer. Updates $messageHead acc. to selected override policy.
/// \param message
void messages_push(Message message)
{
    // Find where to place new message
    if ( MESSAGES_PUSH_OVERRIDE_POLICY == MESSAGES_PUSH_OVERRIDE_SENT_ONLY )
    {
        messages_head_t messagesHeadOriginal = messagesHead;

        // start searching for a hole from buffer's head
        do
        {
            if ( 0 == messages[messagesHead].created_at ) break;    // found empty message: hole
            if ( 1 == messages[messagesHead].transmitted ) break;   // found message that was transmitted: "hole"
        }
        while ( ++messagesHead < MESSAGES_SIZE );

        // if reached buffer's end, rewind at start and search until original buffer's head
        if ( MESSAGES_SIZE == messagesHead )
        {
            messagesHead = 0;

            while ( messagesHead < messagesHeadOriginal )
            {
                if ( 0 == messages[messagesHead].created_at ) break;
                if ( 1 == messages[messagesHead].transmitted ) break;

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
}

/// \brief Main server loop. Calls communication_thread() on each new connection.
void listening_worker()
{
    int status, server_fd, socket_fd;
    struct sockaddr_in serverAddress, clientAddress;
    char ip[INET_ADDRSTRLEN];
    socklen_t clientAddressLength;
    pthread_t communicationThread;

    status = socket(AF_INET, SOCK_STREAM, 0);
    if ( status < 0 )
        error( status, "\tserver_listen(): socket() failed" );
    server_fd = status;

    // Set server address ( forcefully attaching socket to selected socket port )
//    status = 1;
//    status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &status, sizeof(status) );
//    if ( status < 0 )
//        error( status, "\tserver_listen(): setsockopt() failed" );

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
        log_info( "Started listening loop! Waiting in accept()...", "listening_worker()", "-" );

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

        char connectedMessage[22];
        snprintf( connectedMessage, 22, "Connected: AEM = %04d", device.AEM );
        log_info( connectedMessage, "listening_worker()", "-" );

        //  - open thread
        if ( communicationThreadsAvailable > 0 )
        {
            //----- CRITICAL SECTION
            pthread_mutex_lock( &availableThreadsLock );

            communicationThread = communicationThreads[ COMMUNICATION_WORKERS_MAX - communicationThreadsAvailable ];
            communicationThreadsAvailable--;

            pthread_mutex_unlock( &availableThreadsLock );
            //-----:end

            args.concurrent = 1;

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
            args.concurrent = 0;
            communication_worker(&args);
        }
    }
    while (1);
}
