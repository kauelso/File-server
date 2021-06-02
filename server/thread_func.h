#include "dir.h"
#include "../write_file.h"
#include "../send_file.h"

void *recieve_func(void *arg)
{
    int new_socket = *((int*)arg);
    free(arg);
    int rc;
    char buffer[1024];
    char filename[1024];
    bzero(buffer,1024);
    bzero(filename,1024);

    rc = recv(new_socket,filename,sizeof(filename),0);//Recebe nome do arquivo
    if(write_file(new_socket,filename) < 0)//Comeca a escrever o arquivo
    {
        strcpy(buffer,"Erro ao criar arquivo no servidor.\n");
        send(new_socket,buffer,1024,0);
    }
    else{
        strcpy(buffer,"Arquivo criado com sucesso.\n");
        send(new_socket,buffer,1024,0);
    }
    return NULL;
}

void *send_func(void *arg)
{
    int new_socket = *((int*)arg);
    free(arg);
    int rc;
    size_t size = 0;
    FILE *fp;
    char path[4096];
    char filename[1024];
    bzero(filename,1024);

    rc = recv(new_socket,filename,sizeof(filename),0);//Recebe nome do arquivo
    sprintf(path,"./stored/%s",filename);//Cria o path do arquivo

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
    return NULL;
}

void *remove_func(void *arg)
{
    int new_socket = *((int*)arg);
    free(arg);
    int rc;
    char filename[1024];
    char path[4096];

    rc = recv(new_socket,filename,sizeof(filename),0);//Recebe nome do arquivo
    sprintf(path,"./stored/%s",filename);
    int res = 0;
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
    return NULL;
}

void *dir_func(void *arg)
{
    int new_socket = *((int*)arg);

    dir(new_socket);

    return NULL;
}