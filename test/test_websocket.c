#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>
#include <check.h>
#include "websocket.h"

#define SERV_HOSTNAME "localhost"
#define SERV_PORT 8081
#define MAXMSG 512

pthread_t cli_thread_id;

void *client_websck_test(void *arg)
{
  int cli_fd;
  uint16_t port = SERV_PORT;
  char buffer[MAXMSG];
  int bytes;

  printf("Client websocket started\n");
  
  cli_fd = websck_connect(SERV_HOSTNAME, port);
  
  if(cli_fd < 0)
  {
    //fprintf(stderr, "Failed to connect the client websocket\n");
    perror("Failed to connect to the client websocket");
  }

  if(websck_send(cli_fd, "hello world", 10, 0) < 0)
  {
    perror("Failed to send the 'hello world'");
    //fprintf(stderr, "Failed to send the 'hello world'\n");
  }

  if((bytes = websck_recv(cli_fd, buffer, MAXMSG, 0)) < 0)
  {
    perror("Failed to receive the server response");
  }
  else
  {
    printf("Received %*s from the server\n", bytes, buffer); 
  }
}

void  start_client_websck_test_thread(const char *host, uint16_t port)
{
  pthread_attr_t thrd_attr;
  int arg = 1;

  pthread_attr_init(&thrd_attr);
  pthread_create(&cli_thread_id, &thrd_attr, client_websck_test, &arg);
  pthread_attr_destroy(&thrd_attr);
}

START_TEST(test_websck_integr_success)
{
  int serv_fd, cli_fd;
  fd_set active_fd_set, read_fd_set;
  int success;
  struct sockaddr cli_addr;
  socklen_t cli_addr_len;
  char buffer[MAXMSG];
  int flags;
  ssize_t bytes_read;
  int err;
  ssize_t bytes_send;
  struct timeval timeout;
  uint16_t port; 

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
  cli_fd = websck_accept(serv_fd, &cli_addr, &cli_addr_len);
  fail_unless(cli_fd > -1, "Failed to accept the client websocket %d");

  /* receive the socket */
  flags = 0;
  bytes_read = websck_recv(cli_fd, buffer, MAXMSG, flags);
  fail_unless(bytes_read > -1, "Failed to receive the client websocket");

  /* send the socket */ 
  bytes_send = websck_send(cli_fd, buffer, bytes_read, flags); 
  fail_unless(bytes_send > -1, "Failed to send the data to the client websocket");
  pthread_join(cli_thread_id, NULL);  

  websck_close(cli_fd);
  websck_close(serv_fd);
  // fail_unless(sock_fd >= 0, "Create websocket failed");
}
END_TEST


START_TEST(test_websck_bind_success)
{
  int sock_fd;

  sock_fd = websck_bind("localhost", 8081);
  websck_close(sock_fd);
  fail_unless(sock_fd > -1, "Failed to create websocket");
}
END_TEST

/*
START_TEST(test_websck_bind_success)
{
  int             websck_fd;
  struct sockaddr *serv_addr;
  socklen_t       serv_addr_len;
  int             result; 
  
  result = websck_bind(websck_fd, serv_addr, serv_addr_len);
  
  fail_unless(result == 0, "Bind websocket failed");
}
END_TEST
*/

Suite *websck_suite(void)
{
  Suite *s = suite_create("websocket");
  TCase *tc;
 
 /* socket test case */
  tc = tcase_create("socket");
  tcase_add_test(tc, test_websck_bind_success);
  suite_add_tcase(s, tc);
  
  /* bind test case */
  /*
  tc = tcase_create("bind");
  tcase_add_test(tc, test_websck_bind_success);
  suite_add_tcase(s, tc);
  */

  /* integration test case */
  tc = tcase_create("integration");
  tcase_add_test(tc, test_websck_integr_success);
  suite_add_tcase(s, tc);
  
  return s;
}

int main(void)
{
  int number_failed;
  Suite *s = websck_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;  return 0;
}
