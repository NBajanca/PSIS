#ifndef HEADER_CLIENT
#define HEADER_CLIENT

int iniSocket();
proto_msg *loginSendProtocol(char *buffer);
int loginReceiveProtocol(proto_msg *login_response_message);

#endif
