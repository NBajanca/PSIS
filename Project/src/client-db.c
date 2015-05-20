#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "client-db.h"

ClientDB *client_db;

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
	Client* aux = client_db->first;
	
	if (aux == NULL){
		client_db->first = client;
		client_db->counter ++;
		printf("First User Joined (%s)\n", client->user_name);
	}else{	
		while(aux->next != NULL){
			if (strcmp(aux->user_name, client->user_name) == 0) return -1;
			aux = aux->next;
		}
		if (strcmp(aux->user_name, client->user_name) == 0) return -1;
		aux->next = client;
		client_db->counter ++;
		printf("New User Joined (%s)\n", client->user_name);
		
	}
	fflush( stdout );
	return 0;
}

/* removeClient
 * Removes Client from List
 * 
 * */
void removeClient(Client* client){
	Client* aux = client_db->first, *aux2;
	
	if (aux != NULL && aux->next == NULL){
		client_db->first = NULL; //Only one client
		client_db->counter = 0;
	}else if (aux == client){
		client_db->first == aux->next;	//Client is the first client
		client_db->counter --;
	}else if (aux != NULL){
		while(aux->next != NULL){	//Rest of the cases
			if (aux == client){
				aux2->next == aux->next;
				client_db->counter --;
				break;
			}
			aux2 = aux;
			aux = aux->next;
		}
	}
	printf("Client LogOut (%s)\n", client->user_name);
	destroyClient(client);
	return;
}
