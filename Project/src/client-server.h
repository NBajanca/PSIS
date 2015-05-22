#ifndef HEADER_CLIENTSERVER
#define HEADER_CLIENTSERVER

#define BUFFER_SIZE 1024
#define CMD_SIZE 100

typedef struct proto_msg {
	char *msg;
	size_t msg_size;
} proto_msg;

proto_msg * receiveMessage(int sock);
int sendMessage(proto_msg * message, int sock);

proto_msg* createProtoMSG();
void destroyProtoMSG(proto_msg* proto_message);


proto_msg* protoCreateLogin(LOGIN * login_response);
proto_msg* protoCreateMessage(MESSAGE * control_msg);


#endif
