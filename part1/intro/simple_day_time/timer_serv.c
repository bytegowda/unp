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
	char client_ipv4[16] = {0};

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		errExit("failed to open socket\n");

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		errExit("setsockopt failed\n");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	sock_len = sizeof(servAddr);

	if(bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
		errExit("bind failed\n");
	}

	listen(sockfd, 5);

	for(;;) {
		memset(&clientAddr, 0, sizeof(clientAddr));
		memset(client_ipv4, 0, sizeof(client_ipv4));

		printf("\n\nWaiting for Connection.....\n");
		acceptfd = accept(sockfd, (struct sockaddr*)&clientAddr, &sock_len);
		if(accept < 0) {
			printf("Accept failed\n");
			continue;
		}

		inet_ntop(AF_INET, &clientAddr.sin_addr, client_ipv4, sizeof(client_ipv4));
		printf("===== Client %s is connected ===== \n", client_ipv4);
		memset(buf, 0, sizeof(buf));
		ticks = time(NULL);
		snprintf(buf, BUF_LEN, "%.24s\r\n", ctime(&ticks));

		if(write(acceptfd, buf, BUF_LEN) <= 0) {
			printf("write operation failed\n");
		}

		close(acceptfd);
		printf("===== Client %s is disconnected ===== \n", client_ipv4);
	}

	close(sockfd);
	return 0;
}
