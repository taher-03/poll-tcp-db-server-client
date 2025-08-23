#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
  for(int i=0;i<dbhdr->count;++i)
    {
      printf("Employee %d\n", i);
      printf("\tName: %s\n",employees[i].name);
      printf("\tAddress: %s\n",employees[i].address);
      printf("\tHours: %d\n",employees[i].hours);
    }
}
int update_employee(struct dbheader_t *dbhdr,struct employee_t *employees, char *updatestring)
{
  char *name = strtok(updatestring,",");
  char *addr = strtok(NULL,",");
  char *hours = strtok(NULL,",");
  char name_tmp[256];
  char addr_tmp[256];
  unsigned int hours_tmp= atoi(hours);
  strncpy(name_tmp,name,sizeof(name_tmp));
  strncpy(addr_tmp,name,sizeof(addr_tmp));
  int index=-1;
  for(int i=0;i<dbhdr->count;++i)
    {
      if( (strcmp(employees[i].name,name_tmp)) == 0)
	{
	  index = i;
	}
    }
  if(index<0)
    {
      printf("employee not found to update\n");
      return STATUS_ERROR;
    }
  else
    {
      strncpy(employees[index].address,addr,sizeof(employees[index].address));
      employees[index].hours = hours_tmp;
      return STATUS_SUCCESS;
    }
  return STATUS_ERROR;
}
int remove_employee(struct dbheader_t **dbhdr,struct employee_t **employees, char *removestring)
{
  char *name = strtok(removestring,",");
  char *addr = strtok(NULL,",");
  char name_tmp[256];
  char addr_tmp[256];
  strncpy(name_tmp,name,sizeof(name_tmp));
  strncpy(addr_tmp,addr,sizeof(addr_tmp));
  int index=-1;
  for(int i=0;i<(*dbhdr)->count;++i)
    {
      if( (strcmp((*employees)[i].name,name_tmp) == 0)&&(strcmp((*employees)[i].address,addr_tmp)==0))
	{
	  index = i;
	}
    }
  if(index<0)
    {
      printf("employee not found to delete\n");
      return STATUS_ERROR;
    }
  else
    {
      struct employee_t *temp=malloc(((*dbhdr)->count-1)*sizeof(struct employee_t));
      if(index!=0)
	{
	  memcpy(temp,*employees,index*sizeof(struct employee_t));
	}
      if(index!=((*dbhdr)->count-1))
	{
	  memcpy(temp+index,*employees+index+1, ((*dbhdr)->count-index-1)*sizeof(struct employee_t));
	}
      free(*employees);
      
      *employees=temp;
      --(*dbhdr)->count;
      return STATUS_SUCCESS;
    }
  return STATUS_ERROR;
}
int add_employee(struct dbheader_t **dbhdr, struct employee_t **employees, char *addstring) {
  char *name = strtok(addstring,",");
  char *addr = strtok(NULL,",");
  char *hours = strtok(NULL,",");
  printf("%s %s %s\n",name,addr,hours);
  (*dbhdr)->count++;
  *employees = realloc(*employees,(*dbhdr)->count*(sizeof(struct employee_t)));
  struct employee_t *emp=*employees;
  strncpy(emp[(*dbhdr)->count-1].name,name,sizeof(emp[(*dbhdr)->count-1].name));
  strncpy(emp[(*dbhdr)->count-1].address,addr,sizeof(emp[(*dbhdr)->count-1].address));
  emp[(*dbhdr)->count-1].hours = atoi(hours);
  return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
  if(fd<0)
    {
      printf("bad fd from user\n");
      return STATUS_ERROR;
    }
  int count = dbhdr->count;
  struct employee_t *employees = calloc(count,sizeof(struct employee_t));
  if(employees == NULL)
    {
      printf("calloc failed\n");
      return STATUS_ERROR;
    }
  read(fd,employees,count*sizeof(struct employee_t));
  for (int i=0;i<count;++i)
    {
      employees[i].hours = ntohl(employees[i].hours);
    }
  *employeesOut = employees;
  return STATUS_SUCCESS;
  
}

int output_file(int fd, struct dbheader_t *dbhdr,struct employee_t *employees){
  if(fd<0)
    {
      printf("bad fd from user\n");
      return STATUS_ERROR;
    }
  int realcount = dbhdr->count;
  struct dbheader_t *temp= calloc(1,sizeof(struct dbheader_t));
  temp->magic=htonl(dbhdr->magic);
  temp->filesize=htonl(sizeof(struct dbheader_t)+(sizeof(struct employee_t)*realcount));
  temp->count=htons(dbhdr->count);
  temp->version=htons(dbhdr->version);
  lseek(fd,0,SEEK_SET);
  write(fd,temp,sizeof(struct dbheader_t));
  for(int i=0;i<realcount;++i)
    {
      employees[i].hours = htonl(employees[i].hours);
      write(fd,&employees[i],sizeof(struct employee_t));
    }
  free(temp);
  return STATUS_SUCCESS;
}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {
  if (fd<0)
    {
      printf("bad fd\n");
      return STATUS_ERROR;
    }
  struct dbheader_t *header= calloc(1,sizeof(struct dbheader_t));
  if(header==NULL)
    {
      perror("create");
      return STATUS_ERROR;
    }
  if(read(fd,header,sizeof(struct dbheader_t)) !=sizeof(struct dbheader_t))
    {
      perror("read");
      free(header);
      return STATUS_ERROR;
    }
  header->version = ntohs(header->version);
  header->count = ntohs(header->count);
  header->magic = ntohl(header->magic);
  header->filesize = ntohl(header->filesize);
  if(header->version!=1)
    {
      printf("improper header version\n");
      free(header);
      return STATUS_ERROR;
    }
   if(header->magic!=HEADER_MAGIC)
    {
      printf("improper header version\n");
      free(header);
      return STATUS_ERROR;
    }
   struct stat dbstat = {0};
   fstat(fd,&dbstat);
   *headerOut=header;
}

int create_db_header(int fd, struct dbheader_t **headerOut) {
  struct dbheader_t *header= calloc(1,sizeof(struct dbheader_t));
  if(header==NULL)
    {
      perror("create");
      return STATUS_ERROR;
    }
  header->version = 0x1;
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(struct dbheader_t);
  struct dbheader_t *temp=calloc(1,sizeof(struct dbheader_t));
  temp->version = htons(header->version);
  temp->count = htons(header->count);
  temp->magic = htonl(header->magic);
  temp->filesize = htonl(header->filesize);
  write(fd,temp,sizeof(struct dbheader_t));
  *headerOut=header;
  return STATUS_SUCCESS;
}


