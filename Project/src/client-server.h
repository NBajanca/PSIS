#ifndef HEADER_CLIENTSERVER
#define HEADER_CLIENTSERVER

proto_msg* protoCreateLogin(LOGIN * login_response);
proto_msg* protoCreateMessage(MESSAGE * control_msg);

#endif
