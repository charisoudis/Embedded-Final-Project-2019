#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "gtest/gtest.h"
extern "C" {
    #include "utils.h"
}


//------------------------------------------------------------------------------------------------


#define MAX_EXECUTION_TIME 10 //7200     // 2 hours

static pthread_t pollingThread, producerThread;
pthread_mutex_t availableThreadsLock, messagesBufferLock;

pthread_t communicationThreads[COMMUNICATION_WORKERS_MAX];
uint8_t communicationThreadsAvailable = COMMUNICATION_WORKERS_MAX;


//------------------------------------------------------------------------------------------------


class UtilsTest : public ::testing::Test {

protected:

    void SetUp() override
    {
        // Init a random message
        message.transmitted = false;
        message.sender = 9026;
        message.recipient = 8908;
        snprintf( message.body, 256, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc pharetra commodo ligula, id tempor ligula feugiat eu. Quisque condimentum tortor et nunc cursus, suscipit mollis tellus volutpat. Proin semper venenatis eros, eget faucibus nibh facilisis metus" );
        message.created_at = 1561669840;    // 06/27/2019 @ 9:10pm (UTC)

        // Implode Message
        messageSerialized = (char *) malloc( 277 );
        snprintf( messageSerialized, 277, "%d_%d_%ld_%s", message.sender, message.recipient, message.created_at, message.body );
    }

    Message message{};
    MessageSerialized messageSerialized{};

};


//------------------------------------------------------------------------------------------------


/// \brief Tests utils > explode() function.
TEST_F(UtilsTest, Explode)
{
    Message myMessage;

    // Perform explode()
    myMessage = explode( "_", messageSerialized );

    // Check Result
    EXPECT_EQ( message.sender, myMessage.sender );
    EXPECT_EQ( message.recipient, myMessage.recipient );
    EXPECT_EQ( message.created_at, myMessage.created_at );
    EXPECT_STREQ( message.body, myMessage.body );
}

/// \brief Tests utils > inspect() function.
TEST_F(UtilsTest, Implode)
{
    MessageSerialized myMessageSerialized;
    myMessageSerialized = (char *)malloc( 277 );

    // Perform implode()
    implode( "_", message, myMessageSerialized );

    // Check result
    EXPECT_STREQ( myMessageSerialized, messageSerialized );
}

/// \brief Tests utils > isMessageEqual() function.
TEST_F(UtilsTest, IsMessageEqual)
{
    Message myMessage, myMessageDifferent;

    // Check equality
    memcpy( &myMessage, &message, sizeof( Message ) );
    EXPECT_TRUE( isMessageEqual( myMessage, message ) );

    // Check non-equality
    myMessageDifferent = generateRandomMessage();
    EXPECT_FALSE( isMessageEqual( message, myMessageDifferent ) );
}

/// \brief Tests utils > ip2aem() function.
TEST_F(UtilsTest, Ip2Aem)
{
    // TODO
}
