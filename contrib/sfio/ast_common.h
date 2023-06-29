/* : : generated from features/common by iffe version 1998-01-23 : : */
#ifndef _AST_COMMON_H
#define _AST_COMMON_H	1
#define _hdr_stddef	1	/* #include <stddef.h> ok */
#define _sys_types	1	/* #include <sys/types.h> ok */
#define _hdr_time	1	/* #include <time.h> ok */
#define _sys_time	1	/* #include <sys/time.h> ok */
#define _sys_times	0	/* #include <sys/times.h> ok */
#define _hdr_stdlib	1	/* #include <stdlib.h> ok */
#define _typ_size_t	1	/* size_t is a type */
#define _typ_ssize_t	1	/* ssize_t is a type */
#define _typ_long_double	1	/* long double is a type */

/* __STD_C indicates that the language is ANSI-C or C++ */
#ifndef __STD_C
#ifdef __STDC__
#define	__STD_C		1
#else
#if __cplusplus || c_plusplus || __SC__ || __MRC__
#define __STD_C		1
#else
#define __STD_C		0
#endif /*__cplusplus*/
#endif /*__STDC__*/
#endif /*__STD_C*/

/* extern symbols must be protected against C++ name mangling */
#ifndef _BEGIN_EXTERNS_
#if __cplusplus || c_plusplus
#define _BEGIN_EXTERNS_	extern "C" {
#define _END_EXTERNS_	}
#else
#define _BEGIN_EXTERNS_
#define _END_EXTERNS_
#endif
#endif /*_BEGIN_EXTERNS_*/

/* _ARG_ simplifies function prototyping among flavors of C */
#ifndef _ARG_
#if __STD_C
#define _ARG_(x)	x
#else
#define _ARG_(x)	()
#endif
#endif /*_ARG_*/

/* __INLINE__ is the inline keyword */
#if !defined(__INLINE__) && defined(__cplusplus)
#define __INLINE__	inline
#endif
#if !defined(__INLINE__) && defined(_WIN32) && !defined(__GNUC__)
#define __INLINE__	__inline
#endif

/* Void_t is defined so that Void_t* can address any type */
#ifndef Void_t
#if __STD_C
#define Void_t		void
#else
#define Void_t		char
#endif
#endif /*Void_t*/

/* dynamic linked library external scope handling */
#undef extern
#define _astimport	extern

#	define __EXTERN__(T,obj)	extern T obj
#	define __DEFINE__(T,obj,val)	T obj = val

#	if __STD_C && _hdr_stddef
#	include	<stddef.h>
#	endif
#	include	<sys/types.h>

#define _ast_int1_t char
#define _ast_int2_t short
#define _ast_int4_t int
#define _ast_int8_t long long
#define _ast_intmax_t _ast_int4_t

#define _ast_flt4_t float
#define _ast_flt8_t double
#define _ast_flt12_t long double

#define _ast_fltmax_t _ast_flt12_t

/* CodeWarrior 5 defines va_copy, luckily in a compatible way */
#ifdef __MWERKS__
#include <stdarg.h>
#endif
#ifndef va_copy 
#define va_copy(to,fr) ((to)=(fr))	/* copy va_list fr -> to */
#endif

#endif
