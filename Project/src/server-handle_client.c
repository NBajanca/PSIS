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

pthread_mutex_t message_mutex;
pthread_cond_t message_mutex_cv;

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
	Message * message_to_broadcast;
	int *sock_list, number_of_users, i;
	proto_msg* broadcast;
	char aux[STD_SIZE];
	
	while(1){
		
		MESSAGE chat_broadcast;
		message__init(&chat_broadcast);
		chat_broadcast.next_message = 0;
		
		CHAT chat_message;
		chat__init(&chat_message);
		
		pthread_mutex_lock (&message_mutex);
		pthread_cond_wait(&message_mutex_cv, &message_mutex);
		message_to_broadcast = getLastMessage();
		sprintf( aux, "%d - %s", message_to_broadcast->id, message_to_broadcast->msg);
		chat_message.message = strdup(aux);
		pthread_mutex_unlock (&message_mutex);
		
		chat_broadcast.chat = &chat_message;
		broadcast = protoCreateMessage(&chat_broadcast);
		
		sock_list = getSockList(&number_of_users);
		for (i = 0; i < number_of_users; i++){
			send(sock_list[i], broadcast->msg, broadcast->msg_size, 0);
		}
		destroyProtoMSG(broadcast);	
	}
	
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
	
	//User and Message Lists
	iniClientDB();
	iniMessageDB();
	
	pthread_mutex_init(&message_mutex, NULL);
	pthread_cond_init (&message_mutex_cv, NULL);
	
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
	destroyMessageDB();
	pthread_mutex_destroy(&message_mutex);
	pthread_cond_destroy(&message_mutex_cv);
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
	
	Message *chat_message = createMessage();
	chat_message->msg = strdup(message_to_log->msg);
	
	addToLog(message_to_log);
	
	pthread_mutex_lock (&message_mutex);
	addMessage(chat_message);
	pthread_cond_signal(&message_mutex_cv);
	pthread_mutex_unlock (&message_mutex);	
	
	//IMPLEMENTAR BROADCAST
	
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
