#include "../header.h"
#include "../write_file.h"
#include "../send_file.h"
#include "dir.h"

int server_fd, new_socket;

void closesock(){
    close(server_fd);
    exit(1);
}

int main(int argc, char const *argv[])
{
    signal(SIGINT,closesock);

    int rc;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024];
    char filename[1024];
    int opt=0;
    bzero(buffer,1024);
    bzero(filename,1024);
       
    // Creating socket file descriptor
    printf("Criando socket...\n");
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("Erro ao criar socekt");
        exit(1);
    }
    //Configuracoes do servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
       
    //Colocando o servidor para rodar nas configuracoes especificadas
    printf("Configurando servidor...\n");
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0)
    {
        printf("Bind falhou");
        exit(1);
    }
    //Servidor ouvindo novas requisicoes
    printf("Ouvindo requisicoes...\n");
    if (listen(server_fd, 3) < 0)
    {
        printf("Erro ao ouvir conexoes");
        exit(1);
    }
    //Aceita as requisicoes e trata elas no new_socket
    while((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))>0) {
        //Recebe qual opcao foi escolhida pelo usuario
        rc = recv(new_socket,(int*)&opt,sizeof(int),0);
        int size = 0;
        if(opt == 1){
            rc = recv(new_socket,filename,sizeof(filename),0);//Recebe nome do arquivo
            if(write_file(new_socket,filename) < 0)//Comeca a escrever o arquivo
            {
                strcpy(buffer,"Erro ao criar arquivo no servidor.\n");
                send(new_socket,buffer,BUFFER_SIZE,0);
            }
            else{
                strcpy(buffer,"Arquivo criado com sucesso.\n");
                send(new_socket,buffer,BUFFER_SIZE,0);
            }
            close(new_socket);
        }
        if(opt == 2){
            FILE *fp;
            char path[4096];
            rc = recv(new_socket,filename,sizeof(filename),0);//Recebe nome do arquivo
            sprintf(path,"./stored/%s",filename);
            fp = fopen(path,"r");
            if(fp == NULL){
                printf("Arquivo nao existe.");
                size = -1;
                send(new_socket,(int*)&size,sizeof(int),0);//Informa que o arquivo nao existe
            }
            else{
                struct stat st;
                stat(path,&st);
                int size = st.st_size;
                send(new_socket,(int*)&size,sizeof(int),0);
                if(send_file(fp,new_socket,filename,size) == 1){
                    size = -1;
                    send(new_socket,(int*)&size,sizeof(int),0);//Informa o estado da operacao
                }
                else{
                    size = 0;
                    send(new_socket,(int*)&size,sizeof(int),0);//Informa o estado da operacao
                }
            }
        }
            if(opt == 3){
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
        }
            if(opt == 4){
                dir(new_socket);
            }

    }
    if (new_socket <0)
    {
        printf("Erro ao aceitar conexao");
        exit(1);
    }
    
    close(new_socket);
    close(server_fd);
    return 0;
}