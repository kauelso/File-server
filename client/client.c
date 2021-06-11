#include "../header.h"
#include "../send_file.h"
#include "../write_file.h"

int sock = 0;

void closesock(){
    printf("Exiting socket...");
    close(sock);
    exit(1);
}

int main(int argc, char const *argv[])
{
    signal(SIGINT,closesock);

    int rc;
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
        size_t size = 0;
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
        
        send(sock,filename,1024,0);//Informa o nome do arquivo para o servidor
        printf("Enviando arquivo...\n");

        send_file(fp,sock,filename,size);//Inicia o envio do arquivo

        fclose(fp);       
    }
    if(num == 2){//Recebe o arquivo
        int res = 0;
        printf("Nome do arquivo: ");
        scanf("%s",filename);//Nome do arquivo a ser recebido
        send(sock,filename,BUFFER_SIZE,0);//Informa o nome do arquivo para o servidor

        rc = recv(sock,(int*)&res,sizeof(int),0);//Recebe resposta do servidor
        if (res == -1){
            printf("Arquivo nao existe no servidor.");
            close(sock);
            exit(1);
        }
        else//Caso exista o arquivo no servidor, escreve o arquivo localmente
        {
            printf("Recebendo arquivo...\n");
            write_file(sock,filename);
        }

        printf("Arquivo recebido com sucesso.\n");
    }
    if(num == 3){//Excluir um arquivo do servidor
        int res = 0;
        char buffer[4096];
        printf("Nome do arquivo: ");
        scanf("%s",filename);//Nome do arquivo a ser recebido
        send(sock,filename,BUFFER_SIZE,0);//Informa o nome do arquivo para o servidor
        rc = recv(sock,(int*)&res,sizeof(int),0);//Recebe resposta di servidor
        if (res == -1){
            printf("Erro ao encontrar arquivo no servidor.\n");
            close(sock);
            exit(1);
        }

        sprintf(buffer,"Arquivo %s removido do servidor.\n",filename);

        printf("%s\n",buffer);
        
        send(sock,buffer,BUFFER_SIZE,0);//Envia resposta ao servidor
        close(sock);
    }
    if(num == 4){
        int res = 0;
        rc = recv(sock,(int*)&res,sizeof(int),0);//Recebe resposta do servidor
        if (res == 0){
            printf("Erro no servidor ao listar arquivos.");
            close(sock);
            return 1;
        }
        char buffer[BUFFER_SIZE] = {0};
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
        sprintf(buffer,"Todos as informacoes foram recebidas.\n");

        printf("%s\n",buffer);
        
        send(sock,buffer,BUFFER_SIZE,0);//Envia resposta ao servidor
        close(sock);
    }
    
    return 0;
}