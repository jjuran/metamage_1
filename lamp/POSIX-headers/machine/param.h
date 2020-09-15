#ifndef _MACHINE_PARAM_H_
#define _MACHINE_PARAM_H_

#define ALIGNBYTES 3

#define ALIGN( p )  ((long) p + ALIGNBYTES & ~ALIGNBYTES)

#endif
