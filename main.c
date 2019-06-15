#include <locale.h>
#include "h/utils.h"
#include "h/client.h"

int main()
{
    Message message, messageDeserialized, sdfsfsd;
    MessageSerialized messageSerialized;

    // Format datetime stings in Greek
    setlocale( LC_TIME, "el_GR.UTF-8" );

    // Generate a new message ( with random contents )
    message = generateRandomMessage();
    inspect( message, true );

    // Test Implode
    messageSerialized = (char *)malloc( 277 );
    implode( "_", message, messageSerialized );
    printf( "messageSerialized = \"%s\"\n\n", messageSerialized );

    // Test Explode
    messageDeserialized = explode( "_", messageSerialized );
    inspect( messageDeserialized, false );

    // Free resources
    free( messageSerialized );

    return EXIT_SUCCESS;
}