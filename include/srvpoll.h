#ifndef SRVPOLL_H
#define SRVPOLL_H
#include <poll.h>
#include <parse.h>
#define MAX_CLIENTS 256
#define PORT 5555
#define BUFF_SIZE 4096
typedef enum {
  STATE_NEW,
  STATE_CONNECTED,
  STATE_DISCONNECTED,
  STATE_HELLO,
  STATE_MSG,
  STATE_GOODBYT,
} state_e;

typedef struct {
  char buffer[4096];
  int fd;
  state_e state;
} client;

void init_clients(client *CLIENTS);

int freeSlot(client *CLIENTS);

int find_fd(client *CLIENTS,int);
void handle_client_fsm(int dbfd,struct dbheader_t **dbhdr,struct employee_t **employees,client* CLIENTS);
#endif
