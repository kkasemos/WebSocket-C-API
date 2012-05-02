#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>
#include <check.h>
#include "handshake.h"
#include "websocket.h"
#include "util.h"

#define MAX_MSG       512
#define SERV_HOSTNAME "localhost"
#define SERV_PORT     8081
#define HELPS         "The options\n" \
"-b set I/O mode, 0 is blocking otherwise none-blocking" \
"\n\n" \

/* The echo websocket server */
int main(void)
{
	int srvsck, clisck;
	fd_set active_fd_set, read_fd_set;

	struct sockaddr cli_addr;
	socklen_t cli_addr_len;
	uint16_t port; 

	int success;
	int flags;
	int err;

	ssize_t bytes_read;
	ssize_t bytes_send;

	struct timeval timeout;
	char buffer[MAX_MSG];

	/* initialize websocket library */
	websck_init_lib();

	/* create websocket */
	port = SERV_PORT;
	srvsck = websck_bind("localhost", port);

	/* wait for a connection request from a client */
	success = listen(srvsck, 1);
	printf("%s", HELPS);

	/* initialize the set of active sockets */
	FD_ZERO(&active_fd_set);
	FD_SET(srvsck, &active_fd_set);

	/* initialize the timeout */
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;

	/* I/O multiplexing */
	read_fd_set = active_fd_set;
	err = select(FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout);

	/* accept the connection */
	cli_addr_len = sizeof(struct sockaddr);
	clisck = websck_accept(srvsck, &cli_addr, &cli_addr_len);

	/* /\* receive the socket *\/ */
	flags = 0;
	bytes_read = websck_recv(clisck, buffer, MAX_MSG, flags);
	print_hex(buffer, bytes_read);
	printf("%*s\n", bytes_read, buffer);
	
	/* send the socket */
	buffer[bytes_read] = '\0';
	strcat(buffer, " thanks as well");
	bytes_send = websck_send(clisck, buffer, strlen(buffer), flags);

	sleep(1);

	websck_shutdown(clisck, SHUT_RDWR);
	websck_close(clisck);

	websck_shutdown(srvsck, SHUT_RDWR);
	websck_close(srvsck);

}	
	
