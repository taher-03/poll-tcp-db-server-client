#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <poll.h>
#include "common.h"
#include "file.h"
#include "parse.h"
#include "srvpoll.h"
#include <signal.h>
#include <errno.h>
int shutdown_pipe[2];
void handle_signal(int sig)
{
  (void)sig;
  write(shutdown_pipe[1],"x",1);
}
#define BACKLOG 256

client CLIENTS[256];
void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <database file>\n",argv[0]);
  printf("\t -n - create new database file\n");
  printf("\t -f - required path ro database file\n");
}
void poll_loop(int dbfd,unsigned short port,struct dbheader_t **dbhdr,struct employee_t **employees)
{
  init_clients(CLIENTS);
  int nfds=2;
  struct pollfd fds[258];
  memset(&fds,'\0',sizeof(fds));
  struct sockaddr_in mysocket;
  memset(&mysocket,0,sizeof(struct sockaddr_in));
	 mysocket.sin_family=AF_INET;
	 mysocket.sin_port=htons(port);
	 mysocket.sin_addr.s_addr=inet_addr("127.0.0.1");
	 int sockfd,new_fd;
	 if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1)
	   {
	     perror("socket");
	     exit(EXIT_FAILURE);
	   }
	 int yes=1;
	 setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes);
	 if(bind(sockfd,(struct sockaddr *)&mysocket,sizeof(struct sockaddr_in))==-1)
	   {
	     perror("bind");
	     exit(EXIT_FAILURE);
	   }
	 if(listen(sockfd,BACKLOG)==-1)
	   {
	     perror("listen");
	     exit(EXIT_FAILURE);
	   }
	 fds[0].fd=sockfd;
	 fds[0].events=POLLIN;
	 fds[1].fd= shutdown_pipe[0];
	 fds[1].events=POLLIN;
	 
	 while(1)
	   {
	     int ii=2;
	     for(int i=0;i<256;++i)
	       {
		 if(CLIENTS[i].fd!=-1)
		   {
		     fds[ii].fd=CLIENTS[i].fd;
		     fds[ii].events=POLLIN;
		     ++ii;
		   }
	       }
	     int n_events= poll(fds,nfds,-1);
	     if(n_events==-1)
	       {
		 if(errno == EINTR)
		   {
		     continue;
		   }
		 perror("no events");
		 exit(EXIT_FAILURE);
	       }
	     if(n_events==0)
	       {
		 printf("server is writing files there is no user connected\n");
		 return;
	       }
	     if(fds[1].revents & POLLIN)
	       {
		 char buf;
		 read(shutdown_pipe[0],&buf,1);
		 printf("Shutdown Requested\n");
		 close(sockfd);
		 for(int i=0;i<256;++i)
		   {
		     if(CLIENTS[i].fd !=-1)
		       {
			 close(CLIENTS[i].fd);
			 CLIENTS[i].fd=-1;
		       }
		   }
		 break;
	       }
	     if(fds[0].revents& POLLIN)
	       {
		 if((new_fd=accept(sockfd,NULL,NULL))==-1)
		   {
		     perror("accept");
		     continue;
		   }
		     int slot=freeSlot(CLIENTS);
		     if(slot==-1)
		       {
			 printf("server full");
			 close(new_fd);
		       }
		     else
		       {
			 CLIENTS[slot].fd=new_fd;
			 CLIENTS[slot].state=STATE_CONNECTED;
			 nfds++;
			 printf("Slot %d has fd %d\n",slot,CLIENTS[slot].fd);
			   
		       }
		     n_events--;
	       }
	     for(int i=2;i<=nfds&&n_events>0;++i)
	       {
		 if(fds[i].revents&POLLIN)
		   {
		     n_events--;
		     int fd=fds[i].fd;
		     int client_slot=find_fd(CLIENTS,fd);
		     if(client_slot==-1)
		       {
			 printf("client does not exist\n");
		       }
		     ssize_t bytes_read=read(fd,CLIENTS[client_slot].buffer,sizeof(CLIENTS[client_slot].buffer));
		     if(bytes_read<=0)
		       {
			 printf("Client %d has disconnected or error",client_slot);
			 close(fd);
			 CLIENTS[client_slot].fd=-1;
			 memset(CLIENTS[client_slot].buffer,'\0',sizeof(CLIENTS[client_slot].buffer));
			 nfds--;
		       }
		     else
		       {
			 if(CLIENTS[client_slot].state==STATE_CONNECTED||CLIENTS[client_slot].state!=STATE_MSG)
			   {
			     CLIENTS[client_slot].state=STATE_HELLO;
			   }
			 handle_client_fsm(dbfd,dbhdr,employees,&CLIENTS[client_slot]);
			 
		       }
		   }
	       }
	   }
}
int main(int argc, char *argv[]) {
  if(signal(SIGINT, handle_signal)==SIG_ERR)
    {
      perror("signal");
      return 1;
    }
  char *filepath =NULL;
  char *addstring = NULL;
  char *remove_emp= NULL;
  char *update_emp= NULL;
  char *portarg=NULL;
  unsigned short port=0;
  int c;
  bool newfile=false;
  int dbfd=-1;
  bool list_emp=false;
  struct dbheader_t *dbhd=NULL;
  struct employee_t *employees = NULL;
  while((c= getopt(argc,argv,"nf:a:lr:u:p:"))!=-1)
    {
      switch(c) {
      case 'n':
	newfile=true;
	break;
      case 'f':
	filepath = optarg;
	break;
      case 'a':
	addstring = optarg;
	break;
      case 'l':
	list_emp=true;
	break;
      case 'r':
	remove_emp=optarg;
	break;
      case 'u':
	update_emp=optarg;
	break;
      case 'p':
	portarg=optarg;
	port=atoi(portarg);
	if(port==0)
	  {
	    printf("bad port");
	  }
	break;
      case '?':
	printf("unknown usage\n");
	break;
      default:
	  return -1;
      }
    }
  if(filepath==NULL)
    {
      printf("Filepath is a required argument\n");
      print_usage(argv);
      return 0;
    }
  if(newfile)
    {
      dbfd = create_db_file(filepath);
      if (dbfd == STATUS_ERROR)
	{
	  printf("unable to create database file\n");
	  return -1;
	}
      if(create_db_header(dbfd,&dbhd)==STATUS_ERROR)
	{
	  printf("Failed to create database header\n");
	  return -1; 
	}
    }
  else
    {
      dbfd = open_db_file(filepath);
      if(dbfd== STATUS_ERROR)
	{
	  printf("unable to open database file\n");
	  return -1;
	}
      if(validate_db_header(dbfd,&dbhd) == STATUS_ERROR)
	{
	  printf("Database not valid! \n");
	  return STATUS_ERROR;
	}
    }
  if(read_employees(dbfd,dbhd,&employees)!=STATUS_SUCCESS)
    {
      printf("Failed to read employees\n");
      return STATUS_ERROR;
    }
  if(addstring)
    {
      add_employee(&dbhd,&employees,addstring);
    }
  if(remove_emp)
    {
      remove_employee(&dbhd,&employees,remove_emp);
    }
  if(update_emp)
    {
      update_employee(dbhd,employees,update_emp);
    }
  if(list_emp)
    {
      list_employees(dbhd,employees);
    }
  if(pipe(shutdown_pipe)==-1)
    {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
  poll_loop(dbfd,port,&dbhd,&employees);
  output_file(dbfd,dbhd,employees);
  free(dbhd);
  free(employees);
  printf("Newfile: %d\n", newfile);
  printf("Filepath: %s\n", filepath);
  return 0;
}

