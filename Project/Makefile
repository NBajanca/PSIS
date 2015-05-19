CC = gcc

SRC_DIR =./src
EXE_DIR =./bin
OBJ_DIR =./obj

PROTO_FLAGS = -l protobuf-c

SERVER = server
CLIENT = client
PROTO = client-server.pb-c

all: $(SERVER) $(CLIENT)

$(SERVER): $(OBJ_DIR)/$(SERVER).o $(OBJ_DIR)/$(PROTO).o
	$(CC) $(OBJ_DIR)/$(SERVER).o $(OBJ_DIR)/$(PROTO).o -o $(EXE_DIR)/$(SERVER) $(PROTO_FLAGS)

$(OBJ_DIR)/$(SERVER).o: $(SRC_DIR)/$(SERVER).c $(SRC_DIR)/$(SERVER).h
	$(CC) -c $(SRC_DIR)/$(SERVER).c -o $(OBJ_DIR)/$(SERVER).o

$(CLIENT): $(OBJ_DIR)/$(CLIENT).o $(OBJ_DIR)/$(PROTO).o
	$(CC) $(OBJ_DIR)/$(CLIENT).o $(OBJ_DIR)/$(PROTO).o -o $(EXE_DIR)/$(CLIENT) $(PROTO_FLAGS)
	
$(OBJ_DIR)/$(CLIENT).o: $(SRC_DIR)/$(SERVER).c $(SRC_DIR)/$(SERVER).h
	$(CC) -c $(SRC_DIR)/$(CLIENT).c -o $(OBJ_DIR)/$(CLIENT).o

$(OBJ_DIR)/$(PROTO).o: $(SRC_DIR)/$(PROTO).c $(SRC_DIR)/$(PROTO).h 
	$(CC) -c $(SRC_DIR)/$(PROTO).c $(PROTO_FLAGS) -o $(OBJ_DIR)/$(PROTO).o

clean:
	$(RM) $(SERVER) $(CLIENT)
