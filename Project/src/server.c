#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "log.h"
#include "server-handle_client.h"
#include "server-handle_admin.h"
#include "server-handle_relauncher.h"


int main ( int arc, char **argv ) {
	iniLog();
	handleRelauncher();	//Creates Thread(s)
	handleClient(); //Creates Thread(s)
	handleAdmin(); //Blocks until order to quit

	destroyLog();
	exit(0);
}
