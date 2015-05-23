#ifndef HEADER_CLIENT
#define HEADER_CLIENT

int iniSocket();
int handleKeyboard();

int loginProtocol(char *buffer);
int chatProtocol(char *buffer);
int queryProtocol(int first_message, int last_message);

proto_msg *loginSendProto(char *buffer);
void loginReceiveProto(proto_msg *login_response_message);

#endif
