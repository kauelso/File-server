#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>

#define BUFFER_SIZE 1024
#define PORT 8080
#define IP "127.0.0.1"