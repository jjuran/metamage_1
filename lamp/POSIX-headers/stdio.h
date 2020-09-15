/*	$OpenBSD: stdio.h,v 1.35 2006/01/13 18:10:09 miod Exp $	*/
/*	$NetBSD: stdio.h,v 1.18 1996/04/25 18:29:21 jtc Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)stdio.h	5.17 (Berkeley) 6/3/91
 */

#ifndef	_STDIO_H_
#define	_STDIO_H_

#include <CWANSIIncludes/stdio.h>

#include <sys/cdefs.h>

#include <stdarg.h>

#include <sys/types.h>

#define	_FSTDIO			/* Define for new stdio with functions. */

#ifndef SEEK_SET
#define	SEEK_SET	0	/* set file offset to offset */
#endif
#ifndef SEEK_CUR
#define	SEEK_CUR	1	/* set file offset to current plus offset */
#endif
#ifndef SEEK_END
#define	SEEK_END	2	/* set file offset to EOF plus offset */
#endif

#if __BSD_VISIBLE && !defined(__SYS_ERRLIST)
#define __SYS_ERRLIST

extern int sys_nerr;			/* perror(3) external variables */
extern const char *sys_errlist[];
#endif


/*
 * Functions defined in POSIX 1003.1.
 */
#if __BSD_VISIBLE || __POSIX_VISIBLE || __XPG_VISIBLE
#define	L_ctermid	1024	/* size for ctermid(); PATH_MAX */
#define L_cuserid	9	/* size for cuserid(); UT_NAMESIZE + 1 */

__BEGIN_DECLS
int fileno( FILE *stream );

FILE* fdopen( int fd, const char* type );

int renameat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath );

char	*ctermid(char *);
char	*cuserid(char *);

#if __POSIX_VISIBLE >= 199209
int	 pclose(FILE *);
FILE	*popen(const char *, const char *);
#endif

#if __POSIX_VISIBLE >= 199506
void	 flockfile(FILE *);
int	 ftrylockfile(FILE *);
void	 funlockfile(FILE *);

/*
 * These are normally used through macros as defined below, but POSIX
 * requires functions as well.
 */
int	 getc_unlocked(FILE *);
int	 getchar_unlocked(void);
int	 putc_unlocked(int, FILE *);
int	 putchar_unlocked(int);
#endif /* __POSIX_VISIBLE >= 199506 */
__END_DECLS

#ifndef _POSIX_THREADS
#  define flockfile(fp)			/* nothing */
#  define ftrylockfile(fp)		(0)
#  define funlockfile(fp)		/* nothing */
#endif

#endif /* __BSD_VISIBLE || __POSIX_VISIBLE || __XPG_VISIBLE */

/*
 * Routines that are purely local.
 */
#if __BSD_VISIBLE
__BEGIN_DECLS
int	 asprintf(char **, const char *, ...)
		__attribute__((__format__ (printf, 2, 3)))
		__attribute__((__nonnull__ (2)));
int	 vasprintf(char **, const char *, __va_list)
		__attribute__((__format__ (printf, 2, 0)))
		__attribute__((__nonnull__ (2)));
__END_DECLS
#endif /* __BSD_VISIBLE */

#endif /* _STDIO_H_ */
