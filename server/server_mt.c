#include "../header.h"
#include "thread_func.h"

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
    int opt=0;
       
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

        if(opt == 1){
            pthread_t thread;
            pthread_create(&thread, NULL, recieve_func, (void *)&new_socket);
            //pthread_join(thread,NULL);
        }
        if(opt == 2){
            pthread_t thread;
            pthread_create(&thread, NULL, send_func, (void *)&new_socket);
            //pthread_join(thread,NULL);
        }
        if(opt == 3){
            pthread_t thread;
            pthread_create(&thread, NULL, remove_func, (void *)&new_socket);
            //pthread_join(thread,NULL);
        }
        if(opt == 4){
            pthread_t thread;
            pthread_create(&thread, NULL, dir_func, (void *)&new_socket);
            //pthread_join(thread,NULL);
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