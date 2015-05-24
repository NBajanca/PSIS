#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "message-db.h"

MessageDB *message_db;

/* iniMessageDB
 * 
 * Initializes the Message List
 * Changes the global variable
 * 
 * */
void iniMessageDB(){
	message_db = (MessageDB*) malloc(sizeof(MessageDB));
	message_db->first = NULL;
	message_db->last = NULL;
	message_db->counter = 0;
	
	
	
}

/* destroyMessageDB
 * 
 * frees a MessageDN Structure
 * 
 * */
void destroyMessageDB(){
	
	if (message_db->first != NULL){
		Message *aux2, *aux = message_db->first;
		while (aux->next != NULL){
			aux2 = aux;
			aux = aux->next;
			destroyMessage(aux2);
		}
		destroyMessage(aux);
	}
	
	
	free(message_db);
	return;
}

/* createMessage
 * 
 * Initializes a Message Structure
 * 
 * */
Message* createMessage(){
	Message* message = (Message*) malloc(sizeof(Message));
	message->id = -1;
	message->msg = NULL;
	message->next = NULL;
	
	return message;
}

/* destroyMessage
 * 
 * frees a Message Structure
 * 
 * */
void destroyMessage(Message* message){
	free(message->msg);
	free(message);
	return;
}

/* addMessage
 * Initializes a Message Structure
 * 
 * @return - success or failiure
 * 
 * */
int addMessage(Message* message){
	
	//Critical zone
	
	
	if (message_db->last == NULL){
		message_db->first = message;
		message_db->last = message;
		message_db->counter ++;
		message->id = message_db->counter;
	}else{	
		message_db->last->next = message;
		message_db->last = message;
		message_db->counter ++;
		message->id = message_db->counter;
	}
	
	//End of critical zone
	return 0;
}

Message *getLastMessage (){
	
	
	if (message_db->last == NULL){
		return NULL;
	}else{	
		return message_db->last;
	}
	
}
