#include <stdio.h>
#include <time.h>
#include "utils.h"
#include "client.h"

int main()
{
    Message message, messageDeserialized;
    MessageSerialized messageSerialized;

    // Generate a new message ( with random contents )
    message = generateRandomMessage();
    inspect( message, true );

    // Test Implode
    messageSerialized = (char *)malloc( 277 );
    implode( "_", message, messageSerialized );
    printf( "messageSerialized = \"%s\"\n\n", messageSerialized );

    // Test Explode
    Message messageRetrieved = explode( "_", messageSerialized );
    inspect( messageRetrieved, false );

    // Free resources
    free( messageSerialized );

    return EXIT_SUCCESS;
}