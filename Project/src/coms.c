#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "coms.h"

/* receiveMessage
 * 
 * Receives a Message
 * 
 * @ sock_fd
 * 
 * @ returns proto_message (NULL for error)
 * */
proto_msg * receiveMessage(int sock_fd){
	//Receive Message
	proto_msg * message = createProtoMSG( ALLOC_MSG );
	message->msg_size = read(sock_fd, message->msg, BUFFER_SIZE);
	
	//Socket closed abruptly
	if ( message->msg_size == 0){
		destroyProtoMSG(message);
		return NULL;
	}
	
	return message;
}

/* sendMessage
 * 
 * Sends a Message
 * 
 * @ proto_message
 * @ sock
 * 
 * @ int - status (-1 for error, 0 for success)
 * */
int sendMessage(proto_msg * message, int sock_fd){

	if (send(sock_fd, message->msg, message->msg_size, 0) == -1){
		perror("Send ");
		destroyProtoMSG(message);
		return -1;
	}

	destroyProtoMSG(message);
	
	return 0;
}


/* createProtoMSG
 * 
 * Creates Protocol Buffer Structure (MALLOC)
 * 
 * @ msg_alloc (0 for not alloc string msg, 1 for alloc)
 * 
 * @ returns proto_message
 * */
proto_msg* createProtoMSG(int msg_alloc){
	proto_msg* proto_message = (proto_msg*) malloc(sizeof(proto_msg));
	if( proto_message == NULL){
		perror("Proto Message Malloc ");
		exit(-1);
	}
	if (msg_alloc){
		proto_message->msg = (char *) malloc (BUFFER_SIZE*sizeof(char));
		if( proto_message == NULL){
			perror("Proto Message Malloc (MSG) ");
			exit(-1);
		}
	}
	return proto_message;
}

/* destroyProtoMSG
 * 
 * Frees Protocol Buffer Structure (MALLOC)
 * 
 * @ proto_message - structure to free
 * */
void destroyProtoMSG(proto_msg* proto_message){
	free(proto_message->msg);
	free(proto_message);
	return;
}
