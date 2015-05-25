#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>


#include <pthread.h>

#include "client-server.pb-c.h"
#include "server-relauncher.h"
#include "coms.h"

#define BIND 0
#define UNDEFINED 1

int exit_server, parent;

void * keep_parent_alive_thread(void *arg);
void * keep_son_alive_thread(void *arg);
void * send_alive_thread(void *arg);
int createServer(int error_type);

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
			printf("[System Relauncher] Server not responding\n");
			if (error == 2) should_exit = 1;
			else error ++;
		}else{
			alive_message = alive__unpack(NULL, message_from_server->msg_size, (const uint8_t *) message_from_server->msg);
			if (alive_message == NULL){
				if (error == 2) should_exit = 1;
				else error ++;
			}else{
				if (alive_message->state == 0){
					error = 0;
				}else if (alive_message->state == 1){
					should_exit = 1;
					exit_server = 1;
					printf("[System Relauncher] Server turned off by admin. Closing Relauncher...\n");
				}
			
			}
			alive__free_unpacked(alive_message, NULL);
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
	
	createServer(UNDEFINED);
	while ( !should_exit ){
		pid_proc = wait(&status);
		if(WIFEXITED(status) &&  !WIFSIGNALED(status)){
			if (WEXITSTATUS(status) == 0){
				should_exit = 1;
				exit_server = 1;
				printf("[System Relauncher] Server turned off by admin. Closing Relauncher...\n"); fflush(stdout);
			}
		}
		
		if (should_exit == 0){
			printf("[System Relauncher] Server (%d) returned with %d code. Rebooting Server...\n", pid_proc, WEXITSTATUS(status));
			if(WEXITSTATUS(status) == 254) createServer(BIND);
			else createServer(UNDEFINED);
		}
	}
	
	pthread_exit(NULL);
}

void * send_alive_thread(void *arg){
	int fd_fifo = openFIFO(0);
	int should_exit = 0;
	ALIVE alive_message;
	alive__init(&alive_message);
	alive_message.state = 0;
	proto_msg * message_to_server = protoCreateAlive(&alive_message);
	
	while(! should_exit){
		sleep(2);
		if (write(fd_fifo, message_to_server->msg, message_to_server->msg_size) == -1){
			perror("[System Relauncher Error] Write ");
			should_exit = 1;
		}
	}
	
	destroyProtoMSG(message_to_server);
	pthread_exit(NULL);
}

int main ( int argc, char **argv ){
	exit_server = 0;
	
	if (argc > 1){
		parent = 0;
		printf("[System Relauncher] Relauncher started by the Server. Runing...\n");
		pthread_create(&keep_parent_alive_thread_id, NULL, keep_parent_alive_thread, NULL);
	}else{
		parent =1;
		printf("[System Relauncher] Relauncher started. Starting Server...\n");
		pthread_create(&send_alive_thread_id, NULL, send_alive_thread, NULL);
		pthread_create(&keep_son_alive_thread_id, NULL, keep_son_alive_thread, NULL);
	}
	
	while (1){
		sleep(2);
		if(exit_server == 1) break;
	}
	pthread_join(keep_parent_alive_thread_id, NULL);
	pthread_join(keep_son_alive_thread_id, NULL);
	pthread_join(send_alive_thread_id, NULL);
	exit(0);
}

int createServer(int error_type){
	int f_ret;
	f_ret = fork();
	char * v[1] = {NULL};
	
	//Fork
	if (!f_ret){ // IF SON
		printf("[System Relauncher] Server created by Relauncher\n");
		if (error_type == BIND){
			printf("[System Relauncher] Waiting 30 seconds to reboot (bind error on server)\n");
			sleep(30);
		}
		return execve( "server", v , NULL); // Create Process
	}else //PARENT
		return f_ret;
}
