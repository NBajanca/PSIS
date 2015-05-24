#ifndef HEADER_MESSAGE_DB
#define HEADER_MESSAGE_DB

typedef struct Message {
	int id;
	char *msg;
	struct Message *next;
} Message;

typedef struct MessageDB {
	int counter;
	Message *first;
	Message *last;
} MessageDB;

void iniMessageDB();
void destroyMessageDB();

Message* createMessage();
void destroyMessage(Message* message);

int addMessage(Message* message);
Message *getLastMessage ();

#endif
