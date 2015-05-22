#ifndef HEADER_SERVERMSGCLIENT
#define HEADER_SERVERMSGCLIENT

//Message Processing
int controlProtocol(Client* user);
int loginProtocol(Client* user);
int chatProtocol(Client* user, CHAT *chat);
int queryProtocol(Client* user, QUERY *query);

//LOGIN
proto_msg *loginProto(proto_msg * login_message, Client* user);

//QUERY




#endif
