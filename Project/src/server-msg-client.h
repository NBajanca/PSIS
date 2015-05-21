#ifndef HEADER_SERVERMSGCLIENT
#define HEADER_SERVERMSGCLIENT

//Message Processing
int controlProtocol(Client* user);
int loginProtocol(Client* user);
int chatProtocol(Client* user);
int queryProtocol(Client* user);

//LOGIN
proto_msg *loginProto(proto_msg * login_message, Client* user);


//CHAT


//QUERY

#endif
