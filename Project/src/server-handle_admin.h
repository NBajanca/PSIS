#ifndef HEADER_SERVER_HANDLE_ADMIN
#define HEADER_SERVER_HANDLE_ADMIN

int handleAdmin();
int handleNewAdmin (int sock_fd);
int handleAdminRequests(int sock_fd);

int sendLog(int sock_fd);

int iniSocket();
int getExit();

#endif

