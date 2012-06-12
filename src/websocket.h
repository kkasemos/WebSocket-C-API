
#define WEBSCK_EAGAIN  -1
#define WEBSCK_SYS_ERR -2

#define WEBSCK_MAX_IN_BUFF  512
#define WEBSCK_MAX_OUT_BUFF 512

/* websocket ring buffer */
struct websck_buff {
  unsigned char *data;
  size_t size;    /* total size */
  size_t usesize; /* used size */
  int begin; /* index to the begining of used space */
  int end;   /* index to the end of used space */
};

/* websocket buffer iterator */
struct websck_buff_iter {
  struct websck_buff *buff;
  int current;
};  

/* websocket server data */
struct websck_srv_data {
  struct websck_buff *inbuff;
  struct websck_buff *outbuff;
  int sock;
};

/* websocket client data */
struct websck_cli_data {
  struct websck_buff *inbuff;
  struct websck_buff *outbuff;
  struct websck_buff_iter initer;  /* input buffer iterator */
  struct websck_buff_iter outiter; /* output buffer iterator */
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
struct websck_srv_data* websck_get_srv_data(int srvsck);

void websck_add_srv(struct websck_srv_data *srvdata);
void websck_init_srv_data(struct websck_srv_data *srvdata);
void websck_set_default_srv_data(struct websck_srv_data *srvdata, int srvsck);

void websck_add_cli(struct websck_cli_data *clidata);
void websck_init_cli_data(struct websck_cli_data *clidata);
void websck_set_default_cli_data(struct websck_cli_data *clidata, int clisck);

/* websocket buffer functions */
void websck_buff_init(struct websck_buff *buff);
int websck_buff_recv_pload(struct websck_buff *buff, struct websck_buff_iter *iter);
int websck_buff_is_eop(struct websck_buff_iter *iter);
void websck_buff_reset_iter(struct websck_buff_iter *iter);
int websck_buff_is_empty(struct websck_buff *buff);
int websck_buff_recv_frm(struct websck_buff *buff, struct websck_buff_iter *iter);
