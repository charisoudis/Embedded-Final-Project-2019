#include "client.h"

extern pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
extern uint8_t communicationThreadsAvailable;

extern pthread_mutex_t availableThreadsLock;
extern pthread_mutex_t messagesBufferLock;

/// \brief Polling thread. Starts polling to find active servers. Creates a new thread for each server found.
void *polling_worker(void)
{
    sleep( 100 );

    int status;
    uint16_t aem;
    char ip[12];
    pthread_t communicationThread;

    aem = CLIENT_AEM_RANGE_MIN;
    do
    {
        // Format IP address
        snprintf( ip, 12, "10.0.%02d.%02d", (int) aem / 100, aem % 100 );

        // Try connecting
        int socket_fd = socket_connect( ip );
        if ( -1 != socket_fd )
        {
            // Connected > OffLoad to communication worker
            //  - format arguments
            Device device = {.AEM = aem};
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
                    error( status, "\tpolling_worker(): pthread_create() failed" );

                status = pthread_detach( communicationThread );
                if ( status != 0 )
                    error( status, "\tpolling_worker(): pthread_detach() failed" );
            }
            else
            {
                close( socket_fd );
            }
        }

        // Reset polling if reached AEMs range's maximum.
        if ( ++aem > CLIENT_AEM_RANGE_MAX )
        {
            aem = CLIENT_AEM_RANGE_MIN;
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
        /* Disable cancellation for a while, so that we don't
              immediately react to a cancellation request */
        status = pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, NULL );
        if ( status != 0 )
            error( status, "\tproducer_worker(): pthread_setcancelstate(DISABLE) failed" );

        // Generate
        message = generateRandomMessage();

        // Store
        //----- CRITICAL
        pthread_mutex_lock( &messagesBufferLock );

        messages_push( message );

        pthread_mutex_unlock( &messagesBufferLock );
        //-----:end

        status = pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
        if ( status != 0 )
            error( status, "\tproducer_worker(): pthread_setcancelstate(ENABLE) failed" );

        /* sleep() is a cancellation point */

        // Sleep
        delay = randombytes_uniform( PRODUCER_DELAY_RANGE_MAX + 1 - PRODUCER_DELAY_RANGE_MIN ) + PRODUCER_DELAY_RANGE_MIN;
        sleep( delay );
    }
    while( 1 );
}