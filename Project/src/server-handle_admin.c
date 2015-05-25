#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "server-handle_admin.h"
#include "client-server.pb-c.h"
#include "coms.h"
#include "server-admin.h"
#include "log.h"

int exit_server;

int handleAdmin(){
	proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Initializing Admin Handler");
	addToLog(message_to_log);
	
	exit_server = 0;
	int sock_fd = iniSocket();
	
	message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Admin Handler Ready");
	addToLog(message_to_log);
	
	while(1){
		if (handleNewAdmin (sock_fd) != 0) break;
	}
	exit_server = 1;
	close(sock_fd);
	return 0;
}

int handleNewAdmin (int sock_fd){
	int action = 0; 
	proto_msg * message_to_log;
	
	//Acept New Admin
	int new_sock_fd = accept(sock_fd, NULL, NULL);
	if(new_sock_fd == -1){
		message_to_log = createProtoMSG( ALLOC_MSG );
		message_to_log->msg_size = sprintf(message_to_log->msg ,"Accept (Admin) : %s", strerror(errno));
		addToLog(message_to_log);
		exit(-1);
	}
	message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Admin LogIn");
	addToLog(message_to_log);
	
	//Handle Admin Requests
	while(!action){
		action = handleAdminRequests(new_sock_fd);
	}
	
	//Close Admin Connection
	message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Admin LogOut");
	addToLog(message_to_log);
	close(new_sock_fd);
	
	if (action == 1) return 1;
	return 0;
}

int handleAdminRequests(int sock_fd){
	proto_msg * message = receiveMessage(sock_fd);
	if (message == NULL) return -1;
	
	//Unmarshal incoming message
	ADMIN *admin = admin__unpack(NULL, message->msg_size, message->msg);
	destroyProtoMSG(message);
	
	proto_msg *message_to_log;
	
	switch (admin->action){
		case 0:
			message_to_log = createProtoMSG( ALLOC_MSG );
			message_to_log->msg_size = sprintf(message_to_log->msg ,"Received LOG request from Admin");
			addToLog(message_to_log);
			sendLog(sock_fd);
			break;
		case 1:
			message_to_log = createProtoMSG( ALLOC_MSG );
			message_to_log->msg_size = sprintf(message_to_log->msg ,"Received QUIT request from Admin");
			addToLog(message_to_log);
			return 1;
		case 2:
			message_to_log = createProtoMSG( ALLOC_MSG );
			message_to_log->msg_size = sprintf(message_to_log->msg ,"Received DISC request from Admin");
			addToLog(message_to_log);
			return 2;
			break;
	}
	return 0;
}

int sendLog(int sock_fd){
	proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Sending LOG to Admin");
	addToLog(message_to_log);
	
	ADMIN message;
	admin__init(&message);
	message.action = 0;
	
	proto_msg* log_string = getLog();
	message.log = log_string->msg;
	
	proto_msg* log_message = protoCreateAdmin(&message);
	destroyProtoMSG(log_string);
	
	if (sendMessage(log_message,  sock_fd) == -1) return -1;
	
	return 0;
}

int iniSocket(){
	struct sockaddr_in addr, client_addr;
	int addr_len; 
	int sock_fd;
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1){
		proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
		message_to_log->msg_size = sprintf(message_to_log->msg ,"Socket (Admin) : %s", strerror(errno));
		addToLog(message_to_log);
		exit(-1);
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3001);		/*Port*/
    addr.sin_addr.s_addr = INADDR_ANY;	/*IP*/

	if( bind(sock_fd, (struct sockaddr *)  &addr, sizeof(addr)) == -1){
		proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
		message_to_log->msg_size = sprintf(message_to_log->msg ,"Bind (Admin) : %s", strerror(errno));
		addToLog(message_to_log);
		exit(-2);
	}
	
	if( listen(sock_fd, 1) == -1){
		proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
		message_to_log->msg_size = sprintf(message_to_log->msg ,"Listen (Admin) : %s", strerror(errno));
		addToLog(message_to_log);
		exit(-1);
	}
	
	return sock_fd;
}

int getExit(){
	return exit_server;
}
