#include "message-db.h"
#include "unity.h"



void setUp(void)
{
}

void tearDown(void)
{
}

void test_AddedMessageToLog(void)
{
  
  //If a message is successfully added the function should return 0
  TEST_ASSERT_EQUAL(0, addMessage("ola mundo"));
 
}

void test_RemovedMessageFromLog(void)
{
 destroyMessage("ola mundo");
 TEST_ASSERT_NULL(message->msg);
}

void test_GotLastMessage(void)
{
    Message *getLastMessage ();
    //This should be true if the last message is retrieved
    TEST_ASSERT_NOT_NULL(message_db->last);
}

void test_MessageList(void)
{
 


}
