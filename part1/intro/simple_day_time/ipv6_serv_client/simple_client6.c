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
	char buf[BUF_LEN] = {0};
	int nread=0;
	int opt=1;

	sockfd = socket(AF_INET6, SOCK_STREAM, 0);

	if(sockfd < 0) {
		errExit("Failed to open socket, errno = %d", errno);
	}

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		errExit("setsockopt failed\n");
	}

	servAddr.sin6_family = AF_INET6;
	servAddr.sin6_port = htons(8080);
	servAddr.sin6_addr = in6addr_loopback;

#if 0
	//if(inet_pton(AF_INET6, "fe80::bf82:4775:aafb:919f", &servAddr.sin6_addr) <= 0) {
	if(inet_pton(AF_INET6, "fe80::", &servAddr.sin6_addr) <= 0) {

		errExit("inet_pton failed\n");
	}
#endif

	if(connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
		errExit("connect error, errno = %d, error msg = %s\n", errno, strerror(errno));
	}

	printf("Connected to Server\n");

	while((nread = read(sockfd, buf, BUF_LEN)) > 0) {
		printf("Server msg:\n%s\n", buf);
	}

	close(sockfd);
	return 0;
}
