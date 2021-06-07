#include "dir.h"
#include "../write_file.h"
#include "../send_file.h"

void *recieve_func(void *arg)
{
    int new_socket = *((int*)arg);
    free(arg);
    int rc;
    char filename[1024];
    bzero(filename,1024);

    rc = recv(new_socket,filename,sizeof(filename),0);//Recebe nome do arquivo
    write_file(new_socket,filename);//Comeca a escrever o arquivo

    pthread_exit(NULL);
}

void *send_func(void *arg)
{
    int new_socket = *((int*)arg);
    free(arg);
    size_t size = 0;
    int rc;
    FILE *fp;
    char path[4096] = {0};
    char filename[1024] ={0};

    rc = recv(new_socket,filename,sizeof(filename),0);//Recebe nome do arquivo
    sprintf(path,"./stored/%s",filename);//Cria o path do arquivo
    printf("%s\n",path);

    fp = fopen(path,"r");

    if(fp == NULL){
        int res = -1;
        send(new_socket,(int*)&res,sizeof(int),0);
        fclose(fp);
        close(new_socket);
    }

    else{
        int res = 0;
        send(new_socket,(int*)&res,sizeof(int),0);

        struct stat st;
        stat(path,&st);
        size = st.st_size;//Recebe o tamanho do arquivo
        
        send_file(fp,new_socket,filename,size);//Inicia o envio do arquivo

        fclose(fp);
    }
    pthread_exit(NULL);
}

void *remove_func(void *arg)
{
    int new_socket = *((int*)arg);
    int res = 0;
    free(arg);
    char filename[1024];
    char path[4096];

    if(findName(filename) == 1){
        if(remove(path) != 0){
            res = -1;
            send(new_socket,(int*)&res,sizeof(int),0);//Informa o estado da operacao
        }
        else{
            printf("%s excluido\n",path);
            send(new_socket,(int*)&res,sizeof(int),0);//Informa o estado da operacao
        }
    }
    else{
        printf("Erro ao encontrar arquivo.");
        res = -1;
        send(new_socket,(int*)&res,sizeof(int),0);//Informa que o arquivo nao existe
    }

    recv(new_socket,path,4096,MSG_WAITALL);
    printf("%s\n",path);
    shutdown(new_socket,SHUT_WR);
    close(new_socket);
    pthread_exit(NULL);
}

void *dir_func(void *arg)
{
    int new_socket = *((int*)arg);

    dir(new_socket);

    char data[BUFFER_SIZE] = {0};

    recv(new_socket,data,4096,MSG_WAITALL);
    printf("%s\n",data);
    shutdown(new_socket,SHUT_WR);
    close(new_socket);

    pthread_exit(NULL);
}