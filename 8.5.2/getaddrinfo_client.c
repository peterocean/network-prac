#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define BUF_SIZE 1024 
int main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *result;
	struct addrinfo *rp;
	int ret;
	int sfd;
	int j;
	char buf[BUF_SIZE];
	ssize_t nread;
	size_t len;

	if (argc < 3) {
		fprintf(stderr,"Usage: %s host port msg...\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags  = 0;
	hints.ai_protocol = 0; //any protocol

	ret = getaddrinfo(argv[0],argv[1],&hints,&result);
	if (ret != 0) {
		fprintf(stderr,"getaddrinfo: %s.\n",gai_strerror(ret));
		exit(EXIT_FAILURE);
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
		if (sfd < 0) 
			continue;	
		if(connect(sfd,rp->ai_addr,rp->ai_addrlen) < 0)
			break;
		close(sfd);
	}

	if (rp == NULL) {
		fprintf(stderr,"Couldn't connect.\n");
		exit(EXIT_FAILURE);
	}
	
	freeaddrinfo(result);
	
	for (j = 3 ; j < argc; j++) {
		len = strlen(argv[j]) + 1;
		if (len+1 > BUF_SIZE) {
			fprintf(stderr,"Ignore long message in argument %d.\n",j);
			exit(EXIT_FAILURE);
		}
		if (write(sfd,argv[1],len) != len) {
			fprintf(stderr,"Partial failed write.\n");
		}
		
		nread = read(sfd,buf,BUF_SIZE);
		if (nread < 0) {
			fprintf(stderr,"Read failed.\n");
			exit(EXIT_FAILURE);
		}
		printf("Received %ld bytes: %s.\n",(long)nread,buf);
	}

	exit(EXIT_SUCCESS);
}






