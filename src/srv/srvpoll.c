#include <poll.h>
#include "common.h"
#include "srvpoll.h"
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
void handle_client_fsm(int dbfd,struct dbheader_t **dbhdr,struct employee_t **employees,client *CLIENTS)
{
  
  dbproto_hdr_t *hdr=(dbproto_hdr_t*)CLIENTS->buffer;
  hdr->type=ntohl(hdr->type);
  hdr->len=ntohs(hdr->len);
  if(CLIENTS->state==STATE_HELLO)
    {
      if(hdr->type!=MSG_HELLO_REQ || hdr->len!= 1)
	{
	  printf("did'nt get the hello msg");
	}
      dbproto_hello_req *hello=(dbproto_hello_req*)&hdr[1];
      hello->proto=ntohs(hello->proto);
      if(hello->proto!=PROTO_VR)
	{
	  printf("protocol version mismatch!");
	  hdr->type=MSG_ERROR;
	  hdr->len=0;
	  hdr->type=htonl(hdr->type);
	  hdr->len=htons(hdr->len);
	  write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
	  return;
	}
      hdr->type=MSG_HELLO_RESP;
      hdr->len=1;
      hdr->type=htonl(hdr->type);
      hdr->len=htons(hdr->len);
      CLIENTS->state=STATE_MSG;
      write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
      printf("client upgraded to STATE_MSG\n");
    }
  if(CLIENTS->state==STATE_MSG)
    {
      if(hdr->type == MSG_EMPLOYEE_LIST_REQ)
	{
	  hdr->type=MSG_EMPLOYEE_LIST_RESP;
	  hdr->len=(*dbhdr)->count;
	  hdr->type=htonl(hdr->type);
	  hdr->len=htons(hdr->len);
	  write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
	  printf("sending employees to the client\n");
	  for(int i=0;i<(*dbhdr)->count;++i)
	    {
	      struct employee_t temp;
	      temp=(*employees)[i];
	      temp.hours=htonl(temp.hours);
	      write(CLIENTS->fd,(char*)&temp,sizeof(struct employee_t));
	    }
		return;
	}
      if(hdr->type==MSG_EMPLOYEE_UPDATE_REQ)
	{
	  dbproto_employee_add_req *update_string=(dbproto_hello_req*)&hdr[1];
	  char temp[sizeof(update_string->data)];
	  strncpy(temp,update_string->data,sizeof(update_string->data));
	  char *name = strtok(temp,",");
	  char *addr = strtok(NULL,",");
	  char *hours = strtok(NULL,",");
	  if(name!=NULL && addr!=NULL && hours!=NULL)
	    {
	      if(update_employee(*dbhdr,*employees,update_string->data)==STATUS_SUCCESS)
		{
		  hdr->type=MSG_EMPLOYEE_UPDATE_RESP;
		  hdr->len=1;
		  hdr->type=htonl(hdr->type);
		  hdr->len=htons(hdr->len);
		  write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
		  printf("user updated successfully\n");
		  return;
		}
	      else
		{
		  hdr->type=MSG_ERROR;
		  hdr->len=0;
		  hdr->type=htonl(hdr->type);
		  hdr->len=htons(hdr->len);
		  write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
		  return;
		}
	    }
	  else
	    {
	      hdr->type=MSG_ERROR;
	      hdr->len=0;
	      hdr->type=htonl(hdr->type);
	      hdr->len=htons(hdr->len);
	      write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
	      return;
	    }
	}
      if(hdr->type==MSG_EMPLOYEE_DEL_REQ)
	{
	  dbproto_employee_add_req *remove_string=(dbproto_hello_req*)&hdr[1];
	  char temp[sizeof(remove_string->data)];
	  strncpy(temp,remove_string->data,sizeof(remove_string->data));
	  char *name = strtok(temp,",");
	  char *addr = strtok(NULL,",");
	  char *hours = strtok(NULL,",");
	  if(name!=NULL && addr!=NULL && hours!=NULL)
	    {
	      if(remove_employee(dbhdr,employees,remove_string->data)==STATUS_SUCCESS)
		{
		  hdr->type=MSG_EMPLOYEE_DEL_RESP;
		  hdr->len=1;
		  hdr->type=htonl(hdr->type);
		  hdr->len=htons(hdr->len);
		  write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
		  printf("user updated successfully\n");
		  return;
		}
	      else
		{
		  hdr->type=MSG_ERROR;
		  hdr->len=0;
		  hdr->type=htonl(hdr->type);
		  hdr->len=htons(hdr->len);
		  write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
		  return;
		}
	    }
	  else
	    {
	      hdr->type=MSG_ERROR;
	      hdr->len=0;
	      hdr->type=htonl(hdr->type);
	      hdr->len=htons(hdr->len);
	      write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
	      return;
	    }
	}
      if(hdr->type==MSG_EMPLOYEE_ADD_REQ)
	{
	  dbproto_employee_add_req *addstring=(dbproto_hello_req*)&hdr[1];
	  char temp[sizeof(addstring->data)];
	  strncpy(temp,addstring->data,sizeof(addstring->data));
	  char *name = strtok(temp,",");
	  char *addr = strtok(NULL,",");
	  char *hours = strtok(NULL,",");
	  if(name!=NULL && addr!=NULL && hours!=NULL)
	    {
	      if(add_employee(dbhdr,employees,addstring->data)==STATUS_SUCCESS)
		{
		  hdr->type=MSG_EMPLOYEE_ADD_RESP;
		  hdr->len=1;
		  hdr->type=htonl(hdr->type);
		  hdr->len=htons(hdr->len);
		  write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
		  printf("user added successfully\n");
		  return;
		}
	      else
		{
		  hdr->type=MSG_ERROR;
		  hdr->len=0;
		  hdr->type=htonl(hdr->type);
		  hdr->len=htons(hdr->len);
		  write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
		  return;
		}
	    }
	  else
	    {
	      hdr->type=MSG_ERROR;
	      hdr->len=0;
	      hdr->type=htonl(hdr->type);
	      hdr->len=htons(hdr->len);
	      write(CLIENTS->fd,hdr,sizeof(dbproto_hdr_t));
	      return;
	    }
	}
      
    }
}
void init_clients(client *CLIENTS)
{
  for (int i=0;i<256;++i)
    {
      memset(CLIENTS[i].buffer,'\0',4096);
      CLIENTS[i].fd=-1;
    }
}
int freeSlot(client *CLIENTS)
{
  for (int i=0;i<256;++i)
    {
      if(CLIENTS[i].fd==-1)
	{
	  return i;
	}
    }
  return -1;
}
int find_fd(client *CLIENTS,int fd)
{
  for(int i=0;i<256;++i)
    {
      if(CLIENTS[i].fd==fd)
	{
	  return i;
	}
    }
  return -1;
}
