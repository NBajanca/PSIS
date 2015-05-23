#ifndef HEADER_LOG
#define HEADER_LOG

#include "coms.h"

void iniLog();
void destroyLog();

char * getTime();
void addToLog(proto_msg * message_to_log);

#endif
