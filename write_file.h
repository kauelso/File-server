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

int write_file(int socketfd,char* filename){
  size_t n;
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
  while (1)
  {
    recv(socketfd, (size_t*)&n, sizeof(size_t), 0); //Recebe a quantidade de bytes
    printf("%ld\n",n);
    if(n < 0){
      printf("Error recieving data.\n");
      fclose(fp);
      close(socketfd);
      return -1;
    }
    if(n == 0){
      break;
    }
    recv(socketfd, buffer, n, 0); //Recebe os bytes do arquivo
    fwrite(buffer,sizeof(char),n,fp); //Escreve os bytes no arquivo
    
  }
  fclose(fp);
  return 0;
}