
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
