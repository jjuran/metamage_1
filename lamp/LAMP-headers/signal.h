/*	========
 *	signal.h
 *	========
 */

#ifndef KEROSENE_SIGNAL_H
#define KEROSENE_SIGNAL_H

#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void ( *sig_t )( int );

int kill( pid_t pid, int sig );

sig_t signal( int sig, sig_t func );

#ifdef __cplusplus
}
#endif

#endif

