/*
	sched.h
	-------
*/

#ifndef RELIX_SCHED_H
#define RELIX_SCHED_H

// POSIX
#include <sys/types.h>


#define CLONE_VM              0x00000100
#define CLONE_FS              0x00000200
#define CLONE_FILES           0x00000400
#define CLONE_SIGHAND         0x00000800
/* #define CLONE_PID */
#define CLONE_PTRACE          0x00002000
#define CLONE_VFORK           0x00004000
#define CLONE_PARENT          0x00008000
#define CLONE_THREAD          0x00010000
#define CLONE_NEWNS           0x00020000
#define CLONE_SYSVSEM         0x00040000
#define CLONE_SETTLS          0x00080000
#define CLONE_PARENT_SETTID   0x00100000
#define CLONE_CHILD_CLEARTID  0x00200000
#define CLONE_DETACHED        0x00400000
#define CLONE_UNTRACED        0x00800000
#define CLONE_CHILD_SETTID    0x01000000
#define CLONE_STOPPED         0x02000000

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __linux__

pid_t gettid();

#endif

int _relix_clone( int (*f)( void* ), void* stack_base, size_t stack_size, int flags, void* arg );

int unshare( int flags );

#ifdef __cplusplus
}
#endif

#endif
