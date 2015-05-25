#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <time.h>
#include <pthread.h>

#include "log.h"

pthread_mutex_t log_mutex;
int counter;

void iniLog(){
	counter = 0;
	pthread_mutex_init(&log_mutex, NULL);
	return;
}


void destroyLog(){
	pthread_mutex_destroy(&log_mutex);
	return;
}

char * getTime(){
	char *time_string;
	time_string = (char *) malloc( STD_SIZE * sizeof(char));

	time_t t = time(NULL);
	struct tm * p = localtime(&t);

	strftime(time_string, STD_SIZE, "%F, %T", p);
	
	return time_string;
}

void addToLog(proto_msg * message_to_log, int type){
	FILE *log_file;
	
	log_file = fopen("log.txt", "a");
	if (log_file == NULL) {
		perror ("Open (LogFile) ");
		exit(-1);
	}
	
	char *time = getTime();
	pthread_mutex_lock (&log_mutex);
	if (type == SERVER_TYPE)
		fprintf(log_file, "(%s) - %s\n", time , message_to_log->msg);
	else
		fprintf(log_file, "(%s) (%d) - %s\n", time , ++counter , message_to_log->msg);
	pthread_mutex_unlock (&log_mutex);
	if (type == SERVER_TYPE)
		printf("(%s) - %s\n", time , message_to_log->msg);
	else
		printf("(%s) (%d) - %s\n", time , counter , message_to_log->msg);	
	free(time);
	
	fclose(log_file);
	destroyProtoMSG(message_to_log);
	return;
}

proto_msg *getLog(){
	proto_msg * message = createProtoMSG(DONT_ALLOC_MSG);
	
	FILE *log_file = fopen("log.txt", "rb");
	
	pthread_mutex_lock (&log_mutex);
	fseek(log_file, 0, SEEK_END);
	message->msg_size = ftell(log_file);
	rewind(log_file);
	
	message->msg = malloc(message->msg_size * (sizeof(char)));
	if( message->msg == NULL){
		pthread_mutex_unlock (&log_mutex);
		perror("[System Error] Malloc (getLog) ");
		return NULL;
	}
	fread(message->msg, sizeof(char), message->msg_size, log_file);
	pthread_mutex_unlock (&log_mutex);
	
	fclose(log_file);
	
	return message;
}
