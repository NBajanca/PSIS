#ifndef CLIENTDB
#define CLIENTDB

typedef struct Client {
	char *user_name;
	int sock;
	pthread_t thread_id;
	struct Client *next;
} Client;

typedef struct ClientDB {
	int counter;
	Client *first;
} ClientDB;

void iniClientDB();
void destroyClientDB();

Client* createClient();
void destroyClient(Client* client);

int addClient(Client* client);
void removeClient(Client* client);

#endif
