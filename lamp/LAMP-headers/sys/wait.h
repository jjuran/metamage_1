/*	==========
 *	sys/wait.h
 *	==========
 */

#ifndef LAMP_SYS_WAIT_H
#define LAMP_SYS_WAIT_H

#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

pid_t wait( int* stat_loc );

pid_t waitpid( pid_t pid, int* stat_loc, int options );

#ifdef __cplusplus
}
#endif

#endif

