TARGET_SRV = bin/dbserver
TARGET_CLI = bin/dbcli

SRC_SRV = $(wildcard src/srv/*.c)
OBJ_SRV = $(SRC_SRV:src/srv/%.c=obj/srv/%.o)

SRC_CLI = $(wildcard src/cli/*.c)
OBJ_CLI = $(SRC_CLI:src/cli/%.c=obj/cli/%.o)

run: clean default
	./$(TARGET_SRV) -f ./mydatabase.db -n -p 5555
default: $(TARGET_SRV) $(TARGET_CLI)

clean:
	rm -f obj/srv/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET_SRV): $(OBJ_SRV)
	gcc -o $@ $?
$(OBJ_SRV): obj/srv/%.o: src/srv/%.c
	gcc -g -c $< -o $@ -Iinclude
$(TARGET_CLI): $(OBJ_CLI)
	gcc -g -o $@ $?
$(OBJ_CLI): obj/cli/%.o: src/cli/%.c
	gcc -g -c $< -o $@ -Iinclude
