#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <pthread.h>

#include "client-server.pb-c.h"
#include "server-relauncher.h"
#include "coms.h"

int exit_server, parent;

void * keep_parent_alive_thread(void *arg);
void * keep_son_alive_thread(void *arg);
void * send_alive_thread(void *arg);
int createServer();

pthread_t keep_parent_alive_thread_id;
pthread_t keep_son_alive_thread_id;
pthread_t send_alive_thread_id;

void * keep_parent_alive_thread(void *arg){
	int fd_fifo = openFIFO(1);
	int should_exit = 0, error = 0;
	proto_msg * message_from_server;
	ALIVE *alive_message;
	
	
	while(! should_exit){
		sleep(2);
		message_from_server = receiveMessageFIFO(fd_fifo);
		if (message_from_server == NULL){
			printf("Server not responding\n");
			if (error == 2) should_exit = 1;
			else error ++;
		}else{
			alive_message = alive__unpack(NULL, message_from_server->msg_size, message_from_server->msg);
			if (alive_message == NULL){
				if (error == 2) should_exit = 1;
				else error ++;
			}else{
				if (alive_message->state == 0){
					error = 0;
				}else if (alive_message->state == 1){
					should_exit = 1;
					exit_server = 1;
					printf("Server turned off by admin. Closing Relauncher...\n");
				}
			
			}
		}
	}
	
	if (exit_server == 0){
		parent = 1;
		pthread_create(&send_alive_thread_id, NULL, send_alive_thread, NULL);
		pthread_create(&keep_son_alive_thread_id, NULL, keep_son_alive_thread, NULL);
	}
	
	pthread_exit(NULL);
}

void * keep_son_alive_thread(void *arg){
	int should_exit = 0;
	int status;
	int pid_proc;
	
	createServer();
	while ( !should_exit ){
		sleep(120);
		pid_proc = wait(&status);
		if(WIFEXITED(status)){
			if (WEXITSTATUS(status) == 0){
				should_exit = 1;
				exit_server = 1;
				printf("Server turned off by admin. Closing Relauncher...\n");
			}
		}
		
		if (should_exit == 0){
			printf("Server (%d) returned with %d code. Rebooting Server...\n", pid_proc, WEXITSTATUS(status));
			createServer();
		}
	}
	
	pthread_exit(NULL);
}

void * send_alive_thread(void *arg){
	int fd_fifo = openFIFO(0);
	int should_exit = 0;
	proto_msg * message_to_server;
	ALIVE alive_message;
	alive__init(&alive_message);
	alive_message.state = 0;
	
	while(! should_exit){
		sleep(2);
		message_to_server = protoCreateAlive(&alive_message);
		if (write(fd_fifo, message_to_server->msg, message_to_server->msg_size) == -1){
			perror("Write ");
			should_exit = 1;
		}
	}
	
	pthread_exit(NULL);
}

int main ( int argc, char **argv ){
	exit_server = 0;
	
	if (argc > 1){
		printf("Relauncher started by the Server. Runing...\n");
		parent = 0;
		pthread_create(&keep_parent_alive_thread_id, NULL, keep_parent_alive_thread, NULL);
	}else{
		printf("Relauncher started. Starting Server...\n");
		parent =1;
		pthread_create(&send_alive_thread_id, NULL, send_alive_thread, NULL);
		pthread_create(&keep_son_alive_thread_id, NULL, keep_son_alive_thread, NULL);
	}
	
	
	
	while (1){
		if(exit_server == 1) break;
	}
	
	exit(0);
}

int createServer(){
	
	int f_ret;
	f_ret = fork();
	char * v[1] = {NULL};
	
	//Fork
	if (!f_ret){ // IF SON
		printf("Server created by Relauncher\n");
		return execve( "server", v , NULL); // Create Process
	}else //PARENT
		return f_ret;
}
