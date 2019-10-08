#include "conf.h"
#include "client.h"
#include "log.h"
#include "server.h"
#include "utils.h"
#include "communication.h"

//------------------------------------------------------------------------------------------------

extern pthread_mutex_t messagesBufferLock, availableThreadsLock, logEventLock;
extern MessagesStats messagesStats;
extern Message MESSAGES_BUFFER[MESSAGES_SIZE];

extern pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
extern uint8_t communicationThreadsAvailable;

extern uint32_t CLIENT_AEM;

//------------------------------------------------------------------------------------------------

/// \brief Fetches AEM of running device from $interface network interface
/// \param interface usually this is "wlan0"
/// \return uint32_t ( 4-digits )
uint32_t getClientAem(const char *interface)
{
    int fd;
    struct ifreq ifr;
    char ip[INET_ADDRSTRLEN];

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    // Get IPv4 address as string
    sprintf( ip, "%s\n", inet_ntoa( ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr ) );

    uint32_t aem = ip2aem( ip );
    return 0 != aem || 0 == strcmp( interface, "wlp6s0" ) ? aem : getClientAem( "wlp6s0" );
}

/// \brief Polling thread. Starts polling to find active servers. Creates a new thread for each server found.
void *polling_worker(void)
{
    int status;
    int polling_socket_fd;
    uint16_t listIndex;
    uint32_t aem;

    // Use current time as seed for random generator
    srand( (unsigned int) time(NULL) );

    listIndex = 0;
    aem = ( 0 == strcmp( "range", CLIENT_AEM_SOURCE ) ) ? CLIENT_AEM_RANGE_MIN : CLIENT_AEM_LIST[listIndex];
    do
    {
        // Try connecting
        polling_socket_fd = socket_connect( aem, SOCKET_PORT );
        if ( -1 != polling_socket_fd )
        {
            //----- NON-CANCELABLE SECTION
            status = pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, NULL );
            if ( status != 0 )
                error( status, "\tpolling_worker(): pthread_setcancelstate( DISABLE ) failed" );

            // Connected > OffLoad to communication worker
            //  - format arguments
            Device device = {
                    .AEM = aem,
                    .aemIndex = binary_search_index( CLIENT_AEM_LIST, CLIENT_AEM_LIST_LENGTH, aem )
            };
            CommunicationWorkerArgs args = {
                    .connected_socket_fd = (uint16_t) polling_socket_fd,
                    .server = false
            };
            memcpy( &args.connected_device, &device, sizeof( Device ) );

            //  - open thread
            if ( communicationThreadsAvailable > 0 )
            {
                pthread_mutex_lock( &availableThreadsLock );
                    pthread_t communicationThread = communicationThreads[ COMMUNICATION_WORKERS_MAX - communicationThreadsAvailable ];
                    communicationThreadsAvailable--;
                pthread_mutex_unlock( &availableThreadsLock );

                args.concurrent = true;

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
                args.concurrent = false;
                communication_worker( &args );
            }

            status = pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
            if ( status != 0 )
                error( status, "\tpolling_worker(): pthread_setcancelstate( ENABLE ) failed" );
            //-----:end
        }

        // Reset polling if reached AEMs range's maximum.
        if ( 0 == strcmp( "range", CLIENT_AEM_SOURCE ) )
        {
            if ( ++aem > CLIENT_AEM_RANGE_MAX )
            {
                aem = CLIENT_AEM_RANGE_MIN;
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
        pthread_mutex_lock( &logEventLock );
            log_event_start( "production", 0, 0 );

            // Generate
            generateRandomMessage( &message );
            inspect( message, true, stdout );

            //----- NON-CANCELABLE SECTION
            status = pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, NULL );
            if ( status != 0 )
                error( status, "\tproducer_worker(): pthread_setcancelstate( DISABLE ) failed" );

            // Store
            pthread_mutex_lock( &messagesBufferLock );
                messages_push( &message );
            pthread_mutex_unlock( &messagesBufferLock );

            // Log to session.json
            log_event_message( "produced", &message );
            log_event_stop();
        pthread_mutex_unlock( &logEventLock );

        messagesStats.produced++;

        status = pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
        if ( status != 0 )
            error( status, "\tproducer_worker(): pthread_setcancelstate( ENABLE ) failed" );
        //-----:end

        // Sleep
//        delay = (uint32_t) (rand() % (PRODUCER_DELAY_RANGE_MAX + 1 - PRODUCER_DELAY_RANGE_MIN ) + PRODUCER_DELAY_RANGE_MIN);
        delay = (uint32_t) (rand() % 5 + 1);
        messagesStats.producedDelayAvg += delay;
        sleep( delay );
    }
    while( 1 );
}
