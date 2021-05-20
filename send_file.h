#include "./header.h"

void send_file(FILE *fp, int socketfd, char * filename, int size){
  int n;
  char data[BUFFER_SIZE];
  bzero(data,BUFFER_SIZE);
  int check_size = size;

  while(check_size > 0) {
    if (check_size >= BUFFER_SIZE){
      fread(data,sizeof(char),BUFFER_SIZE, fp);
      if (send(socketfd, data, sizeof(data), 0)  < 0) {
        char error[256];
        sprintf(error, "Nao foi possivel enviar o arquivo: %s\n", filename);
        perror(error);
        exit(1);
      }
    }
    else{
      fread(data,sizeof(char),check_size, fp);
      if (send(socketfd, data, sizeof(data), 0)  < 0) {
        char error[256];
        sprintf(error, "Nao foi possivel enviar o arquivo: %s\n", filename);
        perror(error);
        exit(1);
      }
    }
    check_size = check_size - BUFFER_SIZE;
    bzero(data, BUFFER_SIZE);
  }
}