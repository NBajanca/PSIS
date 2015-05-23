#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "server-handle_admin.h"
#include "client-server.pb-c.h"
#include "coms.h"
#include "server-admin.h"



int handleAdmin(){
	
	printf("Initializing Admin Handler\n");
	int sock_fd = iniSocket();
	printf("Admin Handler Ready\n");
	
	while(1){
		if (handleNewAdmin (sock_fd) != 0) break;
	}
	close(sock_fd);
	return 0;
}

int handleNewAdmin (int sock_fd){
	int action = 0; 
	
	//Acept New Admin
	int new_sock_fd = accept(sock_fd, NULL, NULL);
	if(new_sock_fd == -1){
		perror("Accept (Admin) ");
		exit(-1);
	}
	printf("Admin LogIn\n");
	
	//Handle Admin Requests
	while(!action){
		action = handleAdminRequests(new_sock_fd);
	}
	
	//Close Admin Connection
	printf("Admin LogOut\n");
	close(new_sock_fd);
	
	if (action == 1) return 1;
	return 0;
}

int handleAdminRequests(int sock_fd){
	proto_msg * message = receiveMessage(sock_fd);
	if (message == NULL) return -1;
	
	//Unmarshal incoming message
	ADMIN *admin = admin__unpack(NULL, message->msg_size, message->msg);
	destroyProtoMSG(message);
	
	switch (admin->action){
		case 0:
			printf("Received LOG request from Admin\n");
			sendLog(sock_fd);
			break;
		case 1:
			printf("Received QUIT request from Admin\n");
			return 1;
		case 2:
			printf("Received DISC request from Admin\n");
			return 2;
			break;
	}
	return 0;
}

int sendLog(int sock_fd){
	printf("Sending LOG to Admin\n");
	
	return 0;
}

int iniSocket(){
	struct sockaddr_in addr, client_addr;
	int addr_len; 
	int sock_fd;
	
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1){
		perror("Socket (Admin) ");
		exit(-1);
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3001);		/*Port*/
    addr.sin_addr.s_addr = INADDR_ANY;	/*IP*/

	if( bind(sock_fd, (struct sockaddr *)  &addr, sizeof(addr)) == -1){
		perror("Bind (Admin) ");
		exit(-1);
	}
	
	if( listen(sock_fd, 1) == -1){
		perror("Listen (Admin) ");
		
	}
	
	return sock_fd;
}
