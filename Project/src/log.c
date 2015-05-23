#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include "log.h"

pthread_mutex_t log_mutex;

void iniLog(){
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

void addToLog(proto_msg * message_to_log){
	FILE *log_file;
	
	log_file = fopen("log.txt", "a");
	if (log_file == NULL) {
	  perror ("Fopen (LOG) ");
	  exit(-1);
	}
	
	char *time = getTime();
	pthread_mutex_lock (&log_mutex);
	fprintf(log_file, "(%s) - %s\n", time , message_to_log->msg);
	pthread_mutex_unlock (&log_mutex);
	printf("(%s) - %s\n", time , message_to_log->msg);
	free(time);
	
	fclose(log_file);
	destroyProtoMSG(message_to_log);
	return;
}
