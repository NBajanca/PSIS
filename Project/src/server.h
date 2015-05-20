#ifndef HEADER_SERVER
#define HEADER_SERVER

typedef struct ClientDB {
	int counter;
	struct ClientDB *first;
} ClientDB;

typedef struct Client {
	char *user_name;
	int sock;
	pthread_t thread_id;
	struct Client *next;
} Client;

int iniSocket();
void iniClientDB();
proto_msg *loginProtocol(proto_msg * login_message);

#endif
