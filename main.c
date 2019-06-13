#include <stdio.h>
#include <time.h>
#include "utils.h"
#include "client.h"

int main() {

    Message message, messageDeserialized;
    MessageSerialized messageSerialized;

    // Generate a new message ( with random contents )
    message = generateRandomMessage();
    inspect( message );

    // Test Implode
    messageSerialized = implode( '_', message );
    printf( "messageSerialized = \"%s\"\n\n", messageSerialized );

    // Test Explode
    Message messageRetrieved = explode( '_', messageSerialized );
}