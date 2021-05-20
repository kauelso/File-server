#include "./header.h"

int write_file(int socketfd,char* filename, int size){
  int n;
  FILE *fp;
  char path[BUFFER_SIZE];
  sprintf(path,"./stored/%s",filename);
  char buffer[BUFFER_SIZE];
  bzero(buffer,BUFFER_SIZE);
  int check_size = size;

  fp = fopen(path, "w");
  if(fp == NULL){
    return -1;
  }
  while (check_size > 0) {
    if (check_size >= BUFFER_SIZE){
      n = recv(socketfd, buffer, BUFFER_SIZE, 0);
      fwrite(buffer,sizeof(char),BUFFER_SIZE,fp);
    }
    else{
      n = recv(socketfd, buffer, check_size, 0);
      fwrite(buffer,sizeof(char),check_size,fp);
    }
    check_size = check_size - BUFFER_SIZE;
    bzero(buffer, BUFFER_SIZE);
  }
  fclose(fp);
  return 0;
}