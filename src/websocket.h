
#define WEBSCK_EAGAIN  -1
#define WEBSCK_SYS_ERR -2

#define WEBSCK_MAX_IN_BUFF  512
#define WEBSCK_MAX_OUT_BUFF 512


/* websocket buffer iterator */
struct websck_buff_iter {
  websck_buff *buff;
  int current;
  unsigned char state;
  unsigned char fin;
  unsigned char mask;
  size_t plen;
  unsigned char mkey[MASK_KEY_LEN];
};  

/* websocket ring buffer */
struct websck_buff {
  unsigned char *data;
  size_t size;    /* total size */
  size_t usesize; /* used size
  /* unsigned char *front; */
  /* unsigned char *end; */
  int begin; /* index to the begining of used space */
  int end;   /* index to the end of used space */
  websck_buff_iter iter; /* buffer iterator */
};

/* websocket server data */
struct websck_srv_data {
  struct websck_buff *inbuff;
  struct websck_buff *outbuff;
  int sock;
  /* unsigned int clinum; */
  /* struct linklist *clients;  */
};

/* websocket client data */
struct websck_cli_data {
  struct websck_buff *inbuff;
  struct websck_buff *outbuff;
  int sock;
};

/* pointers to socket functions */
extern ssize_t (*neti_recv)(int socket, void *buffer, size_t size, int flags);
extern ssize_t (*neti_send)(int socket, const void *buffer, size_t size, int flags);
extern ssize_t (*neti_accept)(int socket, struct sockaddr *addr, socklen_t *lenp);

/* websocket functions */
int websck_init_lib(void);
int websck_bind(const char* addr, uint16_t port);
int websck_connect(const char* addr, uint16_t port);
int websck_close(int socket);
int websck_recv(int socket, void *buffer, size_t size, int flags);
int websck_send(int socket, void *buffer, size_t size, int flags); 
int websck_accept(int socket, struct sockaddr *addr, socklen_t *lenp);
int websck_proc_cli_hndshk(int socket, struct websck_hndshk *hndp);
int websck_recv_hndshk(int socket, char *buff, size_t size);
struct websck_cli_data* websck_create_cli_data(int clisck);
struct websck_srv_data* websck_create_srv_data(int srvsck);
void websck_add_srv(struct websck_srv_data *srvdata);
void websck_init_srv_data(struct websck_srv_data *srvdata);
void websck_init_buff(struct websck_buff *buff);
struct websck_srv_data* websck_get_srv_data(int srvsck);
void websck_add_cli(struct websck_cli_data *clidata);
void websck_init_cli_data(struct websck_cli_data *clidata);
void websck_set_default_cli_data(struct websck_cli_data *clidata, int clisck);
