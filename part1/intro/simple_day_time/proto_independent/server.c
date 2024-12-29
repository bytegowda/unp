#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>


#define	BUF_LEN	1024
#define PORT	8080

void errExit(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	vfprintf(stdout, fmt, args);
	fprintf(stdout, "\n");

	va_end(args);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	int sockfd, acceptfd;
	struct sockaddr_in servAddr_v4, cliAddr_v4;
	struct sockaddr_in6 servAddr_v6, cliAddr_v6;
	char buf[BUF_LEN] = {0};
	int af_family = AF_INET;	//default option
	void *srvAddr = NULL;
	void *cliAddr = NULL;
	socklen_t addr_len = 0;
	time_t ticks = 0;
	char ipAddr[46] = {0};
	int opt = 1;

	if(argc != 2) {
		errExit("Error: usage %s <-v4/-v6>\n", argv[0]);
	}

	if(!strcmp(argv[1], "-v4")) {
		af_family = AF_INET;
		servAddr_v4.sin_family = af_family;
		servAddr_v4.sin_port = htons(PORT);
		servAddr_v4.sin_addr.s_addr = INADDR_ANY;
		srvAddr = &servAddr_v4;
		cliAddr = &cliAddr_v4;
		addr_len = sizeof(servAddr_v4);
	}
	else if(!strcmp(argv[1], "-v6")) {
		af_family = AF_INET6;
		servAddr_v6.sin6_family = af_family;
		servAddr_v6.sin6_port = htons(PORT);
		servAddr_v6.sin6_addr = in6addr_loopback;
		srvAddr = &servAddr_v6;
		cliAddr = &cliAddr_v6;
		addr_len = sizeof(servAddr_v6);
	}
	else {
		errExit("Error unknow option %s: usage %s <-v4/-v6>\n", argv[1], argv[0]);
	}

	sockfd = socket(af_family, SOCK_STREAM, 0);
	if(sockfd < 0)
		errExit("errno=%d, errno msg = %s\n", errno, strerror(errno));

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		errExit("setsockopt failed, errno msg = %s\n", strerror(errno));
	}

//	if(bind(sockfd, (struct sockaddr*)srvAddr, sizeof(*srvAddr)) < 0) {
	if(bind(sockfd, (struct sockaddr*)srvAddr, (af_family == AF_INET) ? sizeof(servAddr_v4) : sizeof(servAddr_v6)) < 0) {
		errExit("Bind error, errno msg = %s\n", strerror(errno));
	}

	listen(sockfd, 5);

	for(;;) {
		printf("Waiting for the connection....\n");
		acceptfd = accept(sockfd, (struct sockaddr *)cliAddr, &addr_len);
		if(acceptfd < 0) {
			printf("accept failed\n");
			continue;
		}
		if(af_family == AF_INET) {
			inet_ntop(af_family, &((struct sockaddr_in *)cliAddr)->sin_addr, ipAddr, sizeof(ipAddr));
		}
		else {
			inet_ntop(af_family, &((struct sockaddr_in6 *)cliAddr)->sin6_addr, ipAddr, sizeof(ipAddr));
		}

		printf("Connection received from IP %s\n", ipAddr);
		ticks = time(NULL);
		snprintf(buf, BUF_LEN, "%.24s\r\n", ctime(&ticks));
		if(write(acceptfd, buf, BUF_LEN) <= 0) {
			printf("Write to client fd failed\n");
		}
		close(acceptfd);
		printf("Disconnected client IP : %s\n", ipAddr);
	}

	close(sockfd);

	return 0;
}
