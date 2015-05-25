#ifndef HEADER_COMS
#define HEADER_COMS

#define BUFFER_SIZE 1024
#define STD_SIZE 100

#define ALLOC_MSG 1
#define DONT_ALLOC_MSG 0

typedef struct proto_msg {
	char *msg;
	size_t msg_size;
} proto_msg;

proto_msg * receiveMessage(int sock);
int sendMessage(proto_msg * message, int sock);

proto_msg* createProtoMSG(int msg_alloc);
void destroyProtoMSG(proto_msg* proto_message);

#endif
