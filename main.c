#include "conf.h"
#include "client.h"
#include "log.h"
#include "server.h"
#include "utils.h"
#include "communication.h"
#include <signal.h>

//------------------------------------------------------------------------------------------------

uint32_t executionTimeRequested;       // secs
static struct timespec executionTimeActualStart, executionTimeActualFinish;

pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
uint8_t communicationThreadsAvailable = COMMUNICATION_WORKERS_MAX;

static pthread_t pollingThread, producerThread, datetimeListenerThread;
pthread_mutex_t messagesBufferLock, activeDevicesLock, availableThreadsLock, messagesStatsLock, logLock, logEventLock;

//DevicesQueue activeDevicesQueue;
MessagesStats messagesStats;

uint32_t CLIENT_AEM;
uint32_t setupDatetimeAem;

// Communication time for each device
struct timeval CLIENT_AEM_CONN_START_LIST[CLIENT_AEM_LIST_LENGTH][MAX_CONNECTIONS_WITH_SAME_CLIENT] = {0, 0};
struct timeval CLIENT_AEM_CONN_END_LIST[CLIENT_AEM_LIST_LENGTH][MAX_CONNECTIONS_WITH_SAME_CLIENT] = {0, 0};
uint8_t CLIENT_AEM_CONN_N_LIST[CLIENT_AEM_LIST_LENGTH] = {0};

//------------------------------------------------------------------------------------------------

extern messages_head_t messagesHead;
extern messages_head_t messagesForMeHead;
extern Message *messagesForMe;

/// \brief Handler of SIGALRM signal. Used to terminate execution when MAX_EXECUTION_TIME finishes.
/// \param signo
/// \return void - Actually this function terminates program execution.
static void onAlarm(int signo);

/// \brief Handler of SIGALRM signal. Used to terminate setup process if exceeds timeout.
/// \param signo
/// \return void - This function terminates program execution.
static void onSetupAlarm(int signo);

/// \brief
/// \example ./Final [MAX_EXECUTION_TIME] [SETUP_DATE_TIME_AEM]
/// \param argc
/// \param argv
/// \return
int main( int argc, char **argv )
{
    int status;

//    Message message;
//    MessagesStats messagesStats1 = {0, 0,0, 0.0F};
//
//    log_tearUp( "log.txt", "session1.json" );
//    log_event_start( "connnection", 9026, 8600 );
//
//    generateRandomMessage( &message );
//    log_event_message( "received", &message );
//    generateRandomMessage( &message );
//    log_event_message( "received", &message );
//
//    generateRandomMessage( &message );
//    log_event_message( "transmitted", &message );
//    generateRandomMessage( &message );
//    log_event_message( "transmitted", &message );
//
//    log_event_stop();
//    log_tearDown( 0.0, &messagesStats1 );
//
//    return 1;


    // Set max execution time ( in seconds )
    executionTimeRequested = ( argc < 2 ) ? MAX_EXECUTION_TIME :
            (uint32_t) strtol( argv[1], (char **)NULL, STRSEP_BASE_10 );

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
//    status = pthread_mutex_init( &logLock, NULL );
//    if ( status != 0 )
//        error( status, "\tmain(): pthread_mutex_init( logLock ) failed" );
    status = pthread_mutex_init( &logEventLock, NULL );
    if ( status != 0 )
        error( status, "\tmain(): pthread_mutex_init( logEventLock ) failed" );

    // Get AEM of running device
    CLIENT_AEM = getClientAem("wlan0");
    printf( "AEM = %d\n", CLIENT_AEM );

    // Initialize types
    messagesHead = 0;
    messagesForMeHead = 0;

    // Initialize messagesForMe buffer
    messagesForMe = (Message *) malloc( MESSAGES_SIZE * sizeof( Message ) );
//    for ( uint16_t message_i = 0; message_i < MESSAGES_SIZE; message_i++ ) messagesForMe[message_i].created_at = 0;

    // Initialize logger
    log_tearUp( "log.txt", "session1.json" );
    messagesStats.produced = 0;
    messagesStats.received = 0;
    messagesStats.received_for_me = 0;
    messagesStats.transmitted = 0;
    messagesStats.transmitted_to_recipient = 0;

    // Setup datetime
    setupDatetimeAem = ( argc < 3 ) ? SETUP_DATETIME_AEM : (uint32_t) strtol( argv[1], (char **)NULL, STRSEP_BASE_10 );
    if ( setupDatetimeAem > 0 )
    {
        if ( CLIENT_AEM == setupDatetimeAem )
        {
            // Start datetime transmitter server ( in a new thread )
            status = pthread_create( &datetimeListenerThread, NULL, (void *) communication_datetime_listener_worker, NULL );
            if ( status != 0 )
                error( status, "\tmain(): pthread_create( datetimeListenerThread ) failed" );
        }
        else
        {
            // Setup alarm for setup
            alarm( SETUP_DATETIME_TIMEOUT );
            signal( SIGALRM, onSetupAlarm );

            // Receive & set datetime from datetime server
            if ( false == communication_datetime_receiver() )
                error( -1, "\tmain(): communication_datetime_receiver() failed" );
        }
    }

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
    char logMessage[LOG_MESSAGE_MAX_LEN];
    sprintf( logMessage, "Caught the SIGALRM signal ( signo = %d )", signo );
    log_info( logMessage, "onAlarm", "-" );

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

    // Kill Datetime Listener Thread
    if ( CLIENT_AEM == setupDatetimeAem )
    {
        status = pthread_cancel( datetimeListenerThread );
        if ( status != 0 )
            error( status, "\tonAlarm(): pthread_cancel() on datetimeListenerThread failed" );

        status = pthread_join( datetimeListenerThread, NULL );
        if ( status != 0 )
            error( status, "\tonAlarm(): pthread_join() on datetimeListenerThread failed" );
    }

    // Find actual execution time
    clock_gettime(CLOCK_REALTIME, &executionTimeActualFinish);
    long executionTimeActualSeconds = executionTimeActualFinish.tv_sec - executionTimeActualStart.tv_sec;
    long executionTimeActualNanoSeconds = executionTimeActualFinish.tv_nsec - executionTimeActualStart.tv_nsec;

    if (executionTimeActualStart.tv_nsec > executionTimeActualFinish.tv_nsec)   // clock underflow
    {
        --executionTimeActualSeconds;
        executionTimeActualNanoSeconds += 1e9;
    }

    double executionTimeActual = (double)executionTimeActualSeconds + (double)executionTimeActualNanoSeconds/(double)1e9;

    log_info( "Exiting now...", "onAlarm", "-" );

    // Close logger
    messagesStats.producedDelayAvg /= ( float ) messagesStats.produced; // avg
    messagesStats.producedDelayAvg /= 60.0;                             // sec --> min
    log_tearDown(executionTimeActual, &messagesStats);

    exit( EXIT_SUCCESS );
}

static void onSetupAlarm( int signo )
{
    fprintf( stderr, "onSetupAlarm(): Setup timeout (%d sec) reached! Exiting...\n", SETUP_DATETIME_TIMEOUT );
    exit( EXIT_FAILURE );
}