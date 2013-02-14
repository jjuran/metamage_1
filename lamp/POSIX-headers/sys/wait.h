/*
	sys/wait.h
	----------
*/

#ifndef RELIX_SYS_WAIT_H
#define RELIX_SYS_WAIT_H

// POSIX
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WNOHANG    1
#define WUNTRACED  2

#define __WTHREAD  0x80000000

#define WTERMSIG(stat)     ( (stat) & 0x7f )

#define WCOREDUMP(stat)    ( (stat) & 0x80 )

#define WEXITSTATUS(stat)  ( ((stat) >> 8) & 0xFF )

#define WIFSIGNALED(stat)  ( WTERMSIG(stat) != 0 && WTERMSIG(stat) != 0x7F )

#define WIFEXITED(stat)    ( WTERMSIG(stat) == 0 )

#define WIFSTOPPED(stat)   ( WTERMSIG(stat) == 0x7F )

pid_t wait( int* stat_loc );

pid_t waitpid( pid_t pid, int* stat_loc, int options );

#ifdef __cplusplus
}
#endif

#endif

