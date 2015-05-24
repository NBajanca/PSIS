#ifndef HEADER_SERVER_HANDLE_RELAUNCHER
#define HEADER_SERVER_HANDLE_RELAUNCHER

int handleRelauncher();
void * keep_parent_alive_thread(void *arg);
void * keep_son_alive_thread(void *arg);
void * send_alive_thread(void *arg);
int createRelauncher();

#endif
