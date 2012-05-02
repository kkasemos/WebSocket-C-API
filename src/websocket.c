#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "handshake.h"
#include "websocket.h"
#include "dataframe.h"

#define MAX_BUFF_LEN 1024

ssize_t (*neti_recv)(int socket, void *buffer, size_t size, int flags);
ssize_t (*neti_send)(int socket, const void *buffer, size_t size, int flags);
ssize_t (*neti_accept)(int socket, struct sockaddr *addr, socklen_t *lenp);

int websck_init_sockaddr(struct sockaddr_in *name, const char *hostname, uint16_t port)
{
  struct hostent *hostinfo;  

  /* give the socket a name */
  name->sin_family = AF_INET;
  name->sin_port = htons(port);
  hostinfo = gethostbyname(hostname);

  if(hostinfo == NULL)
  {
    fprintf(stderr, "Unknow host %s.\n", hostname);
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
  }

  websck_init_sockaddr(&name, hostname, port);
  if(bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0)
  {
    perror("Failed to bind the socket");
  }

  return sock;
}

int websck_connect(const char* hostname, uint16_t port)
{
  struct sockaddr_in serv_name;
  int sock = socket(PF_INET, SOCK_STREAM, 0);
  int rs;

  websck_init_sockaddr(&serv_name, hostname, port);

  if(connect(sock, (struct sockaddr*)&serv_name, sizeof(serv_name)) < 0)
  {
    //  perror("Failed to connect the client socket");
    return -1;
  }

  return sock;
}

int websck_shutdown(int socket, int how)
{
  if(shutdown(socket, how) < 0)
  {
    return -1;
  }
}

int websck_close(int socket)
{
  if(close(socket) < 0)
  {
    perror("Failed to close the socket");
    return -1;
  }
}

int websck_recv(int socket, void *buffer, size_t size, int flags)
{
	unsigned char fin = 0, mask = 0;
	unsigned char mkey[MASK_KEY_LEN];
	unsigned char frm[MAX_BUFF_LEN];
	size_t plen; /* payload length */
	int    rb;   /* received bytes */

	rb = recv(socket, frm, size, flags);
	print_hex(frm, rb);

	/* get FIN flag */
	fin = datfrm_get_fin(frm);
	
	/* /\* if FIN flag is 0 means the frame is fragmented *\/ */
	/* while(0 == fin) { */
	/* 	/\* read more frames *\/ */
	/* 	rb = recv(socket, frm, size, flags);	 */
	/* } */

	/* get MASK flag */
	mask = datfrm_get_mask(frm);
	/* printf("mask %d\n", mask); */

	/* get payload length */
	plen = datfrm_get_payload_len(frm);
	/* printf("payload lenght %d\n", plen); */

	if(mask) {
		/* get mask key */
		datfrm_get_mask_key(frm, plen, mkey);
		/* printf("mask key %x %x %x %x\n", */
		/*        *mkey, *(mkey+1), *(mkey+2), *(mkey+3)); */
	}

	/* get payload */
	datfrm_get_payload(frm, mask, plen, buffer);

	if(mask) {
		/* unmask payload */
		datfrm_unmask_payload(buffer, plen, mkey);
	}

	return plen;
}

int websck_send(int socket, void *buffer, size_t size, int flags)
{
	int bs; /* sent bytes */
	size_t flen;  /* frame lenght */
	unsigned char *frm;

	frm = datfrm_create_txt(buffer, size, MASK_ON, &flen); 
	bs = neti_send(socket, frm, flen, flags);

	return bs;
}

int websck_accept(int socket, struct sockaddr *addr, socklen_t *lenp)
{
	int clisck;
	int r;
	struct websck_hndshk clihnd;
	struct websck_hndshk srvhnd;

	clisck = neti_accept(socket, addr, lenp);
	if(clisck < 0) {
		return -1;
	}

	/* WebScoket protocol requires the connection handshake */
	r = websck_recv_cli_hndshk(clisck, &clihnd);
	if(r < 0) {
		return -1;
	} else {
		hndshk_gen_srv(&clihnd, &srvhnd);
		websck_send_srv_hndshk(clisck, &srvhnd);
	}
	return clisck;
}

int websck_send_srv_hndshk(int socket, struct websck_hndshk *hndp)
{
	char buff[MAX_BUFF_LEN];

	hndshk_sprintf(buff, hndp);
	websck_send_hndshk(socket, buff, strlen(buff));
}

int websck_recv_cli_hndshk(int socket, struct websck_hndshk *hndp)
{
	char buff[MAX_BUFF_LEN];
	int n; /* number of bytes read/written */
	int r; /* handshak parsing result */
  
	if((n = websck_recv_hndshk(socket, buff, MAX_BUFF_LEN)) < 0)
	{
		return -1;
	}

	r = websck_parse_cli_hndshk(buff, n, hndp);  

	return r;
}

int websck_send_hndshk(int socket, char *buff, size_t size)
{
	int n;

	n = neti_send(socket, buff, size, 0);

	return n;
}

int websck_recv_hndshk(int socket, char *buff, size_t size)
{
	int n;
	char rb[MAX_BUFF_LEN];
	char *rbp, *rbep;  /* pointer to read buffer */
	char *rtp, *rtep;  /* pointer to return buffer */

	/* read data from the socket */
	n = neti_recv(socket, rb, MAX_BUFF_LEN, 0);

	/* setup current and end pointer to the return buffer */
	rtp = buff;
	rtep = buff + size;

	/* setup current and end pointer to the read buffer */
	rbp = rb;
	rbep = rb + n;

	/* read until found the sequence of two CRLF */
	while(rtp < rtep && rbp < rbep) {
		/* first CRLF */
		if(*rbp == '\r') {
			*rtp = *rbp;
			rbp++;
			rtp++;
		}
		if(*rbp == '\n') {
			*rtp = *rbp;
			rbp++;
			rtp++;
		}

		/* second CRLF */
		if(*rbp == '\r') {
			*rtp = *rbp;
			rbp++;
			rtp++;
		}
		if(*rbp == '\n') {
			*rtp = *rbp;
			rbp++;
			rtp++;
			break;
		}

		*rtp = *rbp;

		rbp++;
		rtp++;
	}
	//printf("%*s\n", n, rb);
	return (rtp >= rtep)? -1:(rtp - buff);
}

int websck_parse_cli_hndshk(const char *buff, ssize_t size, struct websck_hndshk *hndp)
{
	hndshk_init(hndp);
	hndshk_parse(buff, size, hndp);
}

int websck_init_lib(void)
{
  //#ifdef WEBSCK_TEST
  neti_recv = recv;
  neti_send = send;
  neti_accept = accept;
  //#else
  //platf_recv = test_recv;
  //#endif /* WENSCK_TEST */
}
/*
int websck_send_srv_handshk(int clifd, struct websck_handshake *hndp, int len)
{
  int srvhlen;
  char *srvhp;

  websck_create_srv_handshk(buff, len, hndp, &hndlen) 
  websck_send(clifd, buff, len, 0);  
}

int websck_srv_handshk(
*/
