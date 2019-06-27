#include <locale.h>
#include <zconf.h>
#include <signal.h>
#include "client.h"
#include "server.h"
#include "utils.h"


//------------------------------------------------------------------------------------------------


#define MAX_EXECUTION_TIME 10 //7200     // 2 hours

static pthread_t pollingThread, producerThread;
pthread_mutex_t availableThreadsLock, messagesBufferLock;

pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
uint8_t communicationThreadsAvailable = COMMUNICATION_WORKERS_MAX;


//------------------------------------------------------------------------------------------------


/// \brief Handler of SIGALRM signal. Used to terminate sampling when totalSamplingTime finishes.
/// \param signo
/// \return void - Actually this function terminates program execution.
static void onAlarm(int signo);


int main( int argc, char **argv )
{
    static uint32_t maxExecutionTime;   // secs
    int status;

    // Set max execution time ( in seconds )
    maxExecutionTime = ( argc < 2 ) ? MAX_EXECUTION_TIME : (uint32_t) strtol( argv[1], (char **)NULL, 10 );

    // Initialize Locks
    status = pthread_mutex_init( &availableThreadsLock, NULL );
    if ( status != 0 )
        error( status, "\tmain(): pthread_mutex_init( availableThreadsLock ) failed" );
    status = pthread_mutex_init( &messagesBufferLock, NULL );
    if ( status != 0 )
        error( status, "\tmain(): pthread_mutex_init( messagesBufferLock ) failed" );

    // Setup alarm
    alarm( maxExecutionTime );
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

//    Message message, messageDeserialized;
//    MessageSerialized messageSerialized;
//
//    // Format datetime stings in Greek
//    setlocale( LC_TIME, "el_GR.UTF-8" );
//
//    // Generate a new message ( with random contents )
//    message = generateRandomMessage();
//    inspect( message, true );
//
//    // Test Implode
//    messageSerialized = (char *)malloc( 277 );
//    implode( "_", message, messageSerialized );
//    printf( "messageSerialized = \"%s\"\n\n", messageSerialized );
//
//    // Test Explode
//    messageDeserialized = explode( "_", messageSerialized );
//    inspect( messageDeserialized, false );
//
//    // Free resources
//    free( messageSerialized );

    return EXIT_SUCCESS;
}

static void onAlarm( int signo )
{
    fprintf( stdout, "\tonAlarm(): Caught the SIGALRM signal ( signo = %d )\n", signo );

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

    // TODO extra work here
    // ...

    printf("\tonAlarm(): Exiting now...\n");
    exit( EXIT_SUCCESS );
}

































