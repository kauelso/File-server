#include "../header.h"
#include "../write_file.h"
#include "dir.h"

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, rc;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024];
    char filename[1024];
    int opt=0;
    bzero(buffer,1024);
    bzero(filename,1024);
       
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("Erro ao criar socekt");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
       
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0)
    {
        printf("Bind falhou");
        exit(1);
    }
    if (listen(server_fd, 3) < 0)
    {
        printf("Erro ao ouvir conexoes");
        exit(1);
    }

    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        

    if (new_socket <0)
    {
        printf("Erro ao aceitar conexao");
        exit(1);
    }

    int size = 0;
    rc = recv(new_socket,filename,sizeof(filename),0);
    rc = recv(new_socket,(int*)&size,sizeof(int),0);
    printf("%s\n",filename);
    printf("%d\n",size);
    if(write_file(new_socket,filename,size) < 0)
    {
        printf("Erro ao criar arquivo no servidor.");
        close(server_fd);
        return 1;
    }

    close(server_fd);
    return 0;
}