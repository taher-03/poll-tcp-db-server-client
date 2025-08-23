#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parse.h"
#include "common.h"
#include <unistd.h>
#include <string.h>
int update_employee_cli(int fd,char *update_emp)
{
  char buf[4096]={0};
  dbproto_hdr_t *hdr=buf;
  hdr->type = MSG_EMPLOYEE_UPDATE_REQ;
  hdr->len=1;
  dbproto_employee_add_req *packed_data=(dbproto_hello_req*)&hdr[1];
  strncpy(&packed_data->data,update_emp,sizeof(packed_data->data));
  hdr->type=htonl(hdr->type);
  hdr->len=htons(hdr->len);
  write(fd,buf,sizeof(dbproto_hdr_t)+sizeof(dbproto_employee_add_req));
  read(fd,buf,sizeof(buf));
  hdr->type=ntohl(hdr->type);
  hdr->len=ntohs(hdr->len);
  if(hdr->type==MSG_ERROR)
    {
      printf("updating user failed\n");
      close(fd);
      return STATUS_ERROR;
    }
  if(hdr->type==MSG_EMPLOYEE_UPDATE_RESP)
    {
      printf("user updated successfully\n");
      return STATUS_SUCCESS;
    }
  return STATUS_ERROR;
}
int list_client(int fd)
{
  char buf[4096]={0};
  dbproto_hdr_t *hdr=buf;
  hdr->type = MSG_EMPLOYEE_LIST_REQ;
  hdr->len=0;
  hdr->type=htonl(hdr->type);
  hdr->len=htons(hdr->len);
  write(fd,buf,sizeof(dbproto_hdr_t));
  read(fd,hdr,sizeof(dbproto_hdr_t));
  hdr->type=ntohl(hdr->type);
  hdr->len=ntohs(hdr->len);
  if(hdr->type==MSG_ERROR)
    {
      printf("failed to list employees");
      close(fd);
      return STATUS_ERROR;
    }
  if(hdr->type==MSG_EMPLOYEE_LIST_RESP) {
    printf("Listing employees...\n");
    
    for (int i=0;i<hdr->len;++i)
      {
	struct employee_t employee;
	read(fd,(char*)&employee,sizeof(struct employee_t));
	employee.hours=ntohl(employee.hours);
	printf("%s, %s. %d\n",employee.name,employee.address,employee.hours);     
      }
  }
}
int add_func(int fd,char *addarg)
{
  char buf[4096]={0};
  dbproto_hdr_t *hdr=buf;
  hdr->type = MSG_EMPLOYEE_ADD_REQ;
  hdr->len=1;
  dbproto_employee_add_req *packed_data=(dbproto_hello_req*)&hdr[1];
  strncpy(&packed_data->data,addarg,sizeof(packed_data->data));
  hdr->type=htonl(hdr->type);
  hdr->len=htons(hdr->len);
  write(fd,buf,sizeof(dbproto_hdr_t)+sizeof(dbproto_employee_add_req));
  read(fd,buf,sizeof(buf));
  hdr->type=ntohl(hdr->type);
  hdr->len=ntohs(hdr->len);
  if(hdr->type==MSG_ERROR)
    {
      printf("adding user failed\n");
      close(fd);
      return STATUS_ERROR;
    }
  if(hdr->type==MSG_EMPLOYEE_ADD_RESP)
    {
      printf("user added successfully\n");
      return STATUS_SUCCESS;
    }
  return STATUS_ERROR;
}
int remove_func(int fd,char *removearg)
{
  char buf[4096]={0};
  dbproto_hdr_t *hdr=buf;
  hdr->type = MSG_EMPLOYEE_DEL_REQ;
  hdr->len=1;
  dbproto_employee_add_req *packed_data=(dbproto_hello_req*)&hdr[1];
  strncpy(&packed_data->data,removearg,sizeof(packed_data->data));
  hdr->type=htonl(hdr->type);
  hdr->len=htons(hdr->len);
  write(fd,buf,sizeof(dbproto_hdr_t)+sizeof(dbproto_employee_add_req));
  read(fd,buf,sizeof(buf));
  hdr->type=ntohl(hdr->type);
  hdr->len=ntohs(hdr->len);
  if(hdr->type==MSG_ERROR)
    {
      printf("removing user failed\n");
      close(fd);
      return STATUS_ERROR;
    }
  if(hdr->type==MSG_EMPLOYEE_DEL_RESP)
    {
      printf("user removed successfully\n");
      return STATUS_SUCCESS;
    }
  return STATUS_ERROR;
}
int handle_client(int fd)
{
  char buf[4096] = {0};
  dbproto_hdr_t *hdr=(dbproto_hdr_t*)&buf;
  hdr->type = MSG_HELLO_REQ;
  hdr->len = 1;
  dbproto_hello_req* hello=(dbproto_hello_req*)&hdr[1];
  hello->proto=PROTO_VR;
  hdr->type=htonl(hdr->type);
  hdr->len=htons(hdr->len);
  hello->proto=htons(hello->proto);
  write(fd,buf,sizeof(dbproto_hdr_t)+sizeof(dbproto_hello_req));
  read(fd,buf,sizeof(buf));
  hdr->type=ntohl(hdr->type);
  hdr->len=ntohs(hdr->len);
  if(hdr->type==MSG_ERROR)
    {
      printf("protocol mismatch\n");
      close(fd);
      return STATUS_ERROR;
    }
  printf("server connected protocol v1 \n");
  return STATUS_SUCCESS;
}
int main(int argc,char *argv[])
{
  bool list=false;
  char *addarg =NULL;
  char *removearg =NULL;
  char *update_arg=NULL;
  char *portarg = NULL, *hostarg=NULL;
  unsigned short port = 0;
  int c;
  if(argc < 2)
    {
      printf("Usage with ip address");
      return 0;
    }
  while((c=getopt(argc,argv,"p:h:a:u:r:l"))!=-1)
    {
      switch (c)
	{
	case 'a':
	  addarg = optarg;
	  break;
	  case 'r':
	  removearg = optarg;
	  break;
	case 'u':
	  update_arg = optarg;
	  break; 
	case 'p':
	  portarg = optarg;
	  port = atoi(portarg);
	  break;
	case 'h':
	  hostarg = optarg;
	  break;
	case 'l':
	  list=true;
	  break;
	case '?':
	  printf("unknown option -%c\n",c);
	default:
	  return -1;
	}
    }
  if(port == 0)
    {
      printf("bad port");
      return -1;
    }
  if(hostarg == NULL)
    {
      printf("must specify the host");
      return -1;
    }
  struct sockaddr_in serverInfo={0};
  serverInfo.sin_family= AF_INET;
  serverInfo.sin_addr.s_addr =inet_addr(hostarg);
  serverInfo.sin_port = htons(5555);
  int fd=socket(AF_INET,SOCK_STREAM,0);
  if(fd<0)
    {
      perror("socket");
      return -1;
    }
  if(connect(fd,(struct sockaddr*)&serverInfo, sizeof(serverInfo))==-1)
    {
      perror("connect");
      close(fd);
      return -1;
    }
      if(handle_client(fd)!=STATUS_SUCCESS)
	{
	  return -1;
	}
  if(addarg!=NULL)
    {
      if(add_func(fd,addarg)!=STATUS_SUCCESS)
	{
	  return -1;
	}
    }
      if(removearg!=NULL)
    {
      if(remove_func(fd,removearg)!=STATUS_SUCCESS)
	{
	  return -1;
	}
    }
  if(list)
    {
      list_client(fd);
    }
  if(update_arg!=NULL)
    {
      if(update_employee_cli(fd,update_arg)!=STATUS_SUCCESS)
	{
	  return -1;
	}
    }
  close(fd);

    }
