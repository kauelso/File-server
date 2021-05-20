#include "../header.h"
#include "../send_file.h"

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char filename[1024] = "Autômatos de Pilha-20210428_162033-Meeting Recording.mp4";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(IP);
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    //send(sock , hello , strlen(hello) , 0 );
    int num = 0;
    printf("-->Digite 1 para ver arquivos no servidor\n");
    printf("-->Digite 2 para enviar um arquivo\n");
    printf("-->Digite 3 para receber um arquivo do servidor\n");
    printf("-->: ");
    // scanf("%d",&num);
    // send(sock, (int*)&num, sizeof(int),0);
    // if(num == 2){
        FILE *fp;
        fp = fopen(filename,"r");
        if(fp == NULL){
            printf("Arquivo nao existe");
            exit(1);
        }
        struct stat st;
        stat(filename,&st);
        int size = st.st_size;
        send(sock,filename,BUFFER_SIZE,0);
        send(sock,(int*)&size,sizeof(int),0);
        printf("Tamanho: %d\n",size);
        send_file(fp,sock,filename,size);
    // }

    read( sock , buffer, 1024);
    printf("%s\n",buffer );

    close(sock);
    return 0;
}