#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/uio.h>
#include <string.h>
#include <stdint.h>

#define BUF_SZ (2048)
#define IDENT_SZ (256)
#define DOMAIN_SZ (256)
#define CHANNEL_NAME ("gevent")
#define RD_POSTFIX ("_RD")
#define WR_POSTFIX ("_WR")


void print_hex(const char *s){
	while(*s)
  		printf("%02x", (unsigned int) *s++);
	printf("\n");
}


void connect(unsigned char identifier[], unsigned char domain[]){
	
}

short toShort(const char* bytes) {
    
	return (short)(((unsigned char)bytes[1] << 8) | (unsigned char)bytes[0]);
}



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

  	//buffer
  	unsigned char buf[BUF_SZ];
  	//type
  	unsigned short type;
  	//identifier
  	unsigned char identifer[IDENT_SZ];
  	//domain
  	unsigned char domain[DOMAIN_SZ];


  	while(1){

    	//read from gevent and check fro errors
    	size_t nread = read(fd,buf,BUF_SZ);
    	if (nread < 0) {
			perror("read issues");
			break;
    	}else if ( 0 == nread){
      		sleep (5);
    	}else {
      		type = toShort(buf);
      		//CONNECT
    		if(1 == type){
        		memcpy(identifer,buf+BUFSIZ,IDENT_SZ);
        		memcpy(domain,buf+BUF_SZ+IDENT_SZ,DOMAIN_SZ);
				char RD_filename[IDENT_SZ + 4];
				char WR_filename[IDENT_SZ + 4];
				strcpy(RD_filename,identifer);
				strcpy(WR_filename,identifer);

				//make domain and check error
				if(mkdir(domain,0777) == -1){
					fprintf(stderr,"Failed to create domain.");
				}
        
				mkfifo(strcat(RD_filename,RD_POSTFIX),0777);
				mkfifo(strcat(WR_filename,WR_POSTFIX),0777);

				int client_handler = fork();
				if(0 == client_handler){
					printf("hello from child");
				}
      		}
		}
	}
    
  	// close channel
  	if(close(fd) == -1){
    	fprintf(stderr, "Unable to close gevent");
  	}
  	return 0;
}
