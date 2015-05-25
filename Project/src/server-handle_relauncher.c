#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <pthread.h>

#include "client-server.pb-c.h"

#include "server-handle_relauncher.h"
#include "server-handle_admin.h"
#include "server-relauncher.h"
#include "log.h"
#include "coms.h"

pthread_t keep_parent_alive_thread_id;
pthread_t keep_son_alive_thread_id;
pthread_t send_alive_thread_id;

pthread_t getRelauncherThread(int thread_number){
	switch(thread_number){
		case 0:
			return keep_parent_alive_thread_id;
		case 1:
			return keep_son_alive_thread_id;
		case 2:
			return send_alive_thread_id;
	}
	return (pthread_t) 0;
}

void * keep_parent_alive_thread(void *arg){
	int fd_fifo = openFIFO(1);
	int should_exit = 0, error = 0;
	proto_msg * message_from_relauncher;
	ALIVE *alive_message;
	
	
	while(! should_exit){
		sleep(2);
		message_from_relauncher = receiveMessageFIFO(fd_fifo);
		if (message_from_relauncher == NULL){
			printf("Relauncher not responding\n");
			if (error == 2) should_exit = 1;
			else error ++;
		}else{
			alive_message = alive__unpack(NULL, message_from_relauncher->msg_size, (const uint8_t *) message_from_relauncher->msg);
			if (alive_message == NULL){
				if (error == 2) should_exit = 1;
				else error ++;
			}else{
				if (alive_message->state == 0) error = 0;
			}
		}
		if (getExit() == 1) should_exit = 1;
	}
	
	if (getExit() != 1){
		pthread_create(&send_alive_thread_id, NULL, send_alive_thread, NULL);
		pthread_create(&keep_son_alive_thread_id, NULL, keep_son_alive_thread, NULL);
	}
	
	pthread_exit(NULL);
}

void * keep_son_alive_thread(void *arg){
	int should_exit = 0;
	int status;
	int pid_proc;
	
	createRelauncher();
	sleep(2);
	while ( !should_exit ){
		pid_proc = wait(&status);
		if(WIFSIGNALED(status)){
			printf("Relauncher (%d) returned with %d code. Rebooting Relauncher...\n", pid_proc, WEXITSTATUS(status));
			if (getExit() == 1){
				should_exit = 1;
			}else createRelauncher();
		}
	}
	
	pthread_exit(NULL);
}

void * send_alive_thread(void *arg){
	int fd_fifo = openFIFO(0);
	int should_exit = 0;
	proto_msg * message_to_relauncher;
	ALIVE alive_message;
	alive__init(&alive_message);
	alive_message.state = 0;
	
	while(! should_exit){
		sleep(2);
		if (getExit() == 1){
			alive_message.state = 1;
			should_exit = 1;
		}
		message_to_relauncher = protoCreateAlive(&alive_message);
		if (write(fd_fifo, message_to_relauncher->msg, message_to_relauncher->msg_size) == -1){
			perror("Write ");
			should_exit = 1;
		}
	}
	
	pthread_exit(NULL);
}

int handleRelauncher(){
	proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Initializing Relauncher Handler");
	addToLog(message_to_log);
	
	
	pthread_create(&keep_parent_alive_thread_id, NULL, keep_parent_alive_thread, NULL);
	
	message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Relauncher Handler Ready");
	addToLog(message_to_log);
	return 0;
}

int createRelauncher(){
	ALIVE alive_message;
	alive__init(&alive_message);
	alive_message.state = 0;
	proto_msg *message_to_relauncher = protoCreateAlive(&alive_message);
	
	int f_ret;
	f_ret = fork();
	char * v[3] = {"relauncher", message_to_relauncher->msg, NULL};
	
	//Fork
	if (!f_ret){ // IF SON
		printf("Relauncher created by Server\n");
		return execve( "relauncher", v , NULL); // Create Process
	}else //PARENT
		return f_ret;
}


