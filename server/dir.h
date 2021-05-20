#include <dirent.h>
#include "../header.h"

int dir(){
    struct dirent *arquivo;
    DIR *directory = opendir("./stored"); 
    if (directory == NULL){ 
        printf("Não foi possivel abrir o diretorio\n"); 
        return 0;
    } 
    arquivo = readdir(directory);
    while (arquivo != NULL){
        if (strcmp(arquivo->d_name,".")!=0 && strcmp(arquivo->d_name,"..")!=0)
        {
            printf("%s\n",arquivo->d_name);
        }
        
        arquivo = readdir(directory);
    }
    closedir(directory);
    return 1;
}