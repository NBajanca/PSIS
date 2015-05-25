#include "message-db.h"
#include "unity.h"


MessageDB *message_db;



void setUp(void)

{

 iniMessageDB();

}



void test_ini(void)

{

 UnityAssertEqualNumber((_U_SINT)((0)), (_U_SINT)((message_db->counter)), (((void *)0)), (_U_UINT)14, UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((_U_SINT)((((void *)0))), (_U_SINT)((message_db->first)), (((void *)0)), (_U_UINT)15, UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((_U_SINT)((((void *)0))), (_U_SINT)((message_db->last)), (((void *)0)), (_U_UINT)16, UNITY_DISPLAY_STYLE_INT);

}



void test_create(void)

{

 Message* test = createMessage();

 UnityAssertEqualNumber((_U_SINT)((-1)), (_U_SINT)((test->id)), (((void *)0)), (_U_UINT)22, UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((_U_SINT)((((void *)0))), (_U_SINT)((test->msg)), (((void *)0)), (_U_UINT)23, UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((_U_SINT)((((void *)0))), (_U_SINT)((test->next)), (((void *)0)), (_U_UINT)24, UNITY_DISPLAY_STYLE_INT);

}



void test_add(void)

{

 Message* test = createMessage();

 int status = addMessage(test);



 UnityAssertEqualNumber((_U_SINT)((0)), (_U_SINT)((status)), (((void *)0)), (_U_UINT)32, UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((_U_SINT)((1)), (_U_SINT)((message_db->counter)), (((void *)0)), (_U_UINT)33, UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((_U_SINT)((test)), (_U_SINT)((message_db->first)), (((void *)0)), (_U_UINT)34, UNITY_DISPLAY_STYLE_INT);

 UnityAssertEqualNumber((_U_SINT)((test)), (_U_SINT)((message_db->last)), (((void *)0)), (_U_UINT)35, UNITY_DISPLAY_STYLE_INT);



}



void test_getLastMessage(void){



 Message* test, *to_test;

 int i;



 for (i = 0; i < 5; i++){

  test = createMessage();

  addMessage(test);

  to_test = getLastMessage();

  UnityAssertEqualNumber((_U_SINT)((test)), (_U_SINT)((to_test)), (((void *)0)), (_U_UINT)48, UNITY_DISPLAY_STYLE_INT);

 }



 UnityAssertEqualNumber((_U_SINT)((5)), (_U_SINT)((message_db->counter)), (((void *)0)), (_U_UINT)51, UNITY_DISPLAY_STYLE_INT);

}



void tearDown(void)

{

 destroyMessageDB();

}
