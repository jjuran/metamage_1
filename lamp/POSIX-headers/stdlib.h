/* $OpenBSD: stdlib.h,v 1.40 2007/09/03 14:37:02 millert Exp $ */
/* $NetBSD: stdlib.h,v 1.25 1995/12/27 21:19:08 jtc Exp $ */

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
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
 * @(#)stdlib.h 5.13 (Berkeley) 6/4/91
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#define abort  __MSL_abort
#define getenv __MSL_getenv
#define system __MSL_system

#include <CWANSIIncludes/stdlib.h>

#undef abort
#undef getenv
#undef system

#include <sys/cdefs.h>
#include <machine/_types.h>
#if __BSD_VISIBLE  /* for quad_t, etc. (XXX - use protected types) */
#include <sys/types.h>
#endif

#include <sys/cdefs.h>

__BEGIN_DECLS
__dead void abort(void);
int clearenv( void );
char* getenv(const char *);
int system(const char *);

#if __BSD_VISIBLE || __XPG_VISIBLE >= 400
int putenv(char *);
#endif

#if __BSD_VISIBLE || __XPG_VISIBLE >= 420
char* realpath(const char *, char *);
#endif /* __BSD_VISIBLE || __XPG_VISIBLE >= 420 */

/*
 * The Open Group Base Specifications, Issue 6; IEEE Std 1003.1-2001 (POSIX)
 */
#if __BSD_VISIBLE || __POSIX_VISIBLE >= 200112 || __XPG_VISIBLE >= 600
int setenv(const char *, const char *, int);
int unsetenv(const char *);
#endif

#if __BSD_VISIBLE
#include <alloca.h>
#endif /* __BSD_VISIBLE */

__END_DECLS

#endif /* _STDLIB_H_ */
