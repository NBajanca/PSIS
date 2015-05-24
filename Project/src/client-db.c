#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "client-db.h"
#include "log.h"

ClientDB *client_db;
pthread_mutex_t client_mutex;

/* iniClientDB
 * 
 * Initializes the Client List
 * Changes the global variable
 * 
 * */
void iniClientDB(){
	client_db = (ClientDB*) malloc(sizeof(ClientDB));
	client_db->first = NULL;
	client_db->counter = 0;
	
	pthread_mutex_init(&client_mutex, NULL);
	
}

/* destroyClientDB
 * 
 * frees a ClientDN Structure
 * 
 * */
void destroyClientDB(){
	
	if (client_db->first != NULL){
		Client *aux2, *aux = client_db->first;
		while (aux->next != NULL){
			aux2 = aux;
			aux = aux->next;
			destroyClient(aux2);
		}
		destroyClient(aux);
	}
	pthread_mutex_destroy(&client_mutex);
	
	free(client_db);
	return;
}

/* createClient
 * 
 * Initializes a Client Structure
 * 
 * */
Client* createClient(){
	Client* client = (Client*) malloc(sizeof(Client));
	client->user_name = NULL;
	client->next = NULL;
	
	return client;
}

/* destroyClient
 * 
 * frees a Client Structure
 * 
 * */
void destroyClient(Client* client){
	free(client->user_name);
	free(client);
	return;
}

/* addClient
 * Initializes a Client Structure
 * 
 * @return - success or failiure
 * 
 * */
int addClient(Client* client){
	pthread_mutex_lock (&client_mutex);
	//Critical zone
	Client* aux = client_db->first;
	if (aux == NULL){
		client_db->first = client;
		client_db->counter ++;
	}else{
		while(aux->next != NULL){
			if (strcmp(aux->user_name, client->user_name) == 0){
				pthread_mutex_unlock (&client_mutex);
				return -1;
			}
			aux = aux->next;
		}
		if (strcmp(aux->user_name, client->user_name) == 0) {
				pthread_mutex_unlock (&client_mutex);
				return -1;
			}
		aux->next = client;
		client_db->counter ++;
	}
	
	//End of critical zone
	pthread_mutex_unlock (&client_mutex);
	
	proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"New User Joined (%s)", client->user_name);
	addToLog(message_to_log);
	
	return 0;
}

/* removeClient
 * Removes Client from List
 * 
 * */
void removeClient(Client* client){
	pthread_mutex_lock (&client_mutex);
	
	//Critical zone
	Client* aux = client_db->first, *aux2 = client_db->first;
	if (aux == NULL){
		printf("ERROR, CLIENT NOT FOUND\n");
	}else if (aux == client){
		client_db->first = aux->next;	//Client is the first client
		client_db->counter --;
	}else if (aux->next == NULL){
		printf("ERROR, CLIENT NOT FOUND\n");
	}else {
		aux = aux->next;
		do{
			if (aux == client){
				aux2->next = aux->next;
				client_db->counter --;
				break;
			}
			aux2 = aux;
			aux = aux->next;
		}while (aux != NULL);
	}
	
	//End of critical zone
	pthread_mutex_unlock (&client_mutex);
	
	proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Client LogOut (%s)", client->user_name);
	addToLog(message_to_log);
	
	destroyClient(client);
	return;
}

/* getSockList()
 * 
 * */
int *getSockList(int *number_of_users){
	int * sock_list;
	pthread_mutex_lock (&client_mutex);
	//Critical zone
	*number_of_users = client_db->counter;
	if (client_db->counter == 0) return NULL;
	sock_list = (int*) malloc (client_db->counter * sizeof (int));
	
	int i = 0;
	Client* aux = client_db->first;
	do{
		sock_list[i] = aux->sock;
		aux = aux->next;
		i++;
	}while (aux != NULL);

	//End of critical zone
	pthread_mutex_unlock (&client_mutex);

	return sock_list;
}
