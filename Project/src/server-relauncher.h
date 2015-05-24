#ifndef HEADER_SERVER_RELAUNCHER
#define HEADER_SERVER_RELAUNCHER

#include "coms.h"

int openFIFO(int type);
proto_msg * receiveMessageFIFO(int sock_fd);
proto_msg* protoCreateAlive(ALIVE * alive);

#endif
