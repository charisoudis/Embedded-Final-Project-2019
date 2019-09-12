#include <conf.h>
#include <client.h>
#include <log.h>
#include <server.h>
#include <utils.h>

//------------------------------------------------------------------------------------------------

extern pthread_mutex_t messagesBufferLock, availableThreadsLock, logLock;
extern MessagesStats messagesStats;
extern Message messages[MESSAGES_SIZE];

extern pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
extern uint8_t communicationThreadsAvailable;

//------------------------------------------------------------------------------------------------


/// \brief Fetches AEM of running device from $interface network interface
/// \param interface usually this is "wlan0"
/// \return uint32_t ( 4-digits )
uint32_t getClientAem(const char *interface)
{
    int fd;
    struct ifreq ifr;
    char ip[15];

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    // Get IPv4 address as string
    sprintf( ip, "%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr) );

    uint32_t aem = ip2aem( ip );
    return 0 != aem || 0 == strcmp( interface, "wlp6s0" ) ? aem : getClientAem( "wlp6s0" );
}

/// \brief Polling thread. Starts polling to find active servers. Creates a new thread for each server found.
void *polling_worker(void)
{
    int status, listIndex = 0;
    uint16_t aem;
    char ip[12];
    char logMessage[255];

    // Use current time as seed for random generator
    srand( (unsigned int) time(NULL) );

    //----- CRITICAL SECTION
    pthread_mutex_lock( &logLock );
    log_info( "Started polling loop! Checking in socket_connect()...", "polling_worker()", "-" );
    pthread_mutex_unlock( &logLock );
    //-----:end

    aem = ( CLIENT_AEM_SOURCE_RANGE == CLIENT_AEM_SOURCE ) ? CLIENT_AEM_RANGE_MIN : CLIENT_AEM_LIST[listIndex];
    do
    {
        // Format IP address
        snprintf( ip, 12, "10.0.%02d.%02d", (int) aem / 100, aem % 100 );
        fprintf( stdout, "ip = %s\n", ip );

        // Try connecting
        int socket_fd = socket_connect( ip );
        if ( -1 != socket_fd )
        {
            //----- NON-CANCELABLE SECTION
            status = pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, NULL );
            if ( status != 0 )
                error( status, "\tpolling_worker(): pthread_setcancelstate( DISABLE ) failed" );

            // Connected > OffLoad to communication worker
            //  - format arguments
            Device device = {.AEM = aem};
            CommunicationWorkerArgs args = {
                    .connected_socket_fd = (uint16_t) socket_fd,
                    .server = 0
            };
            memcpy( &args.connected_device, &device, sizeof( Device ) );

            // Log
            //----- CRITICAL SECTION
            pthread_mutex_lock( &logLock );
            sprintf( logMessage, "Connected: AEM = %04d", device.AEM );
            log_info( logMessage, "polling_worker()", "socket.h > socket_connect()" );
            pthread_mutex_unlock( &logLock );
            //-----:end

            //  - open thread
            if ( communicationThreadsAvailable > 0 )
            {
                //----- CRITICAL SECTION
                pthread_mutex_lock( &availableThreadsLock );

                pthread_t communicationThread = communicationThreads[ COMMUNICATION_WORKERS_MAX - communicationThreadsAvailable ];
                communicationThreadsAvailable--;

                pthread_mutex_unlock( &availableThreadsLock );
                //-----:end

                args.concurrent = 1;

                status = pthread_create( &communicationThread, NULL, (void *) communication_worker, &args );
                if ( status != 0 )
                    error( status, "\tpolling_worker(): pthread_create() failed" );

                status = pthread_detach( communicationThread );
                if ( status != 0 )
                    error( status, "\tpolling_worker(): pthread_detach() failed" );
            }
            else
            {
                // run in current thread
                args.concurrent = 0;
                communication_worker(&args);
            }

            // Log
            //----- CRITICAL SECTION
            pthread_mutex_lock( &logLock );
            sprintf( logMessage, "Finished: AEM = %04d", device.AEM );
            log_info( logMessage, "polling_worker()", "socket.h > socket_connect()" );
            pthread_mutex_unlock( &logLock );
            //-----:end

            status = pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
            if ( status != 0 )
                error( status, "\tpolling_worker(): pthread_setcancelstate( ENABLE ) failed" );
            //-----:end
        }
        else
        {
            sleep( 1 );
        }

        // Reset polling if reached AEMs range's maximum.
        if ( CLIENT_AEM_SOURCE_RANGE == CLIENT_AEM_SOURCE )
        {
            if ( ++aem > CLIENT_AEM_RANGE_MAX )
            {
                aem = CLIENT_AEM_RANGE_MIN;

                //----- CRITICAL SECTION
                pthread_mutex_lock( &logLock );
                log_info( "CLIENT_AEM_RANGE_MAX reached. Starting from CLIENT_AEM_RANGE_MIN...", "polling_worker()", "-" );
                pthread_mutex_unlock( &logLock );
                //-----:end
            }
        }
        else
        {
            if ( ++listIndex == CLIENT_AEM_LIST_LENGTH )
            {
                listIndex = 0;
            }
            aem = CLIENT_AEM_LIST[listIndex];
        }
    }
    while( 1 );
}

/// \brief Message producer thread. Produces a random message at the end of the pre-defined interval.
void *producer_worker(void)
{
    Message message;
    uint32_t delay;
    int status;

    do
    {
        // Generate
        generateRandomMessage( &message );
//        inspect( message, 1, stdout );

        //----- NON-CANCELABLE SECTION
        status = pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, NULL );
        if ( status != 0 )
            error( status, "\tproducer_worker(): pthread_setcancelstate( DISABLE ) failed" );

        // Store
        //----- CRITICAL SECTION
        pthread_mutex_lock( &messagesBufferLock );

        messages_push( message );

        pthread_mutex_unlock( &messagesBufferLock );
        //-----:end

        messagesStats.produced++;

        status = pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
        if ( status != 0 )
            error( status, "\tproducer_worker(): pthread_setcancelstate( ENABLE ) failed" );
        //-----:end

        // Sleep
        delay = (uint32_t) (rand() % (PRODUCER_DELAY_RANGE_MAX + 1 - PRODUCER_DELAY_RANGE_MIN ) + PRODUCER_DELAY_RANGE_MIN);
//        delay = (uint32_t) (60 * delay);
        messagesStats.producedDelayAvg += delay;
        sleep( delay );
    }
    while( 1 );
}