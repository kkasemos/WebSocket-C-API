#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "util.h"
#include "handshake.h"
#include "websocket.h"
#include "dataframe.h"

#define MAX_BUFF_LEN 1024

struct linklist websck_srv_list;
struct linklist websck_cli_list;

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
	struct websck_srv_data *srvdata;

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

	/* create websocket server data */
	srvdata = websck_create_srv_data(sock);

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
	struct websck_cli_data clidata;

	/* get the clinet's buffer by the socket */
	clidata = websck_get_cli(socket);

	/* operate as websocket state */
	switch(clidata->state) {
	case WEBSCK_RECV_PAYLOAD:
		/* receving the payload */
		websck_buff_recv_pload(clidata->inbuff);

		/* all the payload received? */
		if(websck_buff_pload_recv(clidata->inbuff)) {
			clidata->state = WEBSCK_READY;
			/* reset */
		}
		break;
	case WEBSCK_READY:
		/* ready to read the next frame */
		if(websck_buff_is_empty(cli->inbuff)) {
			/* receive data from the socket */
		}

		/* iterate the frame in the buffer */
		websck_buff_recv_frm(cli->inbuff);

		break;
	case WEBSCK_RECV_FRAME:
		/* receving the frame */
		break;
		
	}

	/* end of payload? */
	if(!websck_buff_is_eop(clidata->inbuff)) {
		psize = websck_buff_recv_pload(buffer, size, clidata->inbuff);
		
		/* could copy all? */
		if() {
			websck_buff_reset_iter(clidata->inbuff);
		}
	}

	if(!websck_buff_is_empty(clidata->inbuff)) {
		fsize = websck_buff_recv_frm(socket, clidata->inbuff);
		
		/* frame is complete */
		if(fsize > 0) {
			/* copy frame's payload */
			csize = websck_buff_recv_pload(buffer, size, clidata->inbuff);

			/* could copy all? */
			if(csize == size) {
				websck_buff_reset_iter(clidata->inbuff);
			} 
				
			
		}
	}

	/* read websocket frame to the buffer if there is space available */
	if(!websck_buff_is_full(clidata->inbuff)) {
		/* if there is space available at the end */
		esize = websck_buff_end_size(clidata->inbuff);
		rb = neti_recv(socket, clidata->data + clidata->end, 
			       endsize, flags);

		/* if there is space available at the begining */
		bsize = websck_buff_begin_size(clidata->inbuff);
		rb = neti_recv(socket, clidata->data + , 
			       clidata->size - clidata->usesize, flags);
	}
	
	rb = neti_recv(socket, frm, size, flags);
	/* print_hex(frm, rb); */

	if(rb == -1) {
		/* if no data in the socket in the case of non-blocking I/O */
		if(errno == EAGAIN || errno == EWOULDBLOCK) {
			errno = WEBSCK_AGAIN;
		} else {
			errno = WEBSCK_SYS_ERR;
		}
		return -1;
	}

	/* verify if the frame is complete */
	if(!datfrm_is_complete(frm)) {
		/* threat it as no data in the socket in the case of
		   non-blocking I/O */ 
		errno = WEBSCK_AGAIN;
		return -1;
	}
	
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

struct websck_srv_data *websck_get_srv_data(int srvsck)
{
	return 0;
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
		//struct websck_srv_data *srvdata;
		struct websck_cli_data *clidata;

		hndshk_gen_srv(&clihnd, &srvhnd);
		websck_send_srv_hndshk(clisck, &srvhnd);
		
		clidata = websck_create_cli_data(clisck);
		//srvdata = websck_get_srv_data(socket);
		websck_add_cli(clidata);
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
	int hsize = 0; /* handshake size */
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
			hsize++;
		}

		if(*rbp == '\n') {
			*rtp = *rbp;
			rbp++;
			rtp++;
			hsize++;
		}

		/* second CRLF */
		if(*rbp == '\r') {
			*rtp = *rbp;
			rbp++;
			rtp++;
			hsize++;
		}

		if(*rbp == '\n') {
			*rtp = *rbp;
			rbp++;
			rtp++;
			hsize++;
			break;
		}

		*rtp = *rbp;

		rbp++;
		rtp++;
		hsize++;
	}
	//printf("%*s\n", n, rb);
	//return (rtp >= rtep)? -1:(rtp - buff);
	return (rtp >= rtep)? -1:hsize;
}

int websck_parse_cli_hndshk(const char *buff, ssize_t size, struct websck_hndshk *hndp)
{
	hndshk_init(hndp);
	hndshk_parse(buff, size, hndp);
}

int websck_init_lib(void)
{
	neti_recv = recv;
	neti_send = send;
	neti_accept = accept;
}

struct websck_srv_data *websck_get_srv(int srvsck)
{
	struct websck_srv_data *srvdata = NULL;

	/* iterate the global server list */
	struct linkiter *iter = linkiter_create(&websck_srv_list);
	int found = 0;
	while(!linkiter_eol(iter) && !found)
	{
		struct linknode *node = linkiter_next(iter);
		srvdata = (struct websck_srv_data*)node->data;
		
		if(srvdata->sock == srvsck)
			found = 1;	    
	}

	return srvdata;
}

void websck_add_cli(struct websck_cli_data *clidata)
{
	linklist_add(&websck_cli_list, clidata);
}

void websck_add_srv(struct websck_srv_data *srvdata)
{
	linklist_add(&websck_srv_list, srvdata);
}

struct websck_buff *websck_create_buff(size_t size)
{
	struct websck_buff *buff = 
		(struct websck_buff*)malloc(sizeof(struct websck_buff));

	buff->data = (unsigned char*)malloc(sizeof(unsigned char)*size);
	buff->size = size;

	return buff;
}

void websck_init_srv_data(struct websck_srv_data *srvdata)
{
	srvdata->inbuff = (struct websck_buff*)0;
	srvdata->outbuff = (struct websck_buff*)0;
	srvdata->sock = -1;
	/* srvdata->clinum = 0; */
	/* srvdata->clients = (struct linklist*)0; */
}

void websck_set_default_srv_data(struct websck_srv_data *srvdata, int srvsck)
{
	srvdata->inbuff = websck_create_buff(WEBSCK_MAX_IN_BUFF);
	srvdata->outbuff = websck_create_buff(WEBSCK_MAX_OUT_BUFF);
	srvdata->sock = srvsck;
	/* srvdata->clients = linklist_create(); */
}

struct websck_srv_data *websck_create_srv_data(int srvsck)
{
	struct websck_srv_data *srvdata;

	srvdata = (struct websck_srv_data*)malloc(sizeof(struct websck_srv_data));
	websck_init_srv_data(srvdata);

	/* set default value of the server data */
	websck_set_default_srv_data(srvdata, srvsck);

	/* add the server data to the global list */
	websck_add_srv(srvdata);

	return srvdata;
}

struct websck_cli_data *websck_create_cli_data(int clisck)
{
	struct websck_cli_data *clidata;

	clidata = 
		(struct websck_cli_data*)malloc(sizeof(struct websck_cli_data));

	websck_init_cli_data(clidata);
	websck_set_default_cli_data(clidata, clisck);
	websck_add_cli(clidata);

	return clidata;
}

void websck_init_cli_data(struct websck_cli_data *clidata)
{
	clidata->inbuff  = (struct websck_buff*)0;
	clidata->outbuff = (struct websck_buff*)0;
	clidata->sock    = -1;
}

void websck_set_default_cli_data(struct websck_cli_data *clidata, int clisck)
{
	clidata->inbuff  = websck_create_buff(WEBSCK_MAX_IN_BUFF);
	clidata->outbuff = websck_create_buff(WEBSCK_MAX_OUT_BUFF);
	clidata->sock    = clisck;
}
