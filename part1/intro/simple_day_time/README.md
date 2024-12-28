# Server 
1. Open a Socket - sockfd = socket(AF_INET, SOCK_STREAM, 0);
		```
			where,
					AF_INET - IPv4, AF_INET6 -IPv6
					SOCK_STREAM - TCP, SOCK_DGRAM - UDP
		```

2. Assign appropriate info to struct sockaddr_in servAddr
	```
	The structure format of struct sockaddr_in :
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(PORT);
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	```

3. Set sockopt to reuse port and not wait for TIME_WAIT
	```
	The function setsockopt() format:
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
			where, int opt = 1;
	```

4. Bind the socket to the assigned address and port
	```
	The function bind() format:
		bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr));
	```

5. Start listening on the binded address and port
	```
	The function listen() format:
		listen(sockfd, backlog);
			sockfd - socket file descriptor
			backlog - no of connection requests on queue.
	```

6. In forever loop, add accept() function. It returns a client socket fd when connection is accepted.
	```
	The function accept() format:
		acceptfd = accept(sockfd, (struct sockaddr*)&clientAddr, &sock_len);
	```

7. For client info, use inet_ntop get the client info such as address it is connected over
	```
	The inet_ntop format:
		inet_ntop(AF_INET, &clientAddr.sin_addr, client_ipv4, sizeof(client_ipv4));
			where, char client_ipv4[20];
	```

8. Get current time and write to acceptfd
	```
	Steps to get current time and write the time to client socket:
		time_t ticks = 0;
		ticks = time(NULL);
		snprintf(buf, BUF_LEN, "%.24s\r\n", ctime(&ticks));
		write(acceptfd, buf, strlen(buf));
	```

9. Close the client connection after writing the current time.
	```
	This is important as this is required to accept new connections in a single threaded serial TCP connection.
		close(acceptfd);
	
	Close sockfd in Server if program exits the for(;;) loop
		close(sockfd);
	```

# Client

1. Open a Socket - sockfd = socket(AF_INET, SOCK_STREAM, 0);
		```
			where,
					AF_INET - IPv4, AF_INET6 -IPv6
					SOCK_STREAM - TCP, SOCK_DGRAM - UDP
		```

2. Assign appropriate info to struct sockaddr_in servAddr
	```
	The structure format of struct sockaddr_in :
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(PORT);

		Use inet_pton to convert IPv4/IPv6 from string to binary format

			if(inet_pton(AF_INET, "192.168.2.37", &servAddr.sin_addr) <= 0)
				errExit("inet_pton failed\n");
	```

3. Set sockopt to reuse port and not wait for TIME_WAIT
	```
	The setsockopt formate:
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
			where, int opt = 1;
	```

4. Now send the connection request to server using connect()
	```
	The connect format:
		connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr));
	```

5. Read the data written to sockfd by the Server.
	```
	The read format:
		while((nread = read(sockfd, buf, sizeof(buf))) > 0) {
			printf("buf = %s\n", buf);
		}
	```

6. Close the sockect connection with close()
	```
	The close() format:
		close(sockfd);
	```

## PitFalls
### Watchout for:
	1. Buffer overflows
	2. Incorrect conversion/use of text to binary and binary to text format of IP address
	3. Incorrect endianness of data.

