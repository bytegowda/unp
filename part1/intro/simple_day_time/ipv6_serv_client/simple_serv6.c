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
	struct sockaddr_in6 servAddr, clientAddr;
	socklen_t sockAddr_size=0;
	char buf[BUF_LEN] = "Hello World, this is from Server";
	int nread=0;
	int opt=1;
	char client_ip6[46]= {0};

	sockfd = socket(AF_INET6, SOCK_STREAM, 0);

	if(sockfd < 0) {
		errExit("Failed to open socket, errno = %d", errno);
	}

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		errExit("setsockopt failed\n");
	}

	servAddr.sin6_family = AF_INET6;
	servAddr.sin6_port = htons(8080);
	//servAddr.sin6_addr = in6addr_any;
	servAddr.sin6_addr = in6addr_loopback;

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

		inet_ntop(AF_INET6, &clientAddr.sin6_addr, client_ip6, sizeof(client_ip6));

		printf("=======> Connected to Client, IP = %s <============\n", client_ip6);
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
