int send_file(FILE *fp, int socketfd, char * filename, size_t size){
  size_t n;
  size_t sr; // send response
  char data[BUFFER_SIZE];
  bzero(data,BUFFER_SIZE);
  size_t check_size = size;
  printf("%ld\n",size);

  send(socketfd, (size_t*)&size, sizeof(size_t), 0); //Envia o tamanho do arquivo

  while(check_size > 0) {
    n = fread(data,sizeof(char),BUFFER_SIZE, fp); //Le bytes do arquivo e guarda a quantidade lida em n
    if(n <= 0){
      printf("Error reading data.\n");
      sprintf(data, "read: %s (%d)\n", strerror(errno), errno);
      printf("%s\n",data);
      close(socketfd);
      return -1;
    }
    sr = send(socketfd, (size_t*)&n, sizeof(size_t), 0); //Envia o tamanho do pacote
    if(sr <= 0){
      printf("Error sending data.\n");
      sprintf(data, "send: %s (%d)\n", strerror(errno), errno);
      printf("%s\n",data);
      close(socketfd);
      return -1;
    }
    sr = send(socketfd, data, n, 0); //Envia os bytes
    if(sr <= 0){
      printf("Error sending data.\n");
      sprintf(data, "send: %s (%d)\n", strerror(errno), errno);
      printf("%s\n",data);
      close(socketfd);
      return -1;
    }
    check_size = check_size - n; //Subtrai a quantidade lida com o total
    bzero(data, BUFFER_SIZE);
  }
  bzero(data, BUFFER_SIZE);

  n = recv(socketfd,data,BUFFER_SIZE,MSG_WAITALL);
  if(n < 0){
      printf("Error recieving data.\n");
      sprintf(data, "recv: %s (%d)\n", strerror(errno), errno);
      printf("%s\n",data);
      close(socketfd);
      return -1;
    }
  printf("%s\n",data);
  shutdown(socketfd,SHUT_WR);
  close(socketfd);

  return 0;
}