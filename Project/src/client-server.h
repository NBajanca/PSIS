#ifndef HEADER_CLIENTSERVER
#define HEADER_CLIENTSERVER

#define BUFFER_SIZE 1024

typedef struct proto_msg {
	char *msg;
	size_t msg_size;
} proto_msg;

proto_msg* protoCreateLogin(LOGIN * login_response);
void destroyProtoMSG(proto_msg* proto_message);
proto_msg* createProtoMSG();


#endif
