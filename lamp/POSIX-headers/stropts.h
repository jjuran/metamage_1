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

struct strrecvfd
{
	int    fd;
	uid_t  uid;
	gid_t  gid;
	char   fill[ 4 ];
};

#ifdef __cplusplus
}
#endif

#endif /* _STROPTS_H_ */
