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
#define CHANNEL_NAME ("gevent")
#define RD_POSTFIX ("_RD")
#define WR_POSTFIX ("_WR")


void print_hex(const char *s)
{
  while(*s)
    printf("%02x", (unsigned int) *s++);
  printf("\n");
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

  //read from gevent and check fro errors
  char buf[BUF_SZ+1];
  while(1){
    size_t nread = read(fd,buf,BUF_SZ);
    if (-1 == nread) {
	    if (errno == EAGAIN) {
			  printf("stop the blocking call to read\n");
		  } else {
			  perror("something wrong");
			}

		  sleep(1);
		  continue;
    }else if (0 == nread) {
      if (errno == EAGAIN) {
			  printf("stop the blocking call to read\n");
			  sleep(1);
			  continue;
			} else {
			  // end of file?
		  }
	  } else {
      buf[BUF_SZ] = '\0';
	    printf("nread: %zd\n", nread);
		  printf("buffer: %s\n", buf);
	  }
        
  }
    
    // close channel
  close(fd);
  return 0;
}
