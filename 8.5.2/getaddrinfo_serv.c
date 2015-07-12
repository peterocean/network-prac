#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 1024


int main(int argc, char *argv[])
{
	int res = 0;
	int sfd;
	struct addrinfo hints;
	struct addrinfo *addr_results;
	struct addrinfo *rp;
	struct sockaddr peer_addr;
	socklen_t peer_addr_len;
	char buf[BUF_SIZE];
	ssize_t nread;
	
	if (argc != 2) {
		fprintf(stderr,"Usage: %s port.\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	
	res = getaddrinfo(NULL,argv[1],&hints,&addr_results);	
	if (res != 0) {
		fprintf(stderr,"getaddrinfo: %s.\n",gai_strerror(res));
		exit(EXIT_FAILURE);
	}

	for (rp = addr_results; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);	
		if (sfd < 0) 
			continue;
		if (bind(sfd,rp->ai_addr,rp->ai_addrlen) == 0)
			break;
		close(sfd);
	}

	if (rp == NULL) {
		fprintf(stderr,"Couldn't bind. \n");
		exit(EXIT_FAILURE);
	}
	
	freeaddrinfo(addr_results);

	for (;;) {
		peer_addr_len  = sizeof(struct sockaddr);
		recvfrom(sfd,buf,BUF_SIZE,0,&peer_addr,&peer_addr_len);
		if (nread < 0)
			continue;
		char host[NI_MAXHOST];
		char service[NI_MAXSERV];
		
		res = getnameinfo(&peer_addr,peer_addr_len,host,NI_MAXHOST,
							service,NI_MAXSERV,NI_NUMERICSERV);
		if (res == 0)
			printf("Received %ld bytes from %s:%s.\n",(long)nread,host,service);
		else
			fprintf(stderr,"getnameinfo: %s.\n",gai_strerror(res));
		if (sendto(sfd,buf,nread,0,&peer_addr,peer_addr_len) != nread)
			fprintf(stderr,"Error sending response.\n");
	}

}





