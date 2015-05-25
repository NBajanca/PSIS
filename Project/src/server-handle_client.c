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
#include "server-handle_admin.h"


pthread_mutex_t message_mutex;
pthread_cond_t message_mutex_cv;

pthread_t server_thread_id;

pthread_t getClientThread(){
	return server_thread_id;
}

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
		if (getExit() == 1) should_exit = 1;
	}while (user->user_name == NULL || should_exit);
	//Comunication with Client
	while(! should_exit){
		if (controlProtocol(user) != 0) should_exit = 1;
		if (getExit() == 1) should_exit = 1;
	}
	//Close Connection
	
	//Send Disconnect
	MESSAGE disc_message;
	message__init(&disc_message);
	disc_message.next_message = 2;
	proto_msg * disconnect = protoCreateMessage(&disc_message);
	sendMessage(disconnect, user->sock);
		
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
		if (getExit() == 1) break;
		message_to_broadcast = getLastMessage();
		sprintf( aux, "[%d] %s", message_to_broadcast->id, message_to_broadcast->msg);
		chat_message.message = strdup(aux);
		pthread_mutex_unlock (&message_mutex);
		
		chat_broadcast.chat = &chat_message;
		broadcast = protoCreateMessage(&chat_broadcast);
		
		sock_list = getSockList(&number_of_users);
		if (sock_list == NULL){
			destroyProtoMSG(broadcast);	
			continue;
		}
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
	fd_set readfds;
	
	while (!should_exit){
		FD_ZERO(&readfds);
		FD_SET(sock_fd, &readfds);
		struct timeval tv = {30, 0};
		if (select(sock_fd + 1, &readfds, NULL, NULL, &tv) == -1){
			perror("[System Error] Select (Server Thread)");
			exit(-1);
		}
		if (FD_ISSET(sock_fd, &readfds)) {
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
		if (getExit() == 1) should_exit = 1;
	}
	
	//Close Connection
	close(sock_fd);
	
	//Waits for threads to close
	destroyClientDB();
	
	//Wakes up broadcast thread to close it
	pthread_mutex_lock (&message_mutex);
	pthread_cond_signal(&message_mutex_cv);
	pthread_mutex_unlock (&message_mutex);
	
	//
	destroyMessageDB();
	pthread_mutex_destroy(&message_mutex);
	pthread_cond_destroy(&message_mutex_cv);
	pthread_exit(NULL);
}

int handleClient(){
	proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Initializing Client Handler");
	addToLog(message_to_log);
	
	pthread_create(&server_thread_id, NULL, server_thread, NULL);
	
	message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Client Handler Ready");
	addToLog(message_to_log);
	return 0;
}

//Message Processing
int loginProtocol(Client* user){
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(user->sock, &readfds);
	struct timeval tv = {30, 0};
	if (select(user->sock + 1, &readfds, NULL, NULL, &tv) == -1){
		perror("[System Error] Select (Login Protocol)");
		exit(-1);
	}
	if (FD_ISSET(user->sock, &readfds)) {
		//Receive Message
		proto_msg * login_message = receiveMessage(user->sock);
		if (login_message == NULL) return -1;
		
		//Login Protocol - PROTO and CLIENTDB
		proto_msg * proto_message = loginProto(login_message, user);
		if (proto_message == NULL) return 0;
		destroyProtoMSG(login_message);
		
		//Send Message
		send(user->sock, proto_message->msg, proto_message->msg_size, 0);
		destroyProtoMSG(proto_message);
	}
	
	return 0;
}

int controlProtocol(Client* user){
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(user->sock, &readfds);
	struct timeval tv = {30, 0};
	if (select(user->sock + 1, &readfds, NULL, NULL, &tv) == -1){
		perror("[System Error] Select (Control Protocol)");
		exit(-1);
	}
	if (FD_ISSET(user->sock, &readfds)) {
		//Receive Message
		proto_msg * control_message = receiveMessage(user->sock);
		if (control_message == NULL) return -1;
		
		//Unmarshal incoming message
		MESSAGE *control = message__unpack(NULL, control_message->msg_size,(const uint8_t *) control_message->msg);
		if (control == NULL){
			printf("[System - Client Handler] Message from client in incorrect format (Discarted)\n");
			return 0;
		}
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
	}
	
	return 0;
}

int chatProtocol(Client* user, CHAT *chat){
	char aux[STD_SIZE];
	Message *chat_message = createMessage();
	if (chat_message == NULL) return -1;
	
	sprintf(aux ,"%s : %s",user->user_name, chat->message);
	chat_message->msg = strdup(aux);
	
	pthread_mutex_lock (&message_mutex);
	addMessage(chat_message);
	pthread_cond_signal(&message_mutex_cv);
	pthread_mutex_unlock (&message_mutex);
	
	return 0;
}

int queryProtocol(Client* user, QUERY *query){
	proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"%s  QUERY : %d -> %d",user->user_name, query->id_min, query->id_max);
	
	int number_of_messages, first = query->id_min, last = query->id_max;
	Message ** list_of_messages;
	
	addToLog(message_to_log);
	
	pthread_mutex_lock (&message_mutex);
	list_of_messages = getMessages (first, last, &number_of_messages);
	pthread_mutex_unlock (&message_mutex);
	
	if (list_of_messages == NULL) return -1;
	
	CHAT **chat_messages;
    chat_messages = malloc (sizeof (CHAT*) * (number_of_messages));
    if (chat_messages == NULL){
		perror("[System - Client Handler Error] Malloc (chat messages)\n");
		exit(-1);
	}
    int i;
    
    for (i = 0; i < number_of_messages; i++)
	{
		chat_messages[i] = malloc (sizeof (CHAT));
		if (chat_messages == NULL){
			perror("[System - Client Handler Error] Malloc (chat message)\n");
			exit(-1);
		}
		chat__init (chat_messages[i]);
		chat_messages[i]->message = strdup(list_of_messages[i]->msg);
		chat_messages[i]->has_id = 1;
		chat_messages[i]->id = list_of_messages[i]->id;
	}
	
	QUERY query_message;
	query__init(&query_message);
	query_message.id_min = first;
	query_message.id_max = first + number_of_messages - 1;
	query_message.n_messages = number_of_messages;
	query_message.messages = chat_messages;
	
	MESSAGE message;
	message__init(&message);
	message.next_message = 1;
	message.query = &query_message;
	
	proto_msg* query_message_response =  protoCreateMessage(&message);
	if (sendMessage(query_message_response, user->sock) == -1) return -1;
	
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
	LOGIN *login = login__unpack(NULL, login_message->msg_size, (const uint8_t *) login_message->msg);
	if (login == NULL){
		printf("[System - Client Handler] Message from client in incorrect format (Discarted)\n");
		return NULL;
	}
	
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
	struct sockaddr_in addr;
	
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
		exit(-2);
	}
	
	if( listen(sock_fd, 10) == -1){
		proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
		message_to_log->msg_size = sprintf(message_to_log->msg ,"Listen (Client) : %s", strerror(errno));
		addToLog(message_to_log);
		exit(-1);
	}
	
	return sock_fd;
}
