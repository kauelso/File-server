#include "./header.h"

int send_file(FILE *fp, int socketfd, char * filename, size_t size){
  size_t n;
  char data[BUFFER_SIZE];
  bzero(data,BUFFER_SIZE);
  size_t check_size = size;

  while(check_size > 0) {
    n = fread(data,sizeof(char),BUFFER_SIZE, fp); //Le 1024 bytes do arquivo e guarda a quantidade lida em n
    printf("%ld\n",n);
    send(socketfd, (size_t*)&n, sizeof(size_t), 0); // Envia a quantidade lida
    send(socketfd, data, n, 0); //Envia os bytes
    check_size = check_size - n; //Subtrai a quantidade lida com o total
    bzero(data, BUFFER_SIZE);
  }
  printf("%ld\n",n);
  send(socketfd, (size_t*)&check_size, sizeof(size_t), 0);
  return 0;
}