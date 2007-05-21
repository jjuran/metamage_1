/* lzw.c -- compress files in LZW format.
 * This is a dummy version avoiding patent problems.
 */

/*
 * Changed: August 19, 1995. spd for MacGzip 1.0 (changed stderr)
 */
 
#ifdef RCSID
static char rcsid[] = "$Id$";
#endif

#include "tailor.h"
#include "gzip.h"
#include "lzw.h"

static int msg_done = 0;

/* Compress in to out with lzw method. */
int lzw(in, out)
    int in, out;
{
    if (msg_done) return ERROR;
    msg_done = 1;
#ifndef _MAC_ERRORS_H_ /* spd */
	fprintf(stderr,"output in compress .Z format not supported\n");
#else
	DoError(NO_ERR, WARN_ERR, "output in compress .Z format not supported");
#endif
    if (in != out) { /* avoid warnings on unused variables */
        exit_code = ERROR;
    }
    return ERROR;
}
