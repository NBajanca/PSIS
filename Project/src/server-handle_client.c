#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "server-handle_client.h"

//https://computing.llnl.gov/tutorials/pthreads/#ConditionVariables

void * client_thread(void *arg){
	Client* user = (Client*) arg;
	int should_exit = 0;
	
	//Login Process
	do{
		if (loginProtocol(user) == -1){
			close(user->sock);
			pthread_exit(NULL);
			break;
		}
	}while (user->user_name == NULL);
	//Comunication with Client
	while(! should_exit){
		if (controlProtocol(user) != 0) should_exit = 1;
	}
	//Close Connection
	close(user->sock);
	removeClient(user);
	pthread_exit(NULL);
}

void * broadcast_thread(void *arg){
	
	
	
	pthread_exit(NULL);
}

void * server_thread(void *arg){
	//Variables
	//General
	int should_exit = 0;
	
	//Socket
	int sock_fd, new_sock;
		
	//Program
	//Socket
	sock_fd = iniClientSocket();
	
	//User List
	iniClientDB();
	
	pthread_t broadcast_thread_id;
	pthread_create(&broadcast_thread_id, NULL, broadcast_thread, NULL);
	
	while (!should_exit){
		new_sock = accept(sock_fd, NULL, NULL);
		if(sock_fd == -1){
			proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
			message_to_log->msg_size = sprintf(message_to_log->msg ,"Accept (Client) : %s", strerror(errno));
			addToLog(message_to_log);
			exit(-1);
		}
		Client* user = createClient();
		user->sock = new_sock;
		
		pthread_create(&user->thread_id, NULL, client_thread, user);
	}
	
	//Close Connection
	close(sock_fd);
	destroyClientDB();
	pthread_exit(NULL);
}

int handleClient(){
	proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Initializing Client Handler");
	addToLog(message_to_log);
	
	pthread_t server_thread_id;
	pthread_create(&server_thread_id, NULL, server_thread, NULL);
	
	message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Client Handler Ready");
	addToLog(message_to_log);
	return 0;
}

//Message Processing
int loginProtocol(Client* user){
	//Receive Message
	proto_msg * login_message = receiveMessage(user->sock);
	if (login_message == NULL) return -1;
	
	//Login Protocol - PROTO and CLIENTDB
	proto_msg * proto_message = loginProto(login_message, user);
	destroyProtoMSG(login_message);
	
	//Send Message
	send(user->sock, proto_message->msg, proto_message->msg_size, 0);
	destroyProtoMSG(proto_message);
	
	return 0;
}

int controlProtocol(Client* user){
	//Receive Message
	proto_msg * control_message = receiveMessage(user->sock);
	if (control_message == NULL) return -1;
	
	//Unmarshal incoming message
	MESSAGE *control = message__unpack(NULL, control_message->msg_size, control_message->msg);
	destroyProtoMSG(control_message);
	fflush(stdout);
	
	switch (control->next_message){
		case 0:
			if (chatProtocol(user,control->chat) == -1) return -1;
			break;
		case 1:
			if (queryProtocol(user, control->query) == -1)return -1;
			break;
		case 2:
			return 1;
			break;
	}
	
	return ;
}

int chatProtocol(Client* user, CHAT *chat){
	proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"%s : %s",user->user_name, chat->message);
	addToLog(message_to_log);
	
	//IMPLEMENTAR LOG E BROADCAST
	
	return 0;
}

int queryProtocol(Client* user, QUERY *query){
	proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"%s  QUERY : %d -> %d",user->user_name, query->id_min, query->id_max);
	addToLog(message_to_log);
	
	//IMPLEMENTAR LOG E RESPOSTA
	
	return 0;
}


//LOGIN

/* loginProto
 * 
 * Deals with Login Messages
 * Does the unmarshal and the user name verification
 * Prepares the response for the client
 * 
 * @ login_message - Structure with the login message from the client
 * @ returns proto_message - Marshaled message for the client
 * */
proto_msg *loginProto(proto_msg * login_message, Client* user){
	//Unmarshal incoming message
	LOGIN *login = login__unpack(NULL, login_message->msg_size, login_message->msg);
	
	//Prepare response message
	LOGIN login_response;
	login__init(&login_response);
	login_response.username = strdup(login->username);
	login_response.has_validation = 1;
	
	
	user->user_name = strdup(login->username);
	
	if (addClient(user) != -1){
		login_response.validation = 0;
	}else{
		login_response.validation = 1;
		user->user_name = NULL;
	}
	
	//Marshal response message
	proto_msg * proto_message = protoCreateLogin(&login_response);
	
	return proto_message;
}


int iniClientSocket(){
	int sock_fd;
	struct sockaddr_in addr, client_addr;
	int addr_len; 
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1){
		proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
		message_to_log->msg_size = sprintf(message_to_log->msg ,"Socket (Client) : %s", strerror(errno));
		addToLog(message_to_log);
		exit(-1);
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3000);		/*Port*/
    addr.sin_addr.s_addr = INADDR_ANY;	/*IP*/	

	
	if( bind(sock_fd, (struct sockaddr *)  &addr, sizeof(addr)) == -1){
		proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
		message_to_log->msg_size = sprintf(message_to_log->msg ,"Bind (Client) : %s", strerror(errno));
		addToLog(message_to_log);
		exit(-1);
	}
	
	if( listen(sock_fd, 10) == -1){
		proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
		message_to_log->msg_size = sprintf(message_to_log->msg ,"Listen (Client) : %s", strerror(errno));
		addToLog(message_to_log);
		exit(-1);
	}
	
	return sock_fd;
}
