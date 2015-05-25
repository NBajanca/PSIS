#ifndef HEADER_LOG
#define HEADER_LOG

#include "coms.h"

#define SERVER_TYPE 0
#define MESSAGE_TYPE 1

void iniLog();
void destroyLog();

char * getTime();
void addToLog(proto_msg * message_to_log, int type);

proto_msg *getLog();

#endif
