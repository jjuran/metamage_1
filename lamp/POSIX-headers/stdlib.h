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

#include <sys/cdefs.h>
#include <machine/_types.h>
#if __BSD_VISIBLE  /* for quad_t, etc. (XXX - use protected types) */
#include <sys/types.h>
#endif

/* in C++, wchar_t is a built-in type */
#if !defined(_WCHAR_T_DEFINED_) && !defined(__cplusplus)
#define _WCHAR_T_DEFINED_
typedef __wchar_t wchar_t;
#endif

#ifdef __MWERKS__
#include <div_t.h>
#if defined( __cplusplus )  &&  defined( _MSL_USING_NAMESPACE )
#if __MSL__ < 0x6000
using namespace std;
#else
using std::div_t;
using std::ldiv_t;
#endif
#endif
#else
typedef struct {
	int quot;  /* quotient */
	int rem;   /* remainder */
} div_t;

typedef struct {
	long quot;  /* quotient */
	long rem;   /* remainder */
} ldiv_t;
#endif

#if __ISO_C_VISIBLE >= 1999
typedef struct {
	long long quot;  /* quotient */
	long long rem;   /* remainder */
} lldiv_t;
#endif


#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include <sys/cdefs.h>

/*
 * Some header files may define an abs macro.
 * If defined, undef it to prevent a syntax error and issue a warning.
 */
#ifdef abs
#undef abs
//#warning abs macro collides with abs() prototype, undefining
#endif

__BEGIN_DECLS
__dead void abort(void);
int abs(int);
int atexit(void (*)(void));
double  atof(const char *);
int atoi(const char *);
long atol(const char *);
void* bsearch(const void *, const void *, size_t, size_t,
              int (*)(const void *, const void *));
void* calloc(size_t, size_t);
int clearenv( void );
div_t div(int, int);
__dead void exit(int);
__dead void _Exit(int);
void free(void *);
char* getenv(const char *);
long labs(long);
ldiv_t ldiv(long, long);
void* malloc(size_t);
void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
int rand(void);
void* realloc(void *, size_t);
void srand(unsigned);
double strtod(const char *, char **);
long strtol(const char *, char **, int);
unsigned long
 strtoul(const char *, char **, int);
int system(const char *);

#if __BSD_VISIBLE || __XPG_VISIBLE >= 400
int putenv(char *);
#endif

#if __BSD_VISIBLE || __XPG_VISIBLE >= 420
char* realpath(const char *, char *);
#endif /* __BSD_VISIBLE || __XPG_VISIBLE >= 420 */

/*
 * ISO C99
 */
#if __ISO_C_VISIBLE >= 1999
long long atoll(const char *);
long long llabs(long long);
lldiv_t lldiv(long long, long long);
long long strtoll(const char *, char **, int);
unsigned long long strtoull(const char *, char **, int);
#endif

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
