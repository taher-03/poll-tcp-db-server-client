# Poll-based TCP Employee Database (C)

This project implements a simple TCP **client-server employee database** in C using `poll()` for I/O multiplexing.  
The server supports multiple clients, and the client can perform add, update, remove, and list operations. The server saves the database on shutdown.

---

# Project Structure
├── src/

│ ├── srv/ # Server source files

│ └── cli/ # Client source files

├── obj/ # Object files (created during build)

├── bin/ # Compiled server and client binaries

├── include/ # Header files

├── Makefile

└── README.md

# Building
Make sure you have `gcc` installed. Then simply run:
```bash
make
```
# Running the server
```bash
./bin/dbserver -p <port> -f <database file>
```
if you want to use a new database file add the tag -n after <database file>

To gracefully shut down the server and save the database, press Ctrl-C. The servers handle the shutdown using shutdown_pipe.


# Running the client
```bash
./bin/dbcli -p <port> -h <server_ip> [options]
```
the server is set on localhost meaning 127.0.0.1 in port 5555, you can change this manually in the program with whatever IP you want.


# [options]
Add an Employee: 
```bash
./bin/dbcli -p 5555 -h 127.0.0.1 -a "name,address,age"
```
Update an employee:
```bash
./bin/dbcli -p 5555 -h 127.0.0.1 -u "current_name,new_address,new_age"
```
Remove an employee:
```bash
./bin/dbcli -p 5555 -h 127.0.0.1 -r "name,address,not important but int would be better or simply 0"
```
List all employees:
```bash
./bin/dbcli -p 5555 -h 127.0.0.1 -l
```

# Notes

Server uses poll() for efficient multi-client handling.

Database is automatically saved when the server is shut down with Ctrl-C.

Maximum supported clients: 256. (don't change it or you have to change max fds and also client memory allocation, and lots of edits in poll_loop).



