#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define BUF_SZ (2048)
#define CHANNEL_NAME ("gevent")

int main(int argc, char** argv) {
    
    // create gevent and check for error
    if((mkfifo(CHANNEL_NAME, S_IRWXU | S_IRWXG)) == -1){
        fprintf(stderr, "Unable to create gevent"); 
    }
    //open gevent for read only and check error
    int fd = open(CHANNEL_NAME, O_RDONLY);
    if(-1 == fd){
        fprintf(stderr, "Unable to open gevent");
    }
    
    //read from gevent and store and check error
    unsigned char buf[BUF_SZ];
    if(read(fd,buf,BUF_SZ) == -1){
        fprintf(stderr, "Unable to read gevent");
    }
    
    if (fclose(CHANNEL_NAME) == -1){
        fprintf(stderr, "Unable to close gevent");
    } 
    
    return 0;
}
