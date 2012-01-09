#ifndef	RELIX_SYS_FILE_H
#define	RELIX_SYS_FILE_H

#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8

#ifdef __cplusplus
extern "C"
{
#endif

int flock( int fd, int operation );

#ifdef __cplusplus
}
#endif

#endif

