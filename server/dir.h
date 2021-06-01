#include <dirent.h>

int dir(int socket){
    int res = 0;
    struct dirent *arquivo;
    DIR *directory = opendir("./stored"); 
    if (directory == NULL){
        send(socket,(int*)&res,sizeof(int),0);//Informa o estado da operacao
        return 0;
    }
    res = 1;
    send(socket,(int*)&res,sizeof(int),0);//Informa o estado da operacao
    arquivo = readdir(directory);
    while (arquivo != NULL){
        if (strcmp(arquivo->d_name,".")!=0 && strcmp(arquivo->d_name,"..")!=0)
        {
            send(socket,arquivo->d_name,BUFFER_SIZE,0);//Informa o arquivo
        }
        
        arquivo = readdir(directory);
    }
    send(socket,"END_OF_FUNC",BUFFER_SIZE,0);//Informa o arquivo
    closedir(directory);
    return 1;
}