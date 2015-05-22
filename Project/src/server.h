#ifndef HEADER_SERVER
#define HEADER_SERVER

//Socket
int iniSocket();

//Thread
void * server_thread(void *arg);

//TIME
char * getTime();

#endif
