#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

/*Com fifos. este .c tem de falar com o server de alguma forma. por isso enviei-tre um server.c que falar com este para poderes tirar de lá cenas*/
void *write_thread(void *arg){
	int value = *((int *)arg);
	int fd_fifo, fd_write;
	int a=1;
	
	/*Abre a fifo*/
	fd_fifo = open("/tmp/fifo_homicida", O_WRONLY);
	if(fd_fifo == -1){
		perror("open");
		exit(-1);
	}
	printf("Opened fifo\n");
	
	while(1){
		sleep(value);
		fd_write = write(fd_fifo, &a, sizeof(a));
		printf("Wrote %d to fifo\n", a); fflush(stdout);
		a++;
		if(fd_write  == -1){
			perror("write");
			exit(-1);
		}
	}
	
	pthread_exit(NULL);
}

void *relauncher(void *arg){
	int status;
	char *v[1] = {NULL};
	pthread_t thread_id_2;
	pid_t pid, pid_proc=-1;
	
	pid = fork();
	
	if(pid == 0){
		printf("\nExecuting child's process (%d) whose parent is (%d)\n", getpid(), getppid()); fflush(stdout);
		execve("./server", &v[0], NULL);
	}else{
		while(1){
			pid_proc = wait(&status);
			if(pid_proc != -1){
				if(WIFEXITED(status)){
					printf("Father: My child (%d) returned with (%d) code\n", pid_proc, WEXITSTATUS(status)); fflush(stdout);
				}else if(WIFSIGNALED(status)){
					printf("Father: My child (%d) was killed with (%d) code\n", pid_proc, WTERMSIG(status)); fflush(stdout);
				}
				printf("Estou aqui"); fflush(stdout);
				pid = fork();
				if(pid == 0) {
					printf("\nExecuting child's process (%d) whose parent is (%d)\n", getpid(), getppid()); fflush(stdout);
					execve("./server", &v[0], NULL);
				}
			}	
		}
	}
	 
	pthread_exit(NULL);
}


int main(){
 	pthread_t thread_id_1;
	pthread_t thread_id_2;
	void * res;
	int sleep_time = 3;

	/*Cria a fifo*/
	int fifo = mkfifo("/tmp/fifo_homicide", 0600);
	if(fifo != 0 ){
		perror("mkfifo");
	}
	printf("Created fifo (%d)\n", fifo);

	/*Cria threads*/
	pthread_create(&thread_id_1, NULL, write_thread, &sleep_time);
	printf("New thread %d\n", (int)thread_id_1);
	
	pthread_create(&thread_id_2, NULL, relauncher, NULL);
	printf("New thread %d\n", (int)thread_id_2);

	pthread_join(thread_id_1, &res);
	pthread_join(thread_id_2, &res);
	
	exit(0);
}
