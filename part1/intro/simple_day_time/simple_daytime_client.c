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
	char buf[BUF_LEN] = {0};
	int nread=0;
	int opt=1;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0) {
		errExit("Failed to open socket, errno = %d", errno);
	}

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		errExit("setsockopt failed\n");
	}

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(8080);

	if(inet_pton(AF_INET, "192.168.2.37", &servAddr.sin_addr) <= 0) {
		errExit("inet_pton failed\n");
	}

	if(connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
		errExit("connect error\n");
	}

	printf("Connected to Server\n");

	while((nread = read(sockfd, buf, BUF_LEN)) > 0) {
		printf("Server msg:\n%s\n", buf);
	}

	close(sockfd);
	return 0;
}
