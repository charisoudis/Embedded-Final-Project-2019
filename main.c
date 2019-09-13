#include <signal.h>
#include <utils.h>
#include "conf.h"
#include "client.h"
#include "log.h"
#include "server.h"

//------------------------------------------------------------------------------------------------

#define MAX_EXECUTION_TIME 100                 // 2 hours
static uint32_t executionTimeRequested;       // secs
static struct timespec executionTimeActualStart, executionTimeActualFinish;

pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
uint8_t communicationThreadsAvailable = COMMUNICATION_WORKERS_MAX;

static pthread_t pollingThread, producerThread;
pthread_mutex_t messagesBufferLock, activeDevicesLock, availableThreadsLock, messagesStatsLock, logLock;

//DevicesQueue activeDevicesQueue;
MessagesStats messagesStats;

uint32_t CLIENT_AEM;

//------------------------------------------------------------------------------------------------

extern messages_head_t messagesHead;

/// \brief Handler of SIGALRM signal. Used to terminate sampling when totalSamplingTime finishes.
/// \param signo
/// \return void - Actually this function terminates program execution.
static void onAlarm(int signo);

int main( int argc, char **argv )
{
    int status;

    // Get AEM of running device
    CLIENT_AEM = getClientAem("wlan0");
    printf( "AEM = %d\n", CLIENT_AEM );

    // Initialize types
    messagesHead = 0;

    // Initialize logger
    log_tearUp( "log.txt" );
    messagesStats.produced = 0;
    messagesStats.received = 0;
    messagesStats.transmitted = 0;

    // Set max execution time ( in seconds )
    executionTimeRequested = ( argc < 2 ) ? MAX_EXECUTION_TIME : (uint32_t) strtol( argv[1], (char **)NULL, 10 );

    // Initialize RNG
    srand((unsigned int) time(NULL ));

    // Initialize Locks
    status = pthread_mutex_init( &messagesBufferLock, NULL );
    if ( status != 0 )
        error( status, "\tmain(): pthread_mutex_init( messagesBufferLock ) failed" );
    status = pthread_mutex_init( &activeDevicesLock, NULL );
    if ( status != 0 )
        error( status, "\tmain(): pthread_mutex_init( activeDevicesLock ) failed" );
    status = pthread_mutex_init( &availableThreadsLock, NULL );
    if ( status != 0 )
        error( status, "\tmain(): pthread_mutex_init( activeDevicesLock ) failed" );
    status = pthread_mutex_init( &messagesStatsLock, NULL );
    if ( status != 0 )
        error( status, "\tmain(): pthread_mutex_init( messagesStatsLock ) failed" );
    status = pthread_mutex_init( &logLock, NULL );
    if ( status != 0 )
        error( status, "\tmain(): pthread_mutex_init( logLock ) failed" );

    // Start recording actual time
    clock_gettime(CLOCK_REALTIME, &executionTimeActualStart);

    // Setup alarm
    alarm( executionTimeRequested );
    signal( SIGALRM, onAlarm );

    // Start polling client ( in a new thread )
    status = pthread_create(&pollingThread, NULL, (void *) polling_worker, NULL);
    if ( status != 0 )
        error( status, "\tmain(): pthread_create( pollingThread ) failed" );

    // Start producer client ( in a new thread )
    status = pthread_create(&producerThread, NULL, (void *) producer_worker, NULL);
    if ( status != 0 )
        error( status, "\tmain(): pthread_create( producerThread ) failed" );

    // Start listening server ( main thread )
    listening_worker();

    return EXIT_SUCCESS;
}

static void onAlarm( int signo )
{
    log_info( "Caught the SIGALRM signal", "onAlarm", "-" );

    int status;

    // Kill Producer Thread
    status = pthread_cancel( producerThread );
    if ( status != 0 )
        error( status, "\tonAlarm(): pthread_cancel() on producerThread failed" );

    status = pthread_join( producerThread, NULL );
    if ( status != 0 )
        error( status, "\tonAlarm(): pthread_join() on producerThread failed" );

    // Kill Polling Thread
    status = pthread_cancel( pollingThread );
    if ( status != 0 )
        error( status, "\tonAlarm(): pthread_cancel() on pollingThread failed" );

    status = pthread_join( pollingThread, NULL );
    if ( status != 0 )
        error( status, "\tonAlarm(): pthread_join() on pollingThread failed" );

    // Find actual execution time
    clock_gettime(CLOCK_REALTIME, &executionTimeActualFinish);
    long executionTimeActualSeconds = executionTimeActualFinish.tv_sec - executionTimeActualStart.tv_sec;
    long executionTimeActualNanoSeconds = executionTimeActualFinish.tv_nsec - executionTimeActualStart.tv_nsec;

    if (executionTimeActualStart.tv_nsec > executionTimeActualFinish.tv_nsec)   // clock underflow
    {
        --executionTimeActualSeconds;
        executionTimeActualNanoSeconds += 1000000000;
    }

    double executionTimeActual = (double)executionTimeActualSeconds + (double)executionTimeActualNanoSeconds/(double)1000000000;

    log_info( "Exiting now...", "onAlarm", "-" );

    // Close logger
    messagesStats.producedDelayAvg /= ( float ) messagesStats.produced; // avg
    messagesStats.producedDelayAvg /= 60.0;                             // sec --> min
    log_tearDown( executionTimeRequested, executionTimeActual, &messagesStats );

    exit( EXIT_SUCCESS );
}