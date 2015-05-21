#ifndef HEADER_CLIENTMSGSERVER
#define HEADER_CLIENTMSGSERVER

int loginProtocol(char *buffer);
int chatProtocol(char *buffer);
int queryProtocol(int first_message, int last_message);


proto_msg *loginSendProto(char *buffer);
void loginReceiveProto(proto_msg *login_response_message);

#endif
