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
	int nread = 0;

	if(argc != 3) {
		errExit("Error: usage %s <-v4/-v6> <ip_address>\n", argv[0]);
	}

	if(!strcmp(argv[1], "-v4")) {
		af_family = AF_INET;
		servAddr_v4.sin_family = af_family;
		servAddr_v4.sin_port = htons(PORT);
		if(inet_pton(af_family, argv[2], &servAddr_v4.sin_addr) <= 0)
			errExit("inet_pton failed, err msg = %s\n", strerror(errno));

		srvAddr = &servAddr_v4;
		cliAddr = &cliAddr_v4;
		addr_len = sizeof(servAddr_v4);
	}
	else if(!strcmp(argv[1], "-v6")) {
		af_family = AF_INET6;
		servAddr_v6.sin6_family = af_family;
		servAddr_v6.sin6_port = htons(PORT);
		//servAddr_v6.sin6_addr = in6addr_loopback;
		if(inet_pton(af_family, argv[2], &servAddr_v6.sin6_addr) <= 0)
			errExit("inet_pton failed, err msg = %s\n", strerror(errno));

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

	if(connect(sockfd, (struct sockaddr *)srvAddr, (af_family == AF_INET)?(sizeof(struct sockaddr_in)):(sizeof(struct sockaddr_in6))) < 0) {
		errExit("connect failed, err msg = %s\n", strerror(errno));
	}

	printf("Connected to server\n");

	while((nread = read(sockfd, buf, BUF_LEN)) > 0) {
		printf("%s\n", buf);
	}

	if(nread == -1)
		printf("read failed\n");

	if(nread == 0)
		printf("Server connection ended\n");

	close(sockfd);

	return 0;
}
