#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define BUF_SZ (2048)
#define CHANNEL_NAME ("gevent")
#define RD_POSTFIX ("_RD")
#define WR_POSTFIX ("_WR")

int main(int argc, char** argv) {
    
    // create gevent and check for error
    if((mkfifo(CHANNEL_NAME, S_IRWXU | S_IRWXG)) == -1){
        fprintf(stderr, "Unable to create gevent"); 
    }
    //open gevent for read only and check error
    int fd = open("gevent", O_RDONLY);
    if(fd < 0){
        fprintf(stderr, "Unable to open gevent");
    }

    //read from gevent and check fro errors
    char buf[BUF_SZ];
    while(1){
        int res = read(fd,buf,BUF_SZ);
        if(res < 0){
            fprintf(stderr, "Unable to read gevent");
        }if(0 == res){
            printf("no data availbe\n")
        }
        
    }
    
    // close channel
    close(fd);
    return 0;
}
