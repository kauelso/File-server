#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <pthread.h>
#include <errno.h>

#define BUFFER_SIZE 4096
#define PORT 8080
#define IP "192.168.0.123"
