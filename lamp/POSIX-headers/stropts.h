/*
	stropts.h
	---------
*/

#ifndef _STROPTS_H_
#define _STROPTS_H_

// POSIX
#include <sys/types.h>


#define I_SENDFD  (('S' << 8) | 17)
#define I_RECVFD  (('S' << 8) | 14)

#ifdef __cplusplus
extern "C" {
#endif

struct strbuf
{
	int    maxlen;
	int    len;
	char*  buf;
};

struct strrecvfd
{
	int    fd;
	uid_t  uid;
	gid_t  gid;
	char   fill[ 4 ];
};

int fattach( int, const char* );
int fdetach( const char* );
int getmsg( int, struct strbuf*, struct strbuf*, int* );
int getpmsg( int, struct strbuf*, struct strbuf*, int*, int* );
int ioctl( int, int, ... );
int isastream( int );
int putmsg( int, const struct strbuf*, const struct strbuf*, int );
int putpmsg( int, const struct strbuf*, const struct strbuf*, int, int );

#ifdef __cplusplus
}
#endif

#endif /* _STROPTS_H_ */
