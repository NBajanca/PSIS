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
	message_db->counter = 0;
	message_db->last = NULL;
	
}

/* destroyMessageDB
 * 
 * frees a MessageDB Structure
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
	message->entry = NULL;
	message->next = NULL;
	
	return message;
}

/* destroyMessage
 * 
 * frees a Message Structure
 * 
 * */
void destroyMessage(Message* message){
	free(message->entry);
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
	Message* aux = message_db->first;
	
	if (aux == NULL){
		message_db->first = message;
		message_db->counter ++;
		printf("First Message (%s)\n", message->entry);
	}else{	
		while(aux->next != NULL){
			if (strcmp(aux->entry, message->entry) == 0) return -1;
			aux = aux->next;
		}
		if (strcmp(aux->entry, message->entry) == 0) return -1;
		aux->next = message;
		message_db->counter ++;
		printf("Next Message (%s)\n", message->entry);
		
	}
	fflush( stdout );
	return 0;
}

/* removeMessage
 * Removes Message from list
 * 
 * */
/*
void removeMessage(Message* message){
	Message* aux = message_db->first, *aux2;
	
	if (aux != NULL && aux->next == NULL){
		message_db->first = NULL; //Only one message
		message_db->counter = 0;
	}else if (aux == message){
		Message_db->first == aux->next;	//Message is the first message
		Message_db->counter --;
	}else if (aux != NULL){
		while(aux->next != NULL){	//Rest of the cases
			if (aux == message){
				aux2->next == aux->next;
				message_db->counter --;
				break;
			}
			aux2 = aux;
			aux = aux->next;
		}
	}
	printf("Message deleted (%s)\n", message->entry);
	destroyMessage(message);
	return;
*/
}
