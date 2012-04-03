#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "websocket.h"

int websck_init_sockaddr(struct sockaddr_in *name, const char *hostname, uint16_t port)
{
  struct hostent *hostinfo;  

  /* give the socket a name */
  name->sin_family      = AF_INET;
  name->sin_port        = htons(port);
  hostinfo = gethostbyname(hostname);

  if(hostinfo == NULL)
  {
    fprintf(stderr, "Unknow host %s.\n", hostname);
    //exit(EXIT_FAILURE);
  }

  name->sin_addr = *(struct in_addr *)hostinfo->h_addr;
}

int websck_bind(const char* hostname, uint16_t port)
{
  int sock;
  struct sockaddr_in name;
  struct hostent *hostinfo;

  /* create the socket */
  sock = socket(PF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    perror("could not create the socket");
    //exit(EXIT_FAILURE);
  }

  /* give the socket a name */
  name.sin_family      = AF_INET;
  name.sin_port        = htons(port);
  hostinfo = gethostbyname(hostname);

  if(hostinfo == NULL)
  {
    fprintf(stderr, "Unknow host %s.\n", hostname);
    //exit(EXIT_FAILURE);
  }

  name.sin_addr = *(struct in_addr *)hostinfo->h_addr;
  if(bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0)
  {
    perror("Failed to bind the socket");
    //exit(EXIT_FAILURE);
  }

  return sock;
}

int websck_connect(const char* hostname, uint16_t port)
{
  struct sockaddr_in serv_name;
  int sock = socket(PF_INET, SOCK_STREAM, 0);
  int rs;

  /* give the socket a name */
  /* name.sin_family      = AF_INET; */
  /* name.sin_port        = htons(port); */
  /* name.sin_addr.s_addr = htonl(addr); */

  websck_init_sockaddr(&serv_name, hostname, port);

  if(connect(sock, (struct sockaddr*)&serv_name, sizeof(serv_name)) < 0)
  {
    perror("Failed to connect the client socket");
    //exit(EXIT_FAILURE);
  }

  return sock;
}

int websck_close(int socket)
{
  /* if(shutdown(socket, SHUT_RD) < 0) */
  /* { */
  /*   perror("Failed to shutdown the socket"); */
  /*   return -1; */
  /* } */

  if(close(socket) < 0)
  {
    perror("Failed to close the socket");
    return -1;
  }
}
/* int websck_bind(int socket, struct sockaddr *addr, socklen_t length) */
/* { */
/*   int rs; */

/*   rs = bind(socket, addr, sizeof(addr)); */

/*   /\* failure *\/ */
/*   if(rs) */
/*   { */
/*   } */

/*   return -1; */
/* }  */

int websck_recv(int socket, void *buffer, size_t size, int flags)
{
  int s = recv(socket, buffer, size, flags);
  return s;
}

int websck_send(int socket, void *buffer, size_t size, int flags)
{
  int s = send(socket, buffer, size, flags);
  return s;
}

int websck_accept(int socket, struct sockaddr *addr, socklen_t *length_ptr)
{
  int rs;

  rs = accept(socket, addr, length_ptr);

  if(rs < 0)
  {
    perror("Failed to accept the client websocket");
    //exit(EXIT_FAILURE);
  }

  return rs;
}
