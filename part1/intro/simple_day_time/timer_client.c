#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#define	BUF_LEN	1024
#define	PORT	8080

void errExit(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	vfprintf(stdout, fmt, args);
	fprintf(stdout, "\n");

	va_end(args);
	exit(EXIT_FAILURE);
}

int main() {
	int sockfd=0, acceptfd=0;
	struct sockaddr_in servAddr, clientAddr;
	int opt=1;
	socklen_t sock_len=0;
	time_t ticks;
	char buf[BUF_LEN] = {0};
	int nread=0;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		errExit("failed to open socket\n");

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		errExit("setsockopt failed\n");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, "192.168.2.37", &servAddr.sin_addr) <= 0) {
		errExit("inet_pton failed\n");
	}

	if(connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
		errExit("bind failed\n");
	}

	printf("====== Connected to Server ======\n");
	while((nread = read(sockfd, buf, BUF_LEN)) > 0 ) {
		printf("time = %s\n", buf);
	}

	if(nread == 0)
		printf("==== Connection to Server Disconnected =====\n");

	if(nread == -1)
		printf("failed to read\n");

	close(sockfd);
	return 0;
}
