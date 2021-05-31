#include "./header.h"

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

int write_file(int socketfd,char* filename, int size){
  int n;
  FILE *fp;
  char path[BUFFER_SIZE];
  char buffer[BUFFER_SIZE];

  bzero(buffer,BUFFER_SIZE);

  if (writeName(filename) == 1){
    strcpy(buffer,"Erro ao transfirir arquivo");
    send(socketfd, buffer, sizeof(buffer), 0);
    printf("Erro ao escrever nome do arquivo.\n");
    exit(1);
  }

  sprintf(path,"./stored/%s",filename);

  int check_size = size;

  fp = fopen(path, "w");
  if(fp == NULL){
    strcpy(buffer,"Erro ao transfirir arquivo\n");
    return -1;
  }
  while (check_size > 0) {
    if (check_size >= BUFFER_SIZE){
      n = recv(socketfd, buffer, BUFFER_SIZE, 0);
      if(n < 0){
        printf("Error recieving data.\n");
        fclose(fp);
        close(socketfd);
        exit(1);
      }
      fwrite(buffer,sizeof(char),BUFFER_SIZE,fp);
    }
    else{
      n = recv(socketfd, buffer, check_size, 0);
      if(n < 0){
        printf("Error recieving data.\n");
        fclose(fp);
        close(socketfd);
        exit(1);
      }
      fwrite(buffer,sizeof(char),check_size,fp);
    }
    check_size = check_size - BUFFER_SIZE;
    bzero(buffer, BUFFER_SIZE);
  }
  strcpy(buffer,"Arquivo enviado com sucesso.\n");
  fclose(fp);
  return 0;
}