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
  short type;
  //identifier
  char identifer[IDENT_SZ];
  //domain
  char domain[DOMAIN_SZ];


  while(1){

    //read from gevent and check fro errors
    size_t nread = read(fd,buf,BUF_SZ);
    if (nread < 0) {
			perror("read issues");
			break;
    }else if ( 0 == nread){
      sleep (5);
    }else {
      strncpy(type,buf,2);
      //CONNECT
      if(strcmp(type,1)){
        strncpy(identifer,buf+BUFSIZ,IDENT_SZ);
        strncpy(domain,buf+BUF_SZ+IDENT_SZ,DOMAIN_SZ);
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
      }
	}
  }
    
  // close channel
  if(close(fd) == -1){
    fprintf(stderr, "Unable to close gevent");
  }
  return 0;
}
