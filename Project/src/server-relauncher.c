#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <fcntl.h>
#include "client-server.pb-c.h"
#include "server-relauncher.h"

int openFIFO(int type){
	int fd_fifo;
	
	if( access("/tmp/fifo_alive", F_OK ) == -1 ) {
		if (mkfifo("/tmp/fifo_alive", 0600) != 0 ){
			perror("mkfifo ");
			exit(-1);
		}
	}
	
	switch(type){
		case 0:
			fd_fifo = open("/tmp/fifo_alive", O_WRONLY);
			break;
		case 1:
			fd_fifo = open("/tmp/fifo_alive", O_RDONLY | O_NONBLOCK);
			break;
	}
	
	if(fd_fifo == -1){
		perror("Open (FIFO) ");
		exit(-1);
	}
	sync();
	
	return fd_fifo;

}

proto_msg * receiveMessageFIFO(int sock_fd){
	//Receive Message
	proto_msg * message = createProtoMSG( ALLOC_MSG );
	message->msg_size = read(sock_fd, message->msg, STD_SIZE);
	
	//Socket closed abruptly
	if (message->msg_size <= 0){
		destroyProtoMSG(message);
		return NULL;
	}
	
	return message;
}


proto_msg* protoCreateAlive(ALIVE * alive){
	proto_msg * proto_message = createProtoMSG(DONT_ALLOC_MSG);
	
	proto_message->msg_size = alive__get_packed_size(alive);
	proto_message->msg = malloc(proto_message->msg_size);
	alive__pack(alive, proto_message->msg);
	
	return proto_message;
}
