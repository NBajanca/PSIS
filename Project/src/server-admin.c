#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "client-server.pb-c.h"
#include "coms.h"
#include "server-admin.h"


proto_msg* protoCreateAdmin(ADMIN * admin_msg){
	proto_msg * proto_message = createProtoMSG(DONT_ALLOC_MSG);
	
	proto_message->msg_size = admin__get_packed_size(admin_msg);
	proto_message->msg = malloc(proto_message->msg_size);
	admin__pack(admin_msg, proto_message->msg);
	
	return proto_message;
}
