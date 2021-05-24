#include "../header.h"
#include "../send_file.h"
#include "../write_file.h"

int main(int argc, char const *argv[])
{
    int sock = 0, rc;
    struct sockaddr_in serv_addr;
    char filename[1024];
    char buffer[1024] = {0};
    //Cria Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    //Configura o Socket
    serv_addr.sin_family = AF_INET; //TCP
    serv_addr.sin_port = htons(PORT); //Porta
    serv_addr.sin_addr.s_addr = inet_addr(IP); //Endereco
    //Conecta o socket no servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    int num = 0;
    printf("-->Digite 1 para enviar um arquivo\n");
    printf("-->Digite 2 para receber um arquivo do servidor\n");
    printf("-->: ");
    scanf("%d",&num);
    fflush(stdin);
    send(sock, (int*)&num, sizeof(int),0);//Informa a opcao escolhida para o servidor

    if(num == 1){
        printf("Nome do arquivo: ");
        scanf("%s",filename);
        int size = 0;
        FILE *fp;
        char path[4096];
        sprintf(path,"./stored/%s",filename);
        fp = fopen(path,"r");
        if(fp == NULL){
            printf("Arquivo nao existe");
            exit(1);
        }
        struct stat st;
        stat(path,&st);
        size = st.st_size;
        send(sock,filename,BUFFER_SIZE,0);//Informa o nome do arquivo para o servidor
        send(sock,(int*)&size,sizeof(int),0);//Informa o tamanho do arquivo ai servidor
        send_file(fp,sock,filename,size);//Inicia o envio do arquivo
    }
    if(num == 2){
        printf("Nome do arquivo: ");
        scanf("%s",filename);
        send(sock,filename,BUFFER_SIZE,0);//Informa o nome do arquivo para o servidor
        int size = 0;
        rc = recv(sock,(int*)&size,sizeof(int),0);
        if (size == -1){
            printf("Arquivo nao existe no servidor.");
            exit(1);
        }
        printf("%s\n",filename);
        printf("%d\n",size);
        if(write_file(sock,filename,size) < 0)
        {
            printf("Erro ao criar arquivo %s no client.",filename);
        }
    }
    close(sock);
    return 0;
}