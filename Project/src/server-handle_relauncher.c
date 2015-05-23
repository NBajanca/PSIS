#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "server-handle_relauncher.h"
#include "log.h"

int handleRelauncher(){
	proto_msg * message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Initializing Relauncher Handler");
	addToLog(message_to_log);
	
	message_to_log = createProtoMSG( ALLOC_MSG );
	message_to_log->msg_size = sprintf(message_to_log->msg ,"Relauncher Handler Ready");
	addToLog(message_to_log);
	return 0;
}
