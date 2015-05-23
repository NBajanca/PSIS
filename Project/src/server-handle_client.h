#ifndef HEADER_SERVER_HANDLE_CLIENT
#define HEADER_SERVER_HANDLE_CLIENT

#include "client-server.pb-c.h"
#include "coms.h"

#include "client-server.h"
#include "client-db.h"
#include "log.h"

void * client_thread(void *arg);
void * server_thread(void *arg);

int handleClient();


//Message Processing
int controlProtocol(Client* user);
int loginProtocol(Client* user);
int chatProtocol(Client* user, CHAT *chat);
int queryProtocol(Client* user, QUERY *query);

//LOGIN
proto_msg *loginProto(proto_msg * login_message, Client* user);

//QUERY


int iniClientSocket();

#endif

