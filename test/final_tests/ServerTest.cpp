#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "gtest/gtest.h"
extern "C" {
    #include "server.h"
}


class ServerTest : public ::testing::Test {

protected:

    void SetUp() override
    {

    }

public:

    void TearDown( ) override
    {
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






























