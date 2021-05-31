#include "../header.h"
#include "../send_file.h"
#include "../write_file.h"

int main(int argc, char const *argv[])
{
    int sock = 0, rc;
    struct sockaddr_in serv_addr;
    char filename[1024];

    //Cria Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    printf("Socket criado.\n");

    //Configura o Socket
    serv_addr.sin_family = AF_INET; //TCP
    serv_addr.sin_port = htons(PORT); //Porta
    serv_addr.sin_addr.s_addr = inet_addr(IP); //Endereco

    //Conecta o socket no servidor
    printf("Conectando no servidor...\n");
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Conectado no servidor.\n");

    int num = 0;//Opcao escolhida pelo usuario
    printf("-->Digite 1 para enviar um arquivo\n");
    printf("-->Digite 2 para receber um arquivo do servidor\n");
    printf("-->Digite 3 para excluir um arquivo do servidor\n");
    printf("-->Digite 4 para listar os arquivos do servidor\n");
    printf("-->: ");
    scanf("%d",&num);
    fflush(stdin);
    send(sock, (int*)&num, sizeof(int),0);//Informa a opcao escolhida para o servidor

    if(num == 1){//Envia um arqivo
        printf("Nome do arquivo: ");
        scanf("%s",filename);//Nome do arquivo
        int size = 0;
        FILE *fp;
        char path[4096];
        sprintf(path,"./stored/%s",filename);//Cria o path do arquivo
        fp = fopen(path,"r");
        if(fp == NULL){
            printf("Arquivo nao existe");
            close(sock);
            exit(1);
        }
        struct stat st;
        stat(path,&st);
        size = st.st_size;//Recebe o tamanho do arquivo
        
        send(sock,filename,BUFFER_SIZE,0);//Informa o nome do arquivo para o servidor
        send(sock,(int*)&size,sizeof(int),0);//Informa o tamanho do arquivo ai servidor
        printf("Enviando arquivo...\n");
        send_file(fp,sock,filename,size);//Inicia o envio do arquivo

        rc = recv(sock,(int*)&size,sizeof(int),0);//Recebe o resultado da operacao, -1 para erro
        if (size == -1){
            printf("Erro no servidor.");
            close(sock);
            exit(1);
        }
        else{
            printf("Arquivo enviado com sucesso");
        }
        
    }
    if(num == 2){//Recebe o arquivo
        printf("Nome do arquivo: ");
        scanf("%s",filename);//Nome do arquivo a ser recebido
        send(sock,filename,BUFFER_SIZE,0);//Informa o nome do arquivo para o servidor
        int size = 0;
        rc = recv(sock,(int*)&size,sizeof(int),0);//Recebe resposta do servidor
        if (size == -1){
            printf("Arquivo nao existe no servidor.");
            close(sock);
            exit(1);
        }
        if(write_file(sock,filename,size) < 0)//Caso exista o arquivo no servidor, escreve o arquivo localmente
        {
            printf("Erro ao criar arquivo %s no client.",filename);
            close(sock);
            exit(1);
        }

        printf("Arquivo recebido com sucesso.\n");
        close(sock);
    }
    if(num == 3){//Excluir um arquivo do servidor
        int res = 0;
        printf("Nome do arquivo: ");
        scanf("%s",filename);//Nome do arquivo a ser recebido
        send(sock,filename,BUFFER_SIZE,0);//Informa o nome do arquivo para o servidor
        rc = recv(sock,(int*)&res,sizeof(int),0);//Recebe resposta di servidor
        if (res == -1){
            printf("Erro ao encontrar arquivo no servidor.\n");
            close(sock);
            exit(1);
        }

        printf("Arquivo excluido com sucesso.\n");
        close(sock);
    }
    if(num == 4){
        int res = 0;
        rc = recv(sock,(int*)&res,sizeof(int),0);//Recebe resposta di servidor
        if (res == 0){
            printf("Erro no servidor ao listar arquivos.");
            close(sock);
            return 1;
        }
        char buffer[1024] = {0};
        rc = recv(sock,buffer,BUFFER_SIZE,0);//Recebe nome de arquivo do servidor
        printf("%s\n",buffer);
        while(1){
            bzero(buffer,BUFFER_SIZE);
            rc = recv(sock,buffer,BUFFER_SIZE,0);//Recebe nome de arquivo do servidor
            if(strcmp(buffer,"END_OF_FUNC") == 0){
                break;
            }
            printf("%s\n",buffer);
        }
    }
    close(sock);
    return 0;
}