#include "dir.h"
#include "../write_file.h"
#include "../send_file.h"

void *recieve_func(void *arg)
{
    int *new_socket = (int*)arg;
    int size = 0;
    int rc;
    char filename[1024];
    char buffer[1024];

    rc = recv(*new_socket,filename,sizeof(filename),0);//Recebe nome do arquivo
    rc = recv(*new_socket,(int*)&size,sizeof(int),0);//Recebe tamanho do arquivo
    if(write_file(*new_socket,filename,size) < 0)//Comeca a escrever o arquivo
    {
        strcpy(buffer,"Erro ao criar arquivo no servidor.\n");
        send(*new_socket,buffer,BUFFER_SIZE,0);
    }
    else{
        strcpy(buffer,"Arquivo criado com sucesso.\n");
        send(*new_socket,buffer,BUFFER_SIZE,0);
    }
    pthread_exit(NULL);
}

void *send_func(void *arg)
{
    int *new_socket = (int*)arg;
    int size = 0;
    int rc;
    char filename[1024];
    FILE *fp;
    char path[4096];

    rc = recv(*new_socket,filename,sizeof(filename),0);//Recebe nome do arquivo
    sprintf(path,"./stored/%s",filename);
    fp = fopen(path,"r");
    if(fp == NULL){
        printf("Arquivo nao existe.");
        size = -1;
        send(*new_socket,(int*)&size,sizeof(int),0);//Informa que o arquivo nao existe
    }
    else{
        struct stat st;
        stat(path,&st);
        int size = st.st_size;
        send(*new_socket,(int*)&size,sizeof(int),0);
        if(send_file(fp,*new_socket,filename,size) == 1){
            size = -1;
            send(*new_socket,(int*)&size,sizeof(int),0);//Informa o estado da operacao
        }
        else{
            size = 0;
            send(*new_socket,(int*)&size,sizeof(int),0);//Informa o estado da operacao
        }
    }
    pthread_exit(NULL);
}

void *remove_func(void *arg)
{
    int *new_socket = (int*)arg;
    int rc;
    char filename[1024];
    char path[4096];

    rc = recv(*new_socket,filename,sizeof(filename),0);//Recebe nome do arquivo
    sprintf(path,"./stored/%s",filename);
    int res = 0;
    if(findName(filename) == 1){
        if(remove(path) != 0){
            res = -1;
            send(*new_socket,(int*)&res,sizeof(int),0);//Informa o estado da operacao
        }
        else{
            printf("%s excluido\n",path);
            send(*new_socket,(int*)&res,sizeof(int),0);//Informa o estado da operacao
        }
    }
    else{
        printf("Erro ao encontrar arquivo.");
        res = -1;
        send(*new_socket,(int*)&res,sizeof(int),0);//Informa que o arquivo nao existe
        
    }   
    pthread_exit(NULL);
}

void *dir_func(void *arg)
{
    int *new_socket = (int*)arg;

    dir(*new_socket);

    pthread_exit(NULL);
}