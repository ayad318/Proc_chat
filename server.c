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


int main(int argc, char** argv) {
    
  	// create gevent and check for error
	//printf("hello\n");
  	if((mkfifo(CHANNEL_NAME, S_IRWXU | S_IRWXG)) == -1){
      	fprintf(stderr, "Unable to create gevent"); 
  	}
  
  	//open gevent for read only and check error
	//printf("blocking\n");
  	int fd = open("gevent", O_RDONLY);
  	if(fd < 0){
		if(errno != EEXIST){
			fprintf(stderr, "Unable to open gevent");
		}
  	}
	//printf("open gevent\n");
  	//buffer
  	unsigned char buf[BUF_SZ];
  	
  	//identifier
  	char identifer[IDENT_SZ];
  	//domain
  	char domain[DOMAIN_SZ];


  	while(1){

		memset(buf, '\0', BUF_SZ);
    	//read from gevent and check fro errors
    	size_t nread = read(fd,buf,BUF_SZ);
    	if (nread < 0) {
			perror("read issues");
			break;
    	}else if ( 0 == nread){
			//printf("no data\n");
			//sleep(5);
		}else {
			
			//printf("nread: %zd\n", nread);
			//printf("buffer: %s\n", buf);
      		//IGNORE JUST FOR QUITTING THE LOOP
			if(*buf == 'q'){
				break;
			}

			//CONNECT
			if(*buf == 0 && *(buf+1) == 0){
				for(int i = 0; i < 256; i++){
					identifer[i] = buf[i+2];
					if(buf[i+2] == 0){
						break;
					}
				}
				for(int i = 0; i < 256; i++){
					domain[i] = buf[i+258];
					if(buf[i+258] == 0){
						break;
					}
				}
				identifer[255] = 0;
				domain[255] = 0;
        		
				char RD_filename[260];
				char WR_filename[260];
				strcpy(RD_filename,identifer);
				strcpy(WR_filename,identifer);
				printf("read filename: %s\n",strcat(RD_filename,RD_POSTFIX));
				printf("write filename: %s\n",strcat(WR_filename,WR_POSTFIX));
				printf("domain name: %s",domain);
				//make domain and check error
				if(mkdir(domain,0777) == -1){
					fprintf(stderr,"Failed to create domain.");
				}

				
				mkfifo(RD_filename,0777);
				mkfifo(WR_filename,0777);

				//client handler
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
	unlink(CHANNEL_NAME);
  	return 0;
}
