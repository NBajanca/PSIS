#ifndef HEADER_SERVER
#define HEADER_SERVER

//Socket
int iniSocket();

//Thread
void * server_thread(void *arg);

//Message Processing
void login(Client* user);

//Proto
proto_msg *loginProtocol(proto_msg * login_message, Client* user);


#endif
