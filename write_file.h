int findName(char* filename){
  struct dirent *arquivo;
  DIR *directory = opendir("./stored"); 
  if (directory == NULL){ 
      printf("Não foi possivel abrir o diretorio\n"); 
      return -1;
  } 
  arquivo = readdir(directory);
  while (arquivo != NULL){
      if (strcmp(arquivo->d_name,filename) == 0)
      {
        closedir(directory);
        return 1;
      }
      
      arquivo = readdir(directory);
  }
  closedir(directory);
  return 0;
}

int writeName(char* filename){
  int found = findName(filename);
  int count = 1;

  if (found == -1){
    return -1;
  }

  char buff[1024];
  strcpy(buff,filename);
  while (found == 1)
  {
    sprintf(buff,"(%d)%s",count, filename);
    found = findName(buff);
    count++;
  }

  strcpy(filename,buff);
  return 0;
}

int write_file(int socketfd,char* filename){
  ssize_t n,m;
  size_t size, ws, rcs;
  FILE *fp;
  char path[BUFFER_SIZE];
  char buffer[BUFFER_SIZE];

  bzero(buffer,BUFFER_SIZE);

  if (writeName(filename) == 1){
    strcpy(buffer,"Erro ao transfirir arquivo");
    send(socketfd, buffer, sizeof(buffer), 0);
    printf("Erro ao escrever nome do arquivo.\n");
    return -1;
  }

  sprintf(path,"./stored/%s",filename);

  fp = fopen(path, "w");
  if(fp == NULL){
    strcpy(buffer,"Erro ao transfirir arquivo\n");
    fclose(fp);
    return -1;
  }

  n = recv(socketfd, (size_t*)&size, sizeof(size_t), 0); //Recebe o tamanho do arquivo
  if(n <= 0){
      printf("Error recieving data.\n");
      sprintf(buffer, "recv: %s (%d)\n", strerror(errno), errno);
      printf("%s\n",buffer);
      send(socketfd,buffer,BUFFER_SIZE,0);//Envia resposta ao servidor
      close(socketfd);
      fclose(fp);
      return -1;
    }
  printf("%ld\n",size);
  
  while (size > 0)
  {
    m = recv(socketfd, (size_t*)&rcs, sizeof(size_t), MSG_WAITALL); //Recebe o tamanho do pacote enviado
    n = recv(socketfd, buffer, rcs, MSG_WAITALL); //Recebe os bytes do arquivo
    if(n <= 0 || m <= 0){
      printf("Error recieving data.\n");
      sprintf(buffer, "recv: %s (%d)\n", strerror(errno), errno);
      printf("%s\n",buffer);
      send(socketfd,buffer,BUFFER_SIZE,0);//Envia resposta ao servidor
      close(socketfd);
      fclose(fp);
      return -1;
    }
    ws = fwrite(buffer,sizeof(char),rcs,fp); //Escreve os bytes no arquivo
    if(ws != rcs){
      printf("%ld",rcs);
      printf("Error writing data.\n");
      sprintf(buffer, "fwrite: %s (%d)\n", strerror(errno), errno);
      printf("%s\n",buffer);
      send(socketfd,buffer,BUFFER_SIZE,0);//Envia resposta ao servidor
      close(socketfd);
      fclose(fp);
      return -1;
    }
    bzero(buffer, BUFFER_SIZE);
    size = size - n;
  }

  bzero(buffer,BUFFER_SIZE);
  sprintf(buffer,"Arquivo %s recebido.\n",filename);

  printf("%s\n",buffer);
  
  send(socketfd,buffer,BUFFER_SIZE,0);//Envia resposta ao servidor
  close(socketfd);

  fclose(fp);
  return 0;
}