//#include <sys/socket.h>
int websck_bind(const char* addr, uint16_t port);
int websck_connect(const char* addr, uint16_t port);
int websck_close(int socket);
/* int websck_bind(int socket, struct sockaddr *addr, socklen_t length); */
int websck_recv(int socket, void *buffer, size_t size, int flags);
int websck_send(int socket, void *buffer, size_t size, int flags); 
int websck_accept(int socket, struct sockaddr *addr, socklen_t *length_ptr);
