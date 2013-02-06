#ifndef _STROPTS_H_
#define _STROPTS_H_

#define I_SENDFD  (('S' << 8) | 17)
#define I_RECVFD  (('S' << 8) | 14)

#ifdef __cplusplus
extern "C" {
#endif

struct strrecvfd
{
	int             fd;
	unsigned short  uid;
	unsigned short  gid;
	char            fill[ 8 ];
};

#ifdef __cplusplus
}
#endif

#endif /* _STROPTS_H_ */

