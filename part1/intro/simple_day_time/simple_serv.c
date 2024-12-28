#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUF_LEN		1024

void errExit(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	vfprintf(stdout, fmt, args);
	fprintf(stdout, "\n");

	va_end(args);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	int sockfd=0;
	int acceptfd=0;
	struct sockaddr_in servAddr, clientAddr;
	socklen_t sockAddr_size=0;
	char buf[BUF_LEN] = "Hello World, this is from Server";
	int nread=0;
	int opt=1;
	char client_ip4[16]= {0};

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0) {
		errExit("Failed to open socket, errno = %d", errno);
	}

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		errExit("setsockopt failed\n");
	}

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(8080);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
		errExit("bind error\n");
	}

	listen(sockfd, 5);

	sockAddr_size = sizeof(servAddr);

	for(;;) {
		printf("Waiting for the Connection.....\n");
		acceptfd = accept(sockfd, (struct sockaddr*)&clientAddr, &sockAddr_size);
		if(acceptfd < 0) {
			continue;
		}

		inet_ntop(AF_INET, &clientAddr.sin_addr, client_ip4, sizeof(client_ip4));

		printf("=======> Connected to Client, IP = %s <============\n", client_ip4);
		if((nread = write(acceptfd, buf, sizeof(buf))) <= 0) {
			errExit("write system call failed, errno=%d\n", errno);
		}
		printf("=======> Connection to Client Ended <============\n");

		close(acceptfd);
	}

	if(acceptfd)
		close(acceptfd);
	close(sockfd);
	return 0;
}
