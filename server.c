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
#include <dirent.h>
#include <sys/select.h>
#include <signal.h>

#define BUF_SZ (2048)
#define IDENT_SZ (256)
#define DOMAIN_SZ (256)
#define MSG_SZ (1790)
#define CHANNEL_NAME ("gevent")
#define RD_POSTFIX ("_RD")
#define WR_POSTFIX ("_WR")



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

	int rd_fd;
	int wr_fd;

	//prevent zombie process
	signal(SIGCHLD,SIG_IGN);
  	while(1){

    	//read from gevent and check fro errors
    	size_t nread = read(fd,buf,BUF_SZ);
    	if (nread < 0) {
			perror("read issues");
			break;
    	}else if ( 0 == nread){
			//no input to gevent
		}else {

			//CONNECT
			if(*buf == 0 && *(buf+1) == 0){
				//read indentifier
				for(int i = 0; i < 256; i++){
					identifer[i] = buf[i+2];
					if(buf[i+2] == 0){
						break;
					}
				}
				//read domain
				for(int i = 0; i < 256; i++){
					domain[i] = buf[i+258];
					if(buf[i+258] == 0){
						break;
					}
				}

				identifer[255] = '\0';
				domain[255] = '\0';
        		
				char RD_filename[260+256];
				char WR_filename[260+256];
				strcpy(RD_filename,domain);
				strcpy(WR_filename,domain);
				strcat(RD_filename,"/");
				strcat(WR_filename,"/");
				strcat(RD_filename,identifer);
				strcat(WR_filename,identifer);
				strcat(RD_filename,RD_POSTFIX);
				strcat(WR_filename,WR_POSTFIX);

				//make domain and check error
				if(mkdir(domain,0777) == -1){
					if(errno == EEXIST){

					}else{
						fprintf(stderr,"Failed to create domain.");
					}
				}

				//create WR and RD FIFO and check error
				if((rd_fd = mkfifo(RD_filename,0777)) == -1){
      				fprintf(stderr, "Unable to create RD."); 
  				}
				if((wr_fd = mkfifo(WR_filename,0777)) == -1){
      				fprintf(stderr, "Unable to create WR."); 
  				}
				
				//client handler
				int client_handler = fork();
				if(client_handler < 0){
					fprintf(stderr,"Failed to create client handler");
				}
				else if(0 == client_handler){

					unsigned char buffer[BUF_SZ];
					unsigned char receive_buf[BUF_SZ];
					
					int rec_fd;
					//open RD and WR files
					int client_to_clienthandler = open(WR_filename,O_RDONLY);
					//open domain and read from it
					DIR *dir;
					struct dirent *ent;
					//read from WR
					size_t filename_sz;

					char write_path[DOMAIN_SZ+IDENT_SZ + 2];
					
					while(1){

						//read from client and check error
						size_t nread_b = read(client_to_clienthandler,buffer,BUF_SZ);
						
    					if (nread_b < 0) {
							perror("read issues");
							break;
    					}else if ( 0 == nread_b){
							//nothing no input data
						}else{
							//SAY
							if(*buffer == 1 && *(buffer + 1) == 0){
	
								//make receive binary
								for(int i = 0; i < BUF_SZ; i++){
									receive_buf[i] = 0;
								}
								//type
								receive_buf[0] = 3;

								//identifier
								for(int i = 0; i < IDENT_SZ; i++){
									receive_buf[i+2] = buf[i+2];
								}
								//message
								for(int i = 0; i < MSG_SZ; i++){
									receive_buf[i+258] = buffer[i+2];
								}
								
								//open directory and check error
								if ((dir = opendir(domain)) == NULL){
									fprintf(stderr, " failed to open directory\n");
								}
								
								//loop thourgh the files and send to every file that has _RD as postic and is not the identifier
								while ((ent = readdir (dir)) != NULL) {
									//check is identifer
    								if(strncmp(identifer,ent->d_name,strlen(identifer)) == 0){
										continue;
										
									}else{
										//check if _RD and write to it
										filename_sz = strlen(ent->d_name);
										
										if(strcmp(ent->d_name + filename_sz - 3 ,RD_POSTFIX) == 0){
											//open FIFO and write to it

											sprintf(write_path,"%s/%s",domain,ent->d_name);
											
											rec_fd = open(write_path,O_WRONLY);
											if(rec_fd < 0){
												fprintf(stderr, "Unable to open _RD by CH");
  											}

											//write to rd_fd
											if(write(rec_fd, receive_buf, BUF_SZ) < 0){
												fprintf(stderr,"Unable to RECEIVE");
											}
											
											if(close(rec_fd) == -1){
    											fprintf(stderr, "Unable to close _RD file");
  											}
										}
									}
  								}
								//close directory
								closedir (dir);

							}

							//SAYCONT
							if(*buffer == 2 && *(buffer+1) == 0){
								//make receivecont binary
								
								for(int i = 0; i < BUF_SZ; i++){
									receive_buf[i] = 0;
								}
								//type
								receive_buf[0] = 4;
								

								//identifier
								for(int i = 0; i < IDENT_SZ; i++){
									receive_buf[i+2] = buf[i+2];
								}
								//message
								for(int i = 0; i < MSG_SZ; i++){
									receive_buf[i+258] = buffer[i+2];
								}
								//termination byte
								receive_buf[2047] = buffer[2047];
								

								
								//open directory and check error
								if ((dir = opendir(domain)) == NULL){
									fprintf(stderr, " failed to open directory\n");
								}
								
								
								//loop thourgh the files and send to every file that has _RD as postic and is not the identifier
								while ((ent = readdir (dir)) != NULL) {
									//check is identifer
									
    								if(strncmp(identifer,ent->d_name,strlen(identifer)) == 0){
									
										continue;
										
									}else{
										//check if _RD and write to it
										filename_sz = strlen(ent->d_name);
										
										if(strcmp(ent->d_name + filename_sz - 3 ,RD_POSTFIX) == 0){
											//open FIFO and write to it

											sprintf(write_path,"%s/%s",domain,ent->d_name);
											
											rec_fd = open(write_path,O_WRONLY);
											if(rec_fd < 0){
												fprintf(stderr, "Unable to open _RD by CH");
  											}

											//write to rd_fd
											if(write(rec_fd, receive_buf, BUF_SZ) < 0){
												fprintf(stderr,"Unable to RECEIVE");
											}

											if(close(rec_fd) == -1){
    											fprintf(stderr, "Unable to close _RD file");
  											}
										}
									}
  								}
								//close directory
								closedir (dir);
							}

							//DISCONNECT
							if(*buffer == 7 && *(buffer+1) == 0){
								
								break;
							}

						}						
					}
					//disconnect
					if(close(client_to_clienthandler) == -1){
    					fprintf(stderr, "Unable to close client_to_clienthandler");
  					}
					unlink(RD_filename);
					unlink(WR_filename);
					return 0;
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