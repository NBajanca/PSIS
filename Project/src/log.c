#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#define STD_SIZE 100

char * getTime(){
	char *time_string;
	time_string = (char *) malloc( STD_SIZE * sizeof(char));

	time_t t = time(NULL);
	struct tm * p = localtime(&t);

	strftime(time_string, STD_SIZE, "%F, %T", p);
	
	return time_string;
}
