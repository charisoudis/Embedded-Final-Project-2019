#include <cstddef>
#include "gtest/gtest.h"
extern "C" {
    #include "conf.h"
    #include "types.h"
    #include "server.h"
    #include "utils.h"
    #include "client.h"

    #include <sodium.h>
}

#define GOUT(STREAM) \
    do \
    { \
        std::stringstream ss; \
        ss << STREAM << std::endl; \
        testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[ INFO ] "); \
        testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, ss.str().c_str()); \
    } while (false); \

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


/* messagesHead is in range: [0, $MESSAGES_SIZE - 1] */
extern messages_head_t messagesHead;
extern messages_head_t inboxHead;
extern Message MESSAGES_BUFFER[ MESSAGES_SIZE ];
extern InboxMessage *INBOX;

// Active flag for each AEM
extern bool CLIENT_AEM_ACTIVE_LIST[CLIENT_AEM_LIST_LENGTH];


class ServerTest : public ::testing::Test {

protected:

    void SetUp() override
    {
        int status;
        for (bool & i : CLIENT_AEM_ACTIVE_LIST)
            i = false;

        messagesStats.received = 0;
        messagesStats.received_for_me = 0;
        messagesStats.transmitted = 0;
        messagesStats.transmitted_to_recipient = 0;
        messagesStats.produced = 0;
        messagesStats.producedDelayAvg = 0.0F;

        // Set max execution time ( in seconds )
        executionTimeRequested = MAX_EXECUTION_TIME;

        // Initialize RNG
        srand( (unsigned int) time(nullptr) );

        // Get AEM of running device
        CLIENT_AEM = 9026;

        // Initialize types
        messagesHead = 0;
        inboxHead = 0;

        // Initialize INBOX buffer
        INBOX = (InboxMessage *) malloc(INBOX_SIZE * sizeof( InboxMessage ) );
//    for ( uint16_t message_i = 0; message_i < MESSAGES_SIZE; message_i++ ) INBOX[message_i].created_at = 0;

        // Initialize logger
        messagesStats.produced = 0;
        messagesStats.received = 0;
        messagesStats.received_for_me = 0;
        messagesStats.transmitted = 0;
        messagesStats.transmitted_to_recipient = 0;
    }

public:

    void TearDown( ) override
    {
        // Restore active devices
        for (bool & i : CLIENT_AEM_ACTIVE_LIST)
            i = false;

        // Restore $messagesHead back to 0
        //  - "erase" all MESSAGES_BUFFER
        memset(&MESSAGES_BUFFER, 0, MESSAGES_SIZE * sizeof(Message) );
        //  - set $messagesHead
        messagesHead = 0;
        inboxHead = 0;
    }

};


//------------------------------------------------------------------------------------------------


/// \brief Tests utils > devices_exists() function.
TEST_F(ServerTest, DevicesExists)
{
    Device device1 = {.AEM = 9026};
    Device device2 = {.AEM = 8600};

    devices_push( device1 );

    EXPECT_EQ( 1, devices_exists( device1 ) );
    EXPECT_EQ( 0, devices_exists( device2 ) );

    devices_remove( device1 );
    EXPECT_EQ( 0, devices_exists( device1 ) );

    devices_push( device2 );
    EXPECT_EQ( 1, devices_exists( device2 ) );
}

/// \brief Tests utils > devices_push() function.
TEST_F(ServerTest, DevicesPush_Simple)
{
    Device device1 = {.AEM = 9026};
    Device device2 = {.AEM = 8600};

    device1.aemIndex = resolveAemIndex( device1 );
    device2.aemIndex = resolveAemIndex( device2 );

    devices_push( device1 );
    EXPECT_EQ( false, CLIENT_AEM_ACTIVE_LIST[device2.aemIndex] );
    EXPECT_EQ( true, CLIENT_AEM_ACTIVE_LIST[device1.aemIndex] );

    devices_push( device2 );
    EXPECT_EQ( true, CLIENT_AEM_ACTIVE_LIST[device1.aemIndex] );
    EXPECT_EQ( true, CLIENT_AEM_ACTIVE_LIST[device2.aemIndex] );
}

/// \brief Tests utils > devices_remove() function.
TEST_F(ServerTest, DevicesRemove_Simple)
{
    Device device1, device2, device3;
    device1 = {.AEM = 9026};
    device2 = {.AEM = 8600};
    device3 = {.AEM = 8723};

    devices_push( device1 );
    devices_push( device2 );
    devices_push( device3 );

    devices_remove( device1 );
    EXPECT_EQ( 0, devices_exists(device1) );
    EXPECT_EQ( 1, devices_exists(device2) );
    EXPECT_EQ( 1, devices_exists(device3) );

    devices_remove( device3 );
    EXPECT_EQ( 0, devices_exists(device1) );
    EXPECT_EQ( 1, devices_exists(device2) );
    EXPECT_EQ( 0, devices_exists(device3) );
}

/// \brief Tests utils > devices_remove() function.
TEST_F(ServerTest, DevicesRemove_Full)
{
    Device devices[CLIENT_AEM_LIST_LENGTH];

    // Push max devices
    for ( uint32_t device_i = 0; device_i < CLIENT_AEM_LIST_LENGTH; device_i++ )
    {
        devices[device_i] = {.AEM = CLIENT_AEM_LIST[device_i]};
        devices_push( devices[device_i] );
    }

    // Check active list
    for (auto & device : devices)
    {
        EXPECT_EQ( 1, devices_exists( device ) );
        EXPECT_EQ( true, CLIENT_AEM_ACTIVE_LIST[device.aemIndex] );
    }

    // Remove all
    for (auto & device : devices)
    {
        devices_remove( device );
    }

    // Check active list
    for (auto & device : devices)
    {
        EXPECT_EQ( 0, devices_exists( device ) );
        EXPECT_EQ( false, CLIENT_AEM_ACTIVE_LIST[device.aemIndex] );
    }
}

/// \brief Tests utils > messages_push() function.
TEST_F(ServerTest, MessagesPushSimple)
{
    Message message1;
    Message message2;

    generateRandomMessage( &message1 );
    generateRandomMessage( &message2 );

    messages_push( &message1 );
    EXPECT_EQ(messagesHead, 1);

    messages_push( &message2 );
    EXPECT_EQ(messagesHead, 2);
}

/// \brief Tests utils > messages_push() function.
TEST_F(ServerTest, MessagesPushFull)
{
    char log[100];

    uint32_t firstTransmittedMessageIndex = 0;
    uint8_t firstTransmittedMessageIndexSet = 0;

    uint32_t secondTransmittedMessageIndex = 0;
    uint8_t secondTransmittedMessageIndexSet = 0;

    uint32_t thirdTransmittedMessageIndex = 0;
    uint8_t thirdTransmittedMessageIndexSet = 0;

    // Add $MESSAGE_SIZE MESSAGES_BUFFER
    Message message;
    for ( uint16_t message_i = 0; message_i < MESSAGES_SIZE; message_i++ )
    {
        generateRandomMessage( &message );

        // Initialize RNG
//        srand( (unsigned int) time(nullptr) );
        uint32_t rr = randombytes_random();

//        sprintf( log, "rr = %u\n", rr );
//        GOUT( log );

        if ( randombytes_random() % 10000 < 489 )
        {
            message.transmitted = 1;
        }

        messages_push( &message );

        if ( 1 == message.transmitted )
        {
            if ( 0 == firstTransmittedMessageIndexSet )
            {
                firstTransmittedMessageIndexSet = 1;
                firstTransmittedMessageIndex = message_i;

                sprintf( log, "firstTransmittedMessageIndex = %d\n", firstTransmittedMessageIndex );
                GOUT( log );
            }
            else if ( 0 == secondTransmittedMessageIndexSet )
            {
                secondTransmittedMessageIndexSet = 1;
                secondTransmittedMessageIndex = message_i;

                sprintf( log, "secondTransmittedMessageIndex = %d\n", secondTransmittedMessageIndex );
                GOUT( log );
            }
            else if ( 0 == thirdTransmittedMessageIndexSet )
            {
                thirdTransmittedMessageIndexSet = 1;
                thirdTransmittedMessageIndex = message_i;

                sprintf( log, "thirdTransmittedMessageIndex = %d\n", thirdTransmittedMessageIndex );
                GOUT( log );
            }
        }
    }
    EXPECT_EQ( messagesHead, 0 );

    // Check next push
    generateRandomMessage( &message );
    messages_push( &message );
    uint32_t real_value = 0 != strcmp( "sent_only", MESSAGES_PUSH_OVERRIDE_POLICY ) ? 1 : firstTransmittedMessageIndex + 1;
    EXPECT_EQ( messagesHead, real_value );

    // Check next push
    generateRandomMessage( &message );
    messages_push( &message );
    real_value = 0 != strcmp( "sent_only", MESSAGES_PUSH_OVERRIDE_POLICY ) ? 2 : secondTransmittedMessageIndex + 1;
    EXPECT_EQ( messagesHead, real_value );

    // Check next push
    generateRandomMessage( &message );
    messages_push( &message );
    real_value = 0 != strcmp( "sent_only", MESSAGES_PUSH_OVERRIDE_POLICY ) ? 3 : thirdTransmittedMessageIndex + 1;
    EXPECT_EQ( messagesHead, real_value );
}






























