/*	============
 *	sys/socket.h
 *	============
 */

#ifndef KEROSENE_SYS_SOCKET_H
#define KEROSENE_SYS_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int socklen_t;

struct sockaddr
{
	unsigned short sa_family;
	unsigned char  sa_data[ 14 ];
};

#define PF_INET 2
#define SOCK_STREAM 1

typedef unsigned short sa_family_t;
typedef unsigned short in_port_t;

struct sockaddr_in
{
	sa_family_t		sin_family;
	in_port_t		sin_port;
	unsigned long	sin_addr;
	unsigned char	sin_zero[8];
};

int socket( int domain, int type, int protocol );

int bind( int sockfd, const struct sockaddr* name, socklen_t namelen );

int listen( int sockfd, int backlog );

int accept( int sockfd, struct sockaddr* addr, socklen_t* addrlen );

int connect( int sockfd, const struct sockaddr* serv_addr, socklen_t addrlen );

int getsockname( int sockfd, struct sockaddr* name, socklen_t* namelen );
int getpeername( int sockfd, struct sockaddr* name, socklen_t* namelen );

int shutdown( int s, int how );

#ifdef __cplusplus
}
#endif

#endif

