#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "client-server.pb-c.h"

#include "client-server.h"
#include "client-msg-server.h"
#include "client.h"

#define LOGIN_STR "LOGIN"// username 
#define DISC_STR "DISC" //Disconnects from server
#define QUIT_STR "QUIT" //Exits the application
#define CHAT_STR "CHAT"// string
#define QUERY_STR "QUERY"// id_min id_max – request o

//Socket
int sock_fd;

//Server
int login_status;

int main(){
	//Variables
	//General
	int should_exit = 0;
	char line[CMD_SIZE];
	char command[CMD_SIZE];
	char cmd_str_arg[CMD_SIZE];
	int  cmd_int_arg1, cmd_int_arg2;
	
	//Program
	//Socket
	sock_fd = iniSocket();
	login_status = 0;
	
	
	while(! should_exit){
		
		fgets(line, CMD_SIZE, stdin);
		if(sscanf(line, "%s", command) == 1){
			if(strcmp(command, LOGIN_STR) == 0 && !(login_status)){
				if(sscanf(line, "%*s %s", cmd_str_arg) == 1){
					printf("Sending LOGIN command (%s)\n", cmd_str_arg);
					if (loginProtocol(cmd_str_arg) == -1){
						close(sock_fd);
						should_exit= 1;	
					}
				}
				else{
					printf("Invalid LOGIN command\n");
				}
				
			}else if(strcmp(command, QUIT_STR)==0){
				printf("Exiting the app\n");
				close(sock_fd);
				should_exit= 1;						
			}else if (login_status){
				if(strcmp(command, DISC_STR)==0){
					printf("Sending DISconnnect command\n");
					controlProtocol(2);
					close(sock_fd);
					login_status= 0;						
						
						
				}else if(strcmp(command, CHAT_STR)==0){
					if(sscanf(line, "%*s %s", cmd_str_arg) == 1){
						printf("Sending CHAT command (%s)\n", cmd_str_arg);
						if (chatProtocol(cmd_str_arg) == -1){
						close(sock_fd);
						should_exit= 1;	
						}
					
					}
					else{
						printf("Invalid CHAT command\n");
					}
				}else if(strcmp(command, QUERY_STR)==0){
					if(sscanf(line, "%*s %d %d", &cmd_int_arg1, &cmd_int_arg2) == 2){
						printf("Sending QUERY command (%d %d)\n", cmd_int_arg1, cmd_int_arg2);
						if (queryProtocol(cmd_int_arg1, cmd_int_arg2) == -1){
						close(sock_fd);
						should_exit= 1;	
						}
					
					}
					else{
						printf("Invalid QUERY command\n");
					}
				}else{
					printf("Invalid command\n");
				}
			}else{
					printf("Invalid command - LOGIN Required!\n");
			}
		}else{
			printf("Error in command\n");
		}
		
	}
	exit(0);
}


/* iniSocket
 * 
 * Initializes the socket connection
 * Connects to server on PORT 3000, IP 127.0.0.1
 * 
 * @ returns sock_fd - Socket descriptor
 * */
int iniSocket(){
	int sock_fd;
	struct sockaddr_in server_addr;
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	perror("socket ");
	if(sock_fd == -1){
		exit(-1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(3000);				/*Port*/
	inet_aton("127.0.0.1", & server_addr.sin_addr); /*IP*/	

	if( connect(sock_fd, ( struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		perror("connect ");
		exit(-1);
	}
	
	return sock_fd;
}

int getSock(){
	return sock_fd;
}

int getLoginStatus(){
	return login_status;
}

void setLoginStatus(int status){
	login_status = status;
	return;
}
