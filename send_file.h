int send_file(FILE *fp, int socketfd, char * filename, size_t size){
  size_t n;
  char data[BUFFER_SIZE];
  bzero(data,BUFFER_SIZE);
  size_t check_size = size;
  printf("%ld\n",size);

  send(socketfd, (size_t*)&size, sizeof(size_t), 0); //Envia o tamanho do arquivo

  while(check_size > 0) {
    n = fread(data,sizeof(char),BUFFER_SIZE, fp); //Le bytes do arquivo e guarda a quantidade lida em n
    send(socketfd, data, n, 0); //Envia os bytes
    check_size = check_size - n; //Subtrai a quantidade lida com o total
    bzero(data, BUFFER_SIZE);
  }
  close(socketfd);
  return 0;
}