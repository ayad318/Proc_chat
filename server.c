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


    FILE* read_channel = fopen(CHANNEL_NAME, "r");
    char buf[BUF_SZ];

    while(1){
        fgets(buf,BUF_SZ,read_channel);
    }
    // close channel
    fclose(read_channel);
    
    return 0;
}
