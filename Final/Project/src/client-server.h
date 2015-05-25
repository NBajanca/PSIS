#ifndef HEADER_CLIENTSERVER
#define HEADER_CLIENTSERVER

#define USERNAME_SIZE_LIMIT 20
#define CHATSTRING_SIZE_LIMIT 50

proto_msg* protoCreateLogin(LOGIN * login_response);
proto_msg* protoCreateMessage(MESSAGE * control_msg);

#endif
