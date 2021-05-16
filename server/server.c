#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void write_file(int socket, char* buffer){
	int n;
	FILE *fp;
	char *filename = "text.txt";

	fp = fopen(filename,"w");
	while (1)
	{
		n = recv(socket,buffer,BUFFER_SIZE,0);
		if(n <= 0){
			break;
		}
		printf("%s\n",buffer);
		fprintf(fp,"%s",buffer);
		bzero(buffer,BUFFER_SIZE);
	}
	fclose(fp);
	return;
}

int main(int argc, char const *argv[])
{
	char * ip = "127.0.0.1";
	int socketfd, new_socket;
	struct sockaddr_in sv_address, new_addr;
	socklen_t addr_size;
	int addrlen = sizeof(sv_address);
	char buffer[1024];
	bzero(buffer,BUFFER_SIZE);
	
	// Criar o file descriptor do socket
	//AF_INET é uma familia de endereços que permite o socket se comunicar com
	//o protocolo ipv4
	//SOCK_STREAM protocolo de conecção TCP
	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Erro ao criar socket");
		exit(1);
	}

	printf("Socket criado...\n");
	
	sv_address.sin_family = AF_INET;
	sv_address.sin_addr.s_addr = inet_addr(ip);
	sv_address.sin_port = PORT;
	
	//Coloca o endereço no socket
	if (bind(socketfd, (struct sockaddr *)&sv_address,sizeof(sv_address))<0)
	{
		perror("bind falhou");
		exit(1);
	}

	printf("Bind feito com sucesso...\n");

	if (listen(socketfd, 10) < 0)
	{
		perror("Nao foi possivel ouvir outras conexoes");
		exit(1);
	}else{
		printf("Listening...");
	}

	addr_size = sizeof(new_addr);

	if ((new_socket = accept(socketfd, (struct sockaddr *)&new_addr,&addr_size))<0)
	{
		perror("accept error");
		exit(1);
	}

	write_file(new_socket, buffer);
	printf("Arquivo escrito com sucesso!\n");

	return 0;
}
