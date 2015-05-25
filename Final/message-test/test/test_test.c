#include "unity.h"
#include "message-db.h"
#include <stdlib.h>

MessageDB *message_db;

void setUp(void)
{
	iniMessageDB();
}

void test_ini(void)
{
	TEST_ASSERT_EQUAL_INT(0, message_db->counter);
	TEST_ASSERT_EQUAL(NULL, message_db->first);
	TEST_ASSERT_EQUAL(NULL, message_db->last);
}

void test_create(void)
{
	Message* test = createMessage();
	TEST_ASSERT_EQUAL_INT(-1, test->id);
	TEST_ASSERT_EQUAL(NULL, test->msg);
	TEST_ASSERT_EQUAL(NULL, test->next);
}

void test_add(void)
{
	Message* test = createMessage();
	int status =  addMessage(test);

	TEST_ASSERT_EQUAL_INT(0, status);
	TEST_ASSERT_EQUAL_INT(1, message_db->counter);
	TEST_ASSERT_EQUAL(test, message_db->first);
	TEST_ASSERT_EQUAL(test, message_db->last);

}

void test_getLastMessage(void){

	Message* test, *to_test;
	int i;
	
	for (i = 0; i < 5; i++){
		test = createMessage();
		addMessage(test);
		to_test = getLastMessage();
		TEST_ASSERT_EQUAL(test, to_test);
	}

	TEST_ASSERT_EQUAL_INT(5, message_db->counter);
}

void tearDown(void)
{
	destroyMessageDB();
}
