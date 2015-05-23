CC = gcc

SRC_DIR =./src
EXE_DIR =./bin
OBJ_DIR =./obj

PROTO_FLAGS = -l protobuf-c
THREAD_FLAGS = -lpthread

PROTO = client-server.pb-c

all: $(EXE_DIR)/server $(EXE_DIR)/admin $(EXE_DIR)/client

#Admin Executable
$(EXE_DIR)/admin: $(OBJ_DIR)/admin.o $(OBJ_DIR)/$(PROTO).o $(OBJ_DIR)/coms.o $(OBJ_DIR)/server-admin.o
	$(CC) $(OBJ_DIR)/admin.o $(OBJ_DIR)/$(PROTO).o $(OBJ_DIR)/coms.o $(OBJ_DIR)/server-admin.o -o $(EXE_DIR)/admin  $(PROTO_FLAGS)

#Admin Object
$(OBJ_DIR)/admin.o: $(SRC_DIR)/admin.c $(SRC_DIR)/$(PROTO).h $(SRC_DIR)/coms.h $(SRC_DIR)/server-admin.h
	$(CC) -c $(SRC_DIR)/admin.c -o $(OBJ_DIR)/admin.o $(PROTO_FLAGS)
	
#Client Executable
$(EXE_DIR)/client: $(OBJ_DIR)/client.o $(OBJ_DIR)/$(PROTO).o $(OBJ_DIR)/coms.o $(OBJ_DIR)/client-server.o
	$(CC) $(OBJ_DIR)/client.o $(OBJ_DIR)/$(PROTO).o $(OBJ_DIR)/coms.o $(OBJ_DIR)/client-server.o -o $(EXE_DIR)/client  $(PROTO_FLAGS)

#Client Object
$(OBJ_DIR)/client.o: $(SRC_DIR)/client.c $(SRC_DIR)/$(PROTO).h $(SRC_DIR)/coms.h $(SRC_DIR)/client-server.h
	$(CC) -c $(SRC_DIR)/client.c -o $(OBJ_DIR)/client.o $(PROTO_FLAGS)

#Server Executable
$(EXE_DIR)/server: $(OBJ_DIR)/server.o $(OBJ_DIR)/$(PROTO).o $(OBJ_DIR)/server-handle_client.o $(OBJ_DIR)/server-handle_admin.o $(OBJ_DIR)/server-handle_relauncher.o $(OBJ_DIR)/coms.o $(OBJ_DIR)/server-admin.o $(OBJ_DIR)/log.o $(OBJ_DIR)/client-db.o $(OBJ_DIR)/client-server.o
	$(CC) $(OBJ_DIR)/server.o $(OBJ_DIR)/$(PROTO).o $(OBJ_DIR)/server-handle_client.o $(OBJ_DIR)/server-handle_admin.o $(OBJ_DIR)/server-handle_relauncher.o $(OBJ_DIR)/coms.o $(OBJ_DIR)/server-admin.o $(OBJ_DIR)/log.o $(OBJ_DIR)/client-db.o $(OBJ_DIR)/client-server.o -o $(EXE_DIR)/server $(PROTO_FLAGS) $(THREAD_FLAGS)

#Server Object
$(OBJ_DIR)/server.o: $(SRC_DIR)/server.c $(SRC_DIR)/server-handle_client.h $(SRC_DIR)/server-handle_admin.h $(SRC_DIR)/server-handle_relauncher.h
	$(CC) -c $(SRC_DIR)/server.c -o $(OBJ_DIR)/server.o
	
#Server-handle_Client Object
$(OBJ_DIR)/server-handle_client.o: $(SRC_DIR)/server-handle_client.c $(SRC_DIR)/server-handle_client.h $(SRC_DIR)/coms.h $(SRC_DIR)/$(PROTO).h $(SRC_DIR)/client-db.h $(SRC_DIR)/client-server.h
	$(CC) -c $(SRC_DIR)/server-handle_client.c -o $(OBJ_DIR)/server-handle_client.o $(PROTO_FLAGS) $(THREAD_FLAGS)

#Server-handle_admin Object
$(OBJ_DIR)/server-handle_admin.o: $(SRC_DIR)/server-handle_admin.c $(SRC_DIR)/server-handle_admin.h $(SRC_DIR)/$(PROTO).h $(SRC_DIR)/server-admin.h $(SRC_DIR)/coms.h $(SRC_DIR)/log.h
	$(CC) -c $(SRC_DIR)/server-handle_admin.c -o $(OBJ_DIR)/server-handle_admin.o $(PROTO_FLAGS)
	
#Server-Admin Object
$(OBJ_DIR)/server-admin.o: $(SRC_DIR)/server-admin.c $(SRC_DIR)/server-admin.h $(SRC_DIR)/$(PROTO).h
	$(CC) -c $(SRC_DIR)/server-admin.c -o $(OBJ_DIR)/server-admin.o $(PROTO_FLAGS)

#Server-handle_relauncher Object
$(OBJ_DIR)/server-handle_relauncher.o: $(SRC_DIR)/server-handle_relauncher.c $(SRC_DIR)/server-handle_relauncher.h $(SRC_DIR)/$(PROTO).h $(SRC_DIR)/log.h
	$(CC) -c $(SRC_DIR)/server-handle_relauncher.c -o $(OBJ_DIR)/server-handle_relauncher.o $(PROTO_FLAGS)

#Client-Server Object
$(OBJ_DIR)/client-server.o: $(SRC_DIR)/client-server.c $(SRC_DIR)/client-server.h $(SRC_DIR)/coms.h $(SRC_DIR)/$(PROTO).h
	$(CC) -c $(SRC_DIR)/client-server.c -o $(OBJ_DIR)/client-server.o $(PROTO_FLAGS)

#Client-DB Object
$(OBJ_DIR)/client-db.o: $(SRC_DIR)/client-db.c $(SRC_DIR)/client-db.h $(SRC_DIR)/log.h
	$(CC) -c $(SRC_DIR)/client-db.c -o $(OBJ_DIR)/client-db.o
	
#Log Object
$(OBJ_DIR)/log.o: $(SRC_DIR)/log.c $(SRC_DIR)/log.h
	$(CC) -c $(SRC_DIR)/log.c -o $(OBJ_DIR)/log.o
	
#Coms Object
$(OBJ_DIR)/coms.o: $(SRC_DIR)/coms.c $(SRC_DIR)/coms.h
	$(CC) -c $(SRC_DIR)/coms.c -o $(OBJ_DIR)/coms.o

#Proto Object
$(OBJ_DIR)/$(PROTO).o: $(SRC_DIR)/$(PROTO).c $(SRC_DIR)/$(PROTO).h
	$(CC) -c $(SRC_DIR)/$(PROTO).c -o $(OBJ_DIR)/$(PROTO).o $(PROTO_FLAGS)

clean:
	$(RM) $(EXE_DIR)/server $(EXE_DIR)/admin $(EXE_DIR)/client
