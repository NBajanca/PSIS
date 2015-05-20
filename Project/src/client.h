#ifndef HEADER_CLIENT
#define HEADER_CLIENT

int iniSocket();
void login(char *buffer);
proto_msg *loginSendProtocol(char *buffer);
void loginReceiveProtocol(proto_msg *login_response_message);

#endif
