#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>
#include <check.h>
#include "handshake.h"
#include "websocket.h"

#define SERV_HOSTNAME "localhost"
#define SERV_PORT     8081
#define MAX_MSG       512

/* client handshak fixture */
#define CLI_HNDSHK_FIX "GET /chat HTTP/1.1\r\n" \
        "Host: localhost\r\n" \
        "Upgrade: websocket\r\n" \
        "Connection: Upgrade\r\n" \
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n" \
        "Origin: http://localhost\r\n" \
        "Sec-WebSocket-Protocol: chat, superchat\r\n" \
        "Sec-WebSocket-Version: 13\r\n\r\n" \

#define SRV_HNDSHK_FIX "HTTP/1.1 101 Switching Protocols\r\n" \
        "Upgrade: websocket\r\n" \
        "Connection: Upgrade\r\n" \
        "Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n\r\n" \

pthread_t cli_thread_id;

void *client_websck_test(void *arg)
{
	int clisck;
	uint16_t port = SERV_PORT;
	char buffer[MAX_MSG];
	int bytes;

	printf("Client websocket started\n");
  
	clisck = websck_connect(SERV_HOSTNAME, port);
  
	if(clisck < 0)
	{
		perror("Failed to connect to the client websocket");
	}

	if(websck_send(clisck, CLI_HNDSHK_FIX, strlen(CLI_HNDSHK_FIX), 0) < 0)
	{
		perror("Failed to send the 'hello world'");
	}

	if((bytes = websck_recv(clisck, buffer, MAX_MSG, 0)) < 0)
	{
		perror("Failed to receive the server response");
	}
	else
	{
		printf("Received\n%*s", bytes, buffer); 
	}

	/* wait for the server to receive the message */
	sleep(1);

	websck_shutdown(clisck, SHUT_RDWR);
	websck_close(clisck);
}

void  start_client_websck_test_thread(const char *host, uint16_t port)
{
	pthread_attr_t thrd_attr;
	int arg = 1;

	pthread_attr_init(&thrd_attr);
	pthread_create(&cli_thread_id, &thrd_attr, client_websck_test, &arg);
	pthread_attr_destroy(&thrd_attr);
}

/* integration test */
START_TEST(test_websck_integr)
{
	int serv_fd, clisck;
	fd_set active_fd_set, read_fd_set;
	int success;
	struct sockaddr cli_addr;
	socklen_t cli_addr_len;
	char buffer[MAX_MSG];
	int flags;
	ssize_t bytes_read;
	int err;
	ssize_t bytes_send;
	struct timeval timeout;
	uint16_t port; 

	/* initialize websocket library */
	websck_init_lib();

	/* create websocket */
	port = SERV_PORT;
	serv_fd = websck_bind("localhost", port);
	fail_unless(serv_fd > -1, "Failed to create the websocket");

	/* wait for a connection request from a client */
	success = listen(serv_fd, 1);
	fail_unless(success > -1, "Failed to listen the websocket");

	/* initialize the set of active sockets */
	FD_ZERO(&active_fd_set);
	FD_SET(serv_fd, &active_fd_set);

	/* initialize the timeout */
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;

	/* start the client websocket in another thread */
	start_client_websck_test_thread("localhost", port);

	/* I/O multiplexing */
	read_fd_set = active_fd_set;
	err = select(FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout);
	fail_unless(err > -1, "Failed to select the websocket");

	/* accept the connection */
	cli_addr_len = sizeof(struct sockaddr);
	clisck = websck_accept(serv_fd, &cli_addr, &cli_addr_len);
	fail_unless(clisck > -1, "Failed to accept the client websocket %d");

	/* /\* receive the socket *\/ */
	/* flags = 0; */
	/* bytes_read = websck_recv(clisck, buffer, MAX_MSG, flags); */
	/* fail_unless(bytes_read > -1, "Failed to receive the client websocket"); */

	/* /\* send the socket *\/ */
	/* buffer[bytes_read] = '\0';  */
	/* strcat(buffer, " thanks as well"); */
	/* bytes_send = websck_send(clisck, buffer, strlen(buffer), flags);  */
	/* fail_unless(bytes_send > -1,  */
	/* 	    "Failed to send the data to the client websocket"); */

	/* wait for the client websocket to termintate */
	pthread_join(cli_thread_id, NULL);  

	websck_shutdown(clisck, SHUT_RDWR);
	websck_close(clisck);

	websck_shutdown(serv_fd, SHUT_RDWR);
	websck_close(serv_fd);
}
END_TEST

int stub_recv_cli_hndshk(int socket, void *buff, size_t size, int flags)
{
	strcpy(buff, CLI_HNDSHK_FIX);
	return strlen(CLI_HNDSHK_FIX);
}

const char *srv_sentbuff;
size_t srv_sentbuff_size;

int stub_send_srv_hndshk(int socket, const void *buff, size_t size, int flags)
{
	
	srv_sentbuff = buff;
	srv_sentbuff_size = size;

	return strlen(SRV_HNDSHK_FIX);
}

START_TEST(test_websck_recv_hndshk)
{
	char buff[MAX_MSG];
	size_t size;
	int socket;
	int n; /* number of bytes received */

	/* setup pointers to websocket functions */
	neti_recv = stub_recv_cli_hndshk;
  
	n = websck_recv_hndshk(socket, buff, size);  
  
	fail_unless(strncmp(buff, CLI_HNDSHK_FIX, n) == 0, 
		    "Client handshake mismatched\n%*s", n, buff);
	fail_unless(n == 219, "Client handshake size mismatched %d\n", n);
}
END_TEST

START_TEST(test_websck_send_srv_hndshk)
{
	char buff[MAX_MSG];
	struct websck_hndshk srvhnd;
	int socket;

	/* setup pointer to websocket send functions */
	neti_send = stub_send_srv_hndshk;

        hndshk_init(&srvhnd);
	hndshk_parse(SRV_HNDSHK_FIX, strlen(SRV_HNDSHK_FIX), &srvhnd);

	/* send server handshake */
	websck_send_srv_hndshk(socket, &srvhnd);

	fail_unless(
		strncmp(srv_sentbuff, SRV_HNDSHK_FIX, srv_sentbuff_size) == 0,
		"Failed to send the server handshake %s", srv_sentbuff);
}
END_TEST

START_TEST(test_websck_recv_cli_hndshk)
{
	char buff[MAX_MSG];
	int clisck;
	int r;
	struct websck_hndshk acthnd;
	struct websck_hndshk exphnd;
  
        /* setup pointers to websocket functions */
	neti_recv = stub_recv_cli_hndshk;

	/* expected handshake */
	hndshk_init(&exphnd);
	hndshk_parse(CLI_HNDSHK_FIX, strlen(CLI_HNDSHK_FIX), &exphnd);

	/* parse the client handshake */
	r = websck_recv_cli_hndshk(clisck, &acthnd);

	fail_unless(hndshk_cmp(&acthnd, &exphnd), 
		    "Failed to process the client handshake %s", 
		    hndshk_sprintf(buff, &acthnd));
}
END_TEST


START_TEST(test_websck_bind)
{
	int socket;

	socket = websck_bind(SERV_HOSTNAME, SERV_PORT);
	websck_shutdown(socket, SHUT_RDWR);
	websck_close(socket);

	fail_unless(socket > -1, "Failed to bind websocket");
}
END_TEST

Suite *websck_suite(void)
{
	Suite *s = suite_create("websocket");
	TCase *tc;
 
	/* socket test case */
	tc = tcase_create("test_websck_bind");
	tcase_add_test(tc, test_websck_bind);
	suite_add_tcase(s, tc);
  
	/* process the client websocket handshake test case */
	tc = tcase_create("test_recv_cli_hndshk");
	tcase_add_test(tc, test_websck_recv_cli_hndshk);
	suite_add_tcase(s, tc);

	/* receive the handshake */
	tc = tcase_create("test_websck_recv_hndshk");
	tcase_add_test(tc, test_websck_recv_hndshk);
	suite_add_tcase(s, tc);

	/* send the server handshake */
	tc = tcase_create("test_websck_send_srv_hndshk");
	tcase_add_test(tc, test_websck_send_srv_hndshk);
	suite_add_tcase(s, tc);

	/* integration test case */
	tc = tcase_create("integration");
	tcase_add_test(tc, test_websck_integr);
	suite_add_tcase(s, tc);
  
	return s;
}

int main(void)
{
	int number_failed;
	Suite *s = websck_suite();
	SRunner *sr = srunner_create(s);

	/* run the test suite */
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;  return 0;
}
