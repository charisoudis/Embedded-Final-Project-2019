#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "gtest/gtest.h"
extern "C" {
    #include "server.h"
}


//------------------------------------------------------------------------------------------------


ActiveDevicesQueue activeDevicesQueue;


//------------------------------------------------------------------------------------------------


/* messagesHead is in range: [0, $MESSAGES_SIZE - 1] */
messages_head_t messagesHead;

/* devicesHead is in range: [0, $COMMUNICATION_WORKERS_MAX + 2 - 1] */
devices_head_t devicesHead;

Message messages[ MESSAGES_SIZE ];


class ServerTest : public ::testing::Test {

protected:

    void SetUp() override
    {
        activeDevicesQueue.head = 0;
        activeDevicesQueue.tail = 0;
    }

public:

    void TearDown( ) override
    {
        // Restore devices
        for ( devices_head_t devices_i = 0; devices_i < ACTIVE_DEVICES_MAX - 1; devices_i++ )
        {
           activeDevicesQueue.devices[devices_i].AEM = 0;
        }
        activeDevicesQueue.head = 0;
        activeDevicesQueue.tail = 0;

        // Restore $messagesHead back to 0
        //  - "erase" all messages
        for ( uint16_t message_i = 0; message_i < messagesHead; message_i++ )
        {
            messages[message_i].sender = 0;
            messages[message_i].recipient = 0;
            messages[message_i].created_at = 0;
            messages[message_i].transmitted = 0;
            messages[message_i].transmitted_device.AEM = 0;
        }
        //  - set $messagesHead
        messagesHead = 0;
    }

};


//------------------------------------------------------------------------------------------------


/// \brief Tests utils > devices_exists() function.
TEST_F(ServerTest, DevicesExists)
{
    Device device1 = {.AEM = 9026};
    Device device2 = {.AEM = 9027};

    devices_push( device1 );

    EXPECT_EQ( 1, devices_exists( device1 ) );
    EXPECT_EQ( 0, devices_exists( device2 ) );

    devices_remove( device1 );
    EXPECT_EQ( 0, devices_exists( device1 ) );
}

/// \brief Tests utils > devices_push() function.
TEST_F(ServerTest, DevicesPush_Simple)
{
    devices_head_t head, tail;
    head = activeDevicesQueue.head;
    tail = activeDevicesQueue.tail;

    Device device1 = {.AEM = 9026};
    Device device2 = {.AEM = 9027};

    devices_push( device1 );
    EXPECT_EQ( head, activeDevicesQueue.head );
    EXPECT_EQ( tail + 1, activeDevicesQueue.tail );

    devices_push( device2 );
    EXPECT_EQ( head, activeDevicesQueue.head );
    EXPECT_EQ( tail + 2, activeDevicesQueue.tail );
}

/// \brief Tests utils > devices_push() function when queue fills up.
TEST_F(ServerTest, DevicesPush_OverflowCheck)
{
    devices_head_t devices_i;
    for ( devices_i = 0; devices_i < ACTIVE_DEVICES_MAX - 1; devices_i++ )
    {
       Device device = {.AEM = (uint32_t) (9026 + devices_i)};
       devices_push( device );
    }
    EXPECT_EQ( ACTIVE_DEVICES_MAX - 1, activeDevicesQueue.tail );

    Device device = {.AEM = (uint32_t) (9026 + devices_i)};
    devices_push( device );
    EXPECT_EQ( (devices_head_t) -1, activeDevicesQueue.tail );
}

/// \brief Tests utils > devices_push() function when queue fills up.
TEST_F(ServerTest, DevicesPush_OverflowCheck2)
{
    devices_head_t devices_i;
    for ( devices_i = 0; devices_i < ACTIVE_DEVICES_MAX - 1; devices_i++ )
    {
       Device device = {.AEM = (uint32_t) (9026 + devices_i)};
       devices_push( device );
    }
    EXPECT_EQ( ACTIVE_DEVICES_MAX - 1, activeDevicesQueue.tail );

    activeDevicesQueue.head++;

    Device device = {.AEM = (uint32_t) (9026 + devices_i)};
    devices_push( device );
    EXPECT_NE( (devices_head_t) -1, activeDevicesQueue.tail );

    Device device2 = {.AEM = (uint32_t) (9026 + devices_i + 1)};
    devices_push( device2 );
    EXPECT_EQ( (devices_head_t) -1, activeDevicesQueue.tail );
}

/// \brief Tests utils > devices_remove() function.
TEST_F(ServerTest, DevicesRemove_Simple)
{
    Device device1, device2, device3;
    device1 = {.AEM = 9026};
    device2 = {.AEM = 9027};
    device3 = {.AEM = 9028};

    devices_push( device1 );
    devices_push( device2 );
    devices_push( device3 );

    EXPECT_EQ( 0, activeDevicesQueue.head );
    EXPECT_EQ( 3, activeDevicesQueue.tail );

    devices_remove( device1 );
    EXPECT_EQ( 1, activeDevicesQueue.head );
    EXPECT_EQ( 3, activeDevicesQueue.tail );

    devices_remove( device3 );
    EXPECT_EQ( 2, activeDevicesQueue.head );
    EXPECT_EQ( 3, activeDevicesQueue.tail );
    EXPECT_EQ( 9027, activeDevicesQueue.devices[activeDevicesQueue.head].AEM );
}

/// \brief Tests utils > devices_remove() function.
TEST_F(ServerTest, DevicesRemove_Full)
{
    Device devices[ACTIVE_DEVICES_MAX];

    // Push max devices
    for ( devices_head_t device_i = 0; device_i < ACTIVE_DEVICES_MAX; device_i++ )
    {
        devices[device_i] = {.AEM = (uint32_t) ( 9026 + device_i )};
        devices_push( devices[device_i] );
    }

    // Select random device index
    srand(static_cast<unsigned int>(time(NULL )));
    auto randomDeviceIndex = ( devices_head_t ) ( rand() % ACTIVE_DEVICES_MAX );

    // Remove selected device
    devices_remove( devices[randomDeviceIndex] );

    // Check for removed device
    EXPECT_EQ( 0, devices_exists( devices[randomDeviceIndex] ) );

    // Check for other devices
    for ( devices_head_t device_i = 0; device_i < randomDeviceIndex; device_i++ )
        EXPECT_EQ( 1, devices_exists( devices[device_i] ) );

    randomDeviceIndex++;

    for ( devices_head_t device_i = randomDeviceIndex; device_i < ACTIVE_DEVICES_MAX; device_i++ )
        EXPECT_EQ( 1, devices_exists( devices[device_i] ) );
}

/// \brief Tests utils > messages_push() function.
TEST_F(ServerTest, MessagesPushSimple)
{
    Message message1 = generateRandomMessage();
    Message message2 = generateRandomMessage();

    messages_push( message1 );
    EXPECT_EQ(messagesHead, 1);

    messages_push( message2 );
    EXPECT_EQ(messagesHead, 2);
}

/// \brief Tests utils > messages_push() function.
TEST_F(ServerTest, MessagesPushFull)
{
    uint32_t firstTransmittedMessageIndex = 0;
    uint8_t firstTransmittedMessageIndexSet = 0;

    uint32_t secondTransmittedMessageIndex = 0;
    uint8_t secondTransmittedMessageIndexSet = 0;

    uint32_t thirdTransmittedMessageIndex = 0;
    uint8_t thirdTransmittedMessageIndexSet = 0;

    // Add $MESSAGE_SIZE messages
    for ( uint16_t message_i = 0; message_i < MESSAGES_SIZE; message_i++ )
    {
        Message message = generateRandomMessage();
        messages_push( message );

        if ( 1 == message.transmitted )
        {
            if ( 0 == firstTransmittedMessageIndexSet )
            {

                firstTransmittedMessageIndexSet = 1;
                firstTransmittedMessageIndex = message_i;
            }
            else if ( 0 == secondTransmittedMessageIndexSet )
            {
                secondTransmittedMessageIndexSet = 1;
                secondTransmittedMessageIndex = message_i;
            }
            else if ( 0 == thirdTransmittedMessageIndexSet )
            {
                thirdTransmittedMessageIndexSet = 1;
                thirdTransmittedMessageIndex = message_i;
            }
        }
    }
    EXPECT_EQ( messagesHead, 0 );

    // Check next push
    Message message = generateRandomMessage();
    messages_push( message );
    EXPECT_EQ( messagesHead, MESSAGES_PUSH_OVERRIDE_POLICY ? 1 : firstTransmittedMessageIndex + 1 );

    // Check next push
    message = generateRandomMessage();
    messages_push( message );
    EXPECT_EQ( messagesHead, MESSAGES_PUSH_OVERRIDE_POLICY ? 2 : secondTransmittedMessageIndex + 1 );

    // Check next push
    message = generateRandomMessage();
    messages_push( message );
    EXPECT_EQ( messagesHead, MESSAGES_PUSH_OVERRIDE_POLICY ? 3 : thirdTransmittedMessageIndex + 1 );
}






























