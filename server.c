#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
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


    int fd = open(CHANNEL_NAME, "r");
    lseek(fd, 0, SEEK_SET);
    char buf[BUF_SZ];
    read(fd, buf, BUF_SZ);
    

    //while(1){
    //   fgets(buf,BUF_SZ,read_channel);
    //}
    // close channel
    close(fd);
    return 0;
}
