#ifndef _SFHDR_H
#define _SFHDR_H	1
#ifndef _BLD_sfio
#define _BLD_sfio	1
#endif

/*	Internal definitions for sfio.
**	Written by Kiem-Phong Vo (07/16/90)
**	AT&T Labs
*/

#include	"features/sfio-mac.h"
#include	"sfio_t.h"

/* avoid conflict with BSDI's SF_APPEND */
#undef SF_APPEND

/* file system info */
#if _PACKAGE_ast

#include	<ast.h>
#include	<ast_time.h>
#include	<ast_tty.h>
#include	<ls.h>

#if _mem_st_blksize_stat
#define _stat_blksize	1
#endif

#if _lib_localeconv && _hdr_locale
#define _lib_locale	1
#endif

#else /*!PACKAGE_ast*/

#if _hdr_stdlib
#include	<stdlib.h>
#endif

#if __STD_C
#include	<string.h>
#endif

#if _hdr_time
#include	<time.h>
#endif
#if _sys_time
#include	<sys/time.h>
#endif

#if _sys_stat
#include	<sys/stat.h>
#else
#if _hdr_stat
#include	<stat.h>
#ifndef _sys_stat
#define	_sys_stat	1
#endif
#endif
#endif /*_sys_stat*/

#ifndef _sys_stat
#define _sys_stat	0
#endif

#include	<fcntl.h>

#ifndef F_SETFD
#ifndef FIOCLEX
#if _hdr_filio
#include	<filio.h>
#else
#if _sys_filio
#include	<sys/filio.h>
#endif /*_sys_filio*/
#endif /*_hdr_filio*/
#endif /*_FIOCLEX*/
#endif /*F_SETFD*/

#if _hdr_unistd
#include	<unistd.h>
#endif

#endif /*_PACKAGE_ast*/

#include	<errno.h>
#include	<ctype.h>

#if _lib_select
#undef _lib_poll
#else
#if _lib_poll_fd_1 || _lib_poll_fd_2
#define _lib_poll	1
#endif
#endif /*_lib_select_*/

#if _lib_poll
#include	<poll.h>

#if _lib_poll_fd_1
#define SFPOLL(pfd,n,tm)	poll((pfd),(ulong)(n),(tm))
#else
#define SFPOLL(pfd,n,tm)	poll((ulong)(n),(pfd),(tm))
#endif
#endif /*_lib_poll*/

#if _stream_peek
#include	<stropts.h>
#endif

#if _socket_peek
#include	<sys/socket.h>
#endif

#ifndef X_OK	/* executable */
#define X_OK	01
#endif

#if _lib_vfork && !defined(sparc) && !defined(__sparc)
#if _hdr_vfork
#include	<vfork.h>
#endif
#if _sys_vfork
#include	<sys/vfork.h>
#endif
#define fork	vfork
#endif

#if _lib_unlink
#define remove	unlink
#endif

#if _hdr_floatingpoint
#include	<floatingpoint.h>
#endif
#if _hdr_values
#include	<values.h>
#endif
#if _hdr_math
#include	<math.h>
#endif

#if !_ast_fltmax_double
#if !defined(MAXDOUBLE) && defined(DBL_MAX)
#define MAXDOUBLE	DBL_MAX
#endif
#if !defined(MAXDOUBLE)
#define MAXDOUBLE	1.79769313486231570e+308
#endif
#endif/*_ast_fltmax_double*/

/* 64-bit vs 32-bit file stuff */
#if _sys_stat
#if _lib_lseek64 && _lib_stat64 && _lib_mmap64 && _typ_off64_t && _typ_struct_stat64
typedef struct stat64	Stat_t;
#define	lseek		lseek64
#define stat		stat64
#define fstat		fstat64
#define mmap		mmap64
#define off_t		off64_t
#else
typedef struct stat	Stat_t;
#endif
#endif

/* to get rid of pesky compiler warnings */
#if __STD_C
#define NOTUSED(x)	(void)(x)
#else
#define NOTUSED(x)	(&x,1)
#endif

/* Private flags */
#define SF_MMAP		00000001	/* in memory mapping mode		*/
#define SF_PROCESS	00000002	/* this stream is sfpopen		*/
#define SF_BOTH		00000004	/* both read/write			*/
#define SF_HOLE		00000010	/* a hole of zero's was created		*/
#define SF_NULL		00000020	/* stream is /dev/null			*/
#define SF_SEQUENTIAL	00000040	/* sequential access			*/
#define SF_MVSIZE	00000100	/* sfmove has changed buffer size	*/

/* bits for the mode field, SF_INIT defined in sfio_t.h */
#define SF_RC		00000010	/* peeking for a record			*/
#define SF_RV		00000020	/* reserve without read	or most write	*/
#define SF_LOCK		00000040	/* stream is locked for io op		*/
#define SF_PUSH		00000100	/* stream has been pushed		*/
#define SF_POOL		00000200	/* stream is in a pool but not current	*/
#define SF_PEEK		00000400	/* there is a pending peek		*/
#define SF_PKRD		00001000	/* did a peek read			*/
#define SF_GETR		00002000	/* did a getr on this stream		*/
#define SF_SYNCED	00004000	/* stream was synced			*/
#define SF_STDIO	00010000	/* given up the buffer to stdio		*/
#define SF_AVAIL	00020000	/* was closed, available for reuse	*/
#define SF_LOCAL	00100000	/* sentinel for a local call		*/

/* mode bits usable on the flags argument of sfnew() because they are >16-bit  */
#define SF_OPEN		00200000	/* just opened, ie, seek == 0		*/

#ifdef DEBUG
#define ASSERT(p)	((p) ? 0 : (abort(),0) )
#else
#define ASSERT(p)
#endif

/* short-hands */
#define NIL(t)		((t)0)
#define reg		register
#define uchar		unsigned char
#define ushort		unsigned short
#define ulong		unsigned long
#define uint		unsigned int

#define SECOND		1000	/* millisecond units */

/* macros do determine stream types from Stat_t data */
#ifndef S_IFMT
#define S_IFMT	0
#endif
#ifndef S_IFDIR
#define S_IFDIR	0
#endif
#ifndef S_IFREG
#define S_IFREG	0
#endif
#ifndef S_IFCHR
#define S_IFCHR	0
#endif
#ifndef S_IFIFO
#define S_IFIFO	0
#endif

#ifndef S_ISDIR
#define S_ISDIR(m)	(((m)&S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(m)	(((m)&S_IFMT) == S_IFREG)
#endif
#ifndef S_ISCHR
#define S_ISCHR(m)	(((m)&S_IFMT) == S_IFCHR)
#endif

#ifndef S_ISFIFO
#	ifdef S_IFIFO
#		define S_ISFIFO(m)	(((m)&S_IFIFO) == S_IFIFO)
#	else
#		define S_ISFIFO(m)	(0)
#	endif
#endif

#ifdef S_IRUSR
#define SF_CREATMODE	(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
#else
#define SF_CREATMODE	0666
#endif

/* set close-on-exec */
#ifdef F_SETFD
#	ifndef FD_CLOEXEC
#		define FD_CLOEXEC	1
#	endif /*FD_CLOEXEC*/
#	define SETCLOEXEC(fd)		((void)fcntl((fd),F_SETFD,FD_CLOEXEC))
#else
#	ifdef FIOCLEX
#		define SETCLOEXEC(fd)	((void)ioctl((fd),FIOCLEX,0))
#	else
#		define SETCLOEXEC(fd)
#	endif /*FIOCLEX*/
#endif /*F_SETFD*/

/* see if we can use memory mapping for io */
#if _mmap_worthy
#	if _hdr_mman
#		include	<mman.h>
#	endif
#	if _sys_mman
#		include	<sys/mman.h>
#	endif
#endif

/* function to get the decimal point for local environment */
#if _lib_locale
#include	<locale.h>
#define SFSETLOCALE(decimal,thousand) \
	{ struct lconv*	lv; \
	  if(*(decimal) == 0) \
	  { *(decimal) = '.'; \
	    if((lv = localeconv())) \
	    { if(lv->decimal_point && lv->decimal_point[0]) \
	    	*(decimal) = lv->decimal_point[0]; \
	      if(thousand && lv->thousands_sep && lv->thousands_sep[0]) \
	    	*(thousand) = lv->thousands_sep[0]; \
	    } \
	  } \
	}
#else
#define SFSETLOCALE(decimal,thousand)
#endif

/* stream pool structure. */
typedef struct _sfpool_s	Sfpool_t;
struct _sfpool_s
{	Sfpool_t*	next;
	int		mode;		/* type of pool			*/
	int		s_sf;		/* size of pool array		*/
	int		n_sf;		/* number currently in pool	*/
	Sfio_t**	sf;		/* array of streams		*/
	Sfio_t*		array[3];	/* start with 3			*/
};

/* reserve buffer structure */
typedef struct _sfrsrv_s	Sfrsrv_t;
struct _sfrsrv_s
{	Sfrsrv_t*	next;		/* link list			*/
	Sfio_t*		sf;		/* stream associated with	*/
	ssize_t		slen;		/* last string length		*/
	ssize_t		size;		/* buffer size			*/
	uchar		data[1];	/* data buffer			*/
};

/* extensions to sfvprintf/sfvscanf */
#define FP_SET(fp,fn)	(fp < 0 ? (fn += 1) : (fn = fp) )
#define FP_WIDTH	0
#define FP_PRECIS	1
#define FP_BASE		2
#define FP_STR		3
#define FP_SIZE		4
#define FP_INDEX	5	/* index size	*/

typedef struct _fmt_s		Fmt_t;
typedef struct _fmtpos_s	Fmtpos_t;
typedef union
{	int		i, *ip;
	long		l, *lp;
	short		h, *hp;
	uint		ui;
	ulong		ul;
	ushort		uh;
	Sflong_t	ll, *llp;
	Sfulong_t	lu;
	Sfdouble_t	ld;
	double		d;
	float		f;
	char		c, *s, **sp;
	Void_t		*vp;
	Sffmt_t		*ft;
} Argv_t;

struct _fmt_s
{	char*		form;		/* format string		*/
	va_list		args;		/* corresponding arglist	*/

	char*		oform;		/* original format string	*/
	va_list		oargs;		/* original arg list		*/
	int		argn;		/* number of args already used	*/
	Fmtpos_t*	fp;		/* position list		*/

	Sffmt_t*	ft;		/* formatting environment	*/
	Sffmtevent_f	eventf;		/* event function		*/
	Fmt_t*		next;		/* stack frame pointer		*/
};

struct _fmtpos_s
{	Sffmt_t	ft;			/* environment			*/
	Argv_t	argv;			/* argument value		*/
	int	fmt;			/* original format		*/
	int	need[FP_INDEX];		/* positions depending on	*/
};

#define LEFTP		'('
#define RIGHTP		')'
#define QUOTE		'\''

#ifndef CHAR_BIT
#define CHAR_BIT	8
#endif

#define FMTSET(ft, frm,ags, fv, sz, flgs, wid,pr,bs, ts,ns) \
	((ft->form = (char*)frm), va_copy(ft->args,ags), \
	 (ft->fmt = fv), (ft->size = sz), \
	 (ft->flags = (flgs&SFFMT_SET)), \
	 (ft->width = wid), (ft->precis = pr), (ft->base = bs), \
	 (ft->t_str = ts), (ft->n_str = ns) )
#define FMTGET(ft, frm,ags, fv, sz, flgs, wid,pr,bs) \
	((frm = ft->form), va_copy(ags,ft->args), (fv = ft->fmt), (sz = ft->size), \
	 (flgs = (flgs&~(SFFMT_SET))|(ft->flags&SFFMT_SET)), \
	 (wid = ft->width), (pr = ft->precis), (bs = ft->base) )
#define FMTCMP(sz, type) \
	(sz == sizeof(type) || ((64 == sizeof(type)*CHAR_BIT) && sz == 64) )

/* format flags&types, must coexist with those in sfio.h */
#define SFFMT_FORBIDDEN 007777700	/* for sfio.h	*/
#define SFFMT_INT	000000001	/* %d,%i 	*/
#define SFFMT_UINT	000000002	/* %u, etc.	*/
#define SFFMT_FLOAT	000000004	/* %f,e,g etc.	*/
#define SFFMT_BYTE	000000010	/* %c		*/
#define SFFMT_POINTER	000000020	/* %p, %n	*/
#define SFFMT_CLASS	000000040	/* %[		*/
#define SFFMT_GFORMAT	010000000	/* %g		*/
#define SFFMT_EFORMAT	020000000	/* %e		*/
#define SFFMT_MINUS	040000000	/* minus sign	*/

#define SFFMT_TYPES	(SFFMT_SHORT|SFFMT_LONG|SFFMT_LLONG|SFFMT_LDOUBLE)

/* memory management for the Fmt_t structures */
#define FMTALLOC(f)	((f = _Fmtfree) ? (_Fmtfree = NIL(Fmt_t*), f) : \
				(f = (Fmt_t*)malloc(sizeof(Fmt_t))) )
#define FMTFREE(f)	(_Fmtfree = _Fmtfree ? (free((Void_t*)_Fmtfree),f) : f )

/* local variables used across sf-functions */
#define _Sfpage		(_Sfextern.sf_page)
#define _Sfpool		(_Sfextern.sf_pool)
#define _Sffree		(_Sfextern.sf_free)
#define _Fmtfree	(_Sfextern.fmt_free)
#define _Sfpmove	(_Sfextern.sf_pmove)
#define _Sfstack	(_Sfextern.sf_stack)
#define _Sfnotify	(_Sfextern.sf_notify)
#define _Sfstdio	(_Sfextern.sf_stdio)
#define _Sfudisc	(&(_Sfextern.sf_udisc))
#define _Sfcleanup	(_Sfextern.sf_cleanup)
#define _Sfexiting	(_Sfextern.sf_exiting)
typedef struct _sfext_s
{	size_t			sf_page;
	struct _sfpool_s	sf_pool;
	Sfio_t*			sf_free;
	Fmt_t*			fmt_free;
	int			(*sf_pmove)_ARG_((Sfio_t*, int));
	Sfio_t*			(*sf_stack)_ARG_((Sfio_t*, Sfio_t*));
	void			(*sf_notify)_ARG_((Sfio_t*, int, int));
	int			(*sf_stdio)_ARG_((Sfio_t*));
	struct _sfdisc_s	sf_udisc;
	void			(*sf_cleanup)_ARG_((void));
	int			sf_exiting;
} Sfext_t;

/* function to clear an sfio structure */
#define SFCLEAR(f) \
	((f)->next = (f)->endw = (f)->endr = (f)->endb = (f)->data = NIL(uchar*), \
	 (f)->flags = 0, (f)->file = -1, \
	 (f)->extent = (Sfoff_t)(-1), (f)->here = (Sfoff_t)0, \
	 (f)->getr = 0, (f)->bits = 0, (f)->mode = 0, (f)->size = (ssize_t)(-1), \
	 (f)->disc = NIL(Sfdisc_t*), (f)->pool = NIL(Sfpool_t*), \
	 (f)->push = NIL(Sfio_t*) )

/* get the real value of a byte in a coded long or ulong */
#define SFUVALUE(v)	(((ulong)(v))&(SF_MORE-1))
#define SFSVALUE(v)	((( long)(v))&(SF_SIGN-1))

/* amount of precision to get in each iteration during coding of doubles */
#define SF_PRECIS	(SF_UBITS-1)

/* grain size for buffer increment */
#define SF_GRAIN	1024
#define SF_PAGE		(SF_GRAIN*sizeof(int)*2)

/* number of pages to memory map at a time */
#define SF_NMAP		8

/* set/unset sequential states for mmap */
#if _lib_madvise && defined(MADV_SEQUENTIAL) && defined(MADV_NORMAL)
#define SFMMSEQON(f,a,s)	(void)(madvise((caddr_t)(a),(size_t)(s),MADV_SEQUENTIAL) )
#define SFMMSEQOFF(f,a,s)	(void)(madvise((caddr_t)(a),(size_t)(s),MADV_NORMAL) )
#else
#define SFMMSEQON(f,a,s)
#define SFMMSEQOFF(f,a,s)
#endif

#define SFMUNMAP(f,a,s)		(void)(munmap((caddr_t)(a),(size_t)(s)) )

#ifndef MAP_VARIABLE
#define MAP_VARIABLE	0
#endif
#ifndef _mmap_fixed
#define _mmap_fixed	0
#endif

/* the bottomless bit bucket */
#ifdef macintosh
#define DEVNULL		"Dev:Null"
#else
#define DEVNULL		"/dev/null"
#endif
#define SFSETNULL(f)	((f)->extent = (Sfoff_t)(-1), (f)->bits |= SF_NULL)
#define SFISNULL(f)	((f)->extent < 0 && ((f)->bits&SF_NULL) )

#define SFKILL(f)	((f)->mode = (SF_AVAIL|SF_LOCK) )
#define SFKILLED(f)	(((f)->mode&(SF_AVAIL|SF_LOCK)) == (SF_AVAIL|SF_LOCK) )

/* exception types */
#define SF_EDONE	0	/* stop this operation and return	*/
#define SF_EDISC	1	/* discipline says it's ok		*/
#define SF_ESTACK	2	/* stack was popped			*/
#define SF_ECONT	3	/* can continue normally		*/

#define SETLOCAL(f)	((f)->mode |= SF_LOCAL)
#define GETLOCAL(f,v)	((v) = ((f)->mode&SF_LOCAL), (f)->mode &= ~SF_LOCAL, (v))
#define SFWRALL(f)	((f)->mode |= SF_RV)
#define SFISALL(f,v)	((((v) = (f)->mode&SF_RV) ? ((f)->mode &= ~SF_RV) : 0), \
			 ((v) || (f)->extent < 0 || \
			  ((f)->flags&(SF_SHARE|SF_APPENDWR|SF_WHOLE)) ) )
#define SFSK(f,a,o,d)	(SETLOCAL(f),sfsk(f,(Sfoff_t)a,o,d))
#define SFRD(f,b,n,d)	(SETLOCAL(f),sfrd(f,(Void_t*)b,n,d))
#define SFWR(f,b,n,d)	(SETLOCAL(f),sfwr(f,(Void_t*)b,n,d))
#define SFSYNC(f)	(SETLOCAL(f),sfsync(f))
#define SFCLOSE(f)	(SETLOCAL(f),sfclose(f))
#define SFFLSBUF(f,n)	(SETLOCAL(f),_sfflsbuf(f,n))
#define SFFILBUF(f,n)	(SETLOCAL(f),_sffilbuf(f,n))
#define SFSETBUF(f,s,n)	(SETLOCAL(f),sfsetbuf(f,s,n))
#define SFWRITE(f,s,n)	(SETLOCAL(f),sfwrite(f,s,n))
#define SFREAD(f,s,n)	(SETLOCAL(f),sfread(f,s,n))
#define SFSEEK(f,p,t)	(SETLOCAL(f),sfseek(f,p,t))
#define SFNPUTC(f,c,n)	(SETLOCAL(f),sfnputc(f,c,n))
#define SFRAISE(f,e,d)	(SETLOCAL(f),sfraise(f,e,d))

/* lock/open a stream */
#define SFMODE(f,l)	((f)->mode & ~(SF_RV|SF_RC|((l) ? SF_LOCK : 0)) )
#define SFLOCK(f,l)	(void)((f)->mode |= SF_LOCK, (f)->endr = (f)->endw = (f)->data)
#define _SFOPEN(f)	((f)->endr=((f)->mode == SF_READ) ? (f)->endb : (f)->data, \
			 (f)->endw=(((f)->mode == SF_WRITE) && !((f)->flags&SF_LINE)) ? \
				      (f)->endb : (f)->data )
#define SFOPEN(f,l)	(void)((l) ? 0 : \
				((f)->mode &= ~(SF_LOCK|SF_RC|SF_RV), _SFOPEN(f), 0) )

/* check to see if the stream can be accessed */
#define SFFROZEN(f)	((f)->mode&(SF_PUSH|SF_LOCK|SF_PEEK) ? 1 : \
			 ((f)->mode&SF_STDIO) ? (*_Sfstdio)(f) : 0)


/* set discipline code */
#define SFDISC(f,dc,iof,local) \
	{	Sfdisc_t* d; \
		if(!(dc)) \
			d = (dc) = (f)->disc; \
		else 	d = (local) ? (dc) : ((dc) = (dc)->disc); \
		while(d && !(d->iof))	d = d->disc; \
		if(d)	(dc) = d; \
	}

/* fast peek of a stream */
#define _SFAVAIL(f,s,n)	((n) = (f)->endb - ((s) = (f)->next) )
#define SFRPEEK(f,s,n)	(_SFAVAIL(f,s,n) > 0 ? (n) : \
				((n) = SFFILBUF(f,-1), (s) = (f)->next, (n)) )
#define SFWPEEK(f,s,n)	(_SFAVAIL(f,s,n) > 0 ? (n) : \
				((n) = SFFLSBUF(f,-1), (s) = (f)->next, (n)) )

/* malloc and free of streams */
#define SFFREE(f)	((_Sffree ? (free((Void_t*)_Sffree),0) : 0), _Sffree = f)
#define SFALLOC(f)	((f = _Sffree) ? (_Sffree = NIL(Sfio_t*), f) : \
				   (f = (Sfio_t*)malloc(sizeof(Sfio_t))))

/* more than this for a line buffer, we might as well flush */
#define HIFORLINE	128

/* safe closing function */
#define CLOSE(f)	{ while(close(f) < 0 && errno == EINTR) errno = 0; }

/* string stream extent */
#define SFSTRSIZE(f)	{ reg Sfoff_t s = (f)->next - (f)->data; \
			  if(s > (f)->here) \
			    { (f)->here = s; if(s > (f)->extent) (f)->extent = s; } \
			}

/* control flags for open() */
#ifdef O_CREAT
#define _has_oflags	1
#else	/* for example, research UNIX */
#define _has_oflags	0
#define O_CREAT		004
#define O_TRUNC		010
#define O_APPEND	020
#define O_EXCL		040

#ifndef O_RDONLY
#define	O_RDONLY	000
#endif
#ifndef O_WRONLY
#define O_WRONLY	001
#endif
#ifndef O_RDWR
#define O_RDWR		002
#endif
#endif /*O_CREAT*/

#ifndef O_BINARY
#define O_BINARY	000
#endif
#ifndef O_TEXT
#define O_TEXT		000
#endif
#ifndef O_TEMPORARY
#define O_TEMPORARY	000
#endif

#define	SF_RADIX	64	/* maximum integer conversion base */

#if _PACKAGE_ast
#define SF_MAXINT	INT_MAX
#define SF_MAXLONG	LONG_MAX
#else
#define SF_MAXINT	((int)(((uint)~0) >> 1))
#define SF_MAXLONG	((long)(((ulong)~0L) >> 1))
#endif

#define SF_MAXCHAR	((uchar)(~0))

/* floating point to ascii conversion */
#define SF_MAXEXP10	6
#define SF_MAXPOW10	(1 << SF_MAXEXP10)
#if !_ast_fltmax_double
#define SF_FDIGITS	1024		/* max allowed fractional digits */
#define SF_IDIGITS	(8*1024)	/* max number of digits in int part */
#else
#define SF_FDIGITS	256		/* max allowed fractional digits */
#define SF_IDIGITS	1024		/* max number of digits in int part */
#endif
#define SF_MAXDIGITS	(((SF_FDIGITS+SF_IDIGITS)/sizeof(int) + 1)*sizeof(int))

/* tables for numerical translation */
#define _Sfpos10	(_Sftable.sf_pos10)
#define _Sfneg10	(_Sftable.sf_neg10)
#define _Sfdec		(_Sftable.sf_dec)
#define _Sfdigits	(_Sftable.sf_digits)
#define _Sfcvinitf	(_Sftable.sf_cvinitf)
#define _Sfcvinit	(_Sftable.sf_cvinit)
#define _Sffmtposf	(_Sftable.sf_fmtposf)
#define _Sffmtintf	(_Sftable.sf_fmtintf)
#define _Sfcv36		(_Sftable.sf_cv36)
#define _Sfcv64		(_Sftable.sf_cv64)
#define _Sftype		(_Sftable.sf_type)
typedef struct _sftab_
{	Sfdouble_t	sf_pos10[SF_MAXEXP10];	/* positive powers of 10	*/
	Sfdouble_t	sf_neg10[SF_MAXEXP10];	/* negative powers of 10	*/
	uchar		sf_dec[200];		/* ascii reps of values < 100	*/
	char*		sf_digits;		/* digits for general bases	*/ 
	int		(*sf_cvinitf)();	/* initialization function	*/
	int		sf_cvinit;		/* initialization state		*/
	Fmtpos_t*	(*sf_fmtposf)_ARG_((Sfio_t*,const char*,va_list,int));
	char*		(*sf_fmtintf)_ARG_((const char*,int*));
	uchar		sf_cv36[SF_MAXCHAR+1];	/* conversion for base [2-36]	*/
	uchar		sf_cv64[SF_MAXCHAR+1];	/* conversion for base [37-64]	*/
	uchar		sf_type[SF_MAXCHAR+1];	/* conversion formats&types	*/
} Sftab_t;

/* thread-safe macro/function to initialize _Sfcv* conversion tables */
#define SFCVINIT()      (_Sfcvinit ? 1 : (_Sfcvinit = (*_Sfcvinitf)()) )

/* sfucvt() converts decimal integers to ASCII */
#define SFDIGIT(v,scale,digit) \
	{ if(v < 5*scale) \
		if(v < 2*scale) \
			if(v < 1*scale) \
				{ digit = '0'; } \
			else	{ digit = '1'; v -= 1*scale; } \
		else	if(v < 3*scale) \
				{ digit = '2'; v -= 2*scale; } \
			else if(v < 4*scale) \
				{ digit = '3'; v -= 3*scale; } \
			else	{ digit = '4'; v -= 4*scale; } \
	  else	if(v < 7*scale) \
			if(v < 6*scale) \
				{ digit = '5'; v -= 5*scale; } \
			else	{ digit = '6'; v -= 6*scale; } \
		else	if(v < 8*scale) \
				{ digit = '7'; v -= 7*scale; } \
			else if(v < 9*scale) \
				{ digit = '8'; v -= 8*scale; } \
			else	{ digit = '9'; v -= 9*scale; } \
	}
#define sfucvt(v,s,n,list,type,utype) \
	{ while((utype)v >= 10000) \
	  {	n = v; v = (type)(((utype)v)/10000); \
		n = (type)((utype)n - ((utype)v)*10000); \
	  	s -= 4; SFDIGIT(n,1000,s[0]); SFDIGIT(n,100,s[1]); \
			s[2] = *(list = (char*)_Sfdec + (n <<= 1)); s[3] = *(list+1); \
	  } \
	  if(v < 100) \
	  { if(v < 10) \
	    { 	s -= 1; s[0] = (char)('0'+v); \
	    } else \
	    { 	s -= 2; s[0] = *(list = (char*)_Sfdec + (v <<= 1)); s[1] = *(list+1); \
	    } \
	  } else \
	  { if(v < 1000) \
	    { 	s -= 3; SFDIGIT(v,100,s[0]); \
			s[1] = *(list = (char*)_Sfdec + (v <<= 1)); s[2] = *(list+1); \
	    } else \
	    {	s -= 4; SFDIGIT(v,1000,s[0]); SFDIGIT(v,100,s[1]); \
			s[2] = *(list = (char*)_Sfdec + (v <<= 1)); s[3] = *(list+1); \
	    } \
	  } \
	}

/* handy functions */
#ifndef __cplusplus
#undef min
#undef max
#define min(x,y)	((x) < (y) ? (x) : (y))
#define max(x,y)	((x) > (y) ? (x) : (y))
#elif defined(__SC__) || defined(__MRC__)
#include <algorithm>
#else
#include <algorithm.h>
#endif

/* fast functions for memory copy and memory clear */
#if _PACKAGE_ast
#define memclear(s,n)	memzero(s,n)
#else
#if _lib_bcopy && !_lib_memcpy
#define memcpy(to,fr,n)	bcopy((fr),(to),(n))
#endif
#if _lib_bzero && !_lib_memset
#define memclear(s,n)	bzero((s),(n))
#else
#define memclear(s,n)	memset((s),'\0',(n))
#endif
#endif /*_PACKAGE_ast*/

/* note that MEMCPY advances the associated pointers */
#define MEMCPY(to,fr,n) \
	switch(n) \
	{ default : memcpy((Void_t*)to,(Void_t*)fr,n); to += n; fr += n; break; \
	  case  7 : *to++ = *fr++; \
	  case  6 : *to++ = *fr++; \
	  case  5 : *to++ = *fr++; \
	  case  4 : *to++ = *fr++; \
	  case  3 : *to++ = *fr++; \
	  case  2 : *to++ = *fr++; \
	  case  1 : *to++ = *fr++; \
	}
#define MEMSET(s,c,n) \
	switch(n) \
	{ default : memset((Void_t*)s,(int)c,n); s += n; break; \
	  case  7 : *s++ = c; \
	  case  6 : *s++ = c; \
	  case  5 : *s++ = c; \
	  case  4 : *s++ = c; \
	  case  3 : *s++ = c; \
	  case  2 : *s++ = c; \
	  case  1 : *s++ = c; \
	}

_BEGIN_EXTERNS_

extern Sfext_t		_Sfextern;
extern Sftab_t		_Sftable;

extern int		_sfpopen _ARG_((Sfio_t*, int, int));
extern int		_sfpclose _ARG_((Sfio_t*));
extern int		_sfmode _ARG_((Sfio_t*, int, int));
extern int		_sftype _ARG_((const char*, int*));
extern int		_sfexcept _ARG_((Sfio_t*, int, ssize_t, Sfdisc_t*));
extern Sfrsrv_t*	_sfrsrv _ARG_((Sfio_t*, ssize_t));
extern int		_sfsetpool _ARG_((Sfio_t*));
extern void		_sfswap _ARG_((Sfio_t*, Sfio_t*, int));
extern char*		_sfcvt _ARG_((Void_t*, int, int*, int*, int));
extern char**		_sfgetpath _ARG_((char*));
extern Sfdouble_t	_sfstrtod _ARG_((const char*, char**));

#if !_lib_strtod
#define strtod		_sfstrtod
#endif

#ifndef errno
_astimport int		errno;
#endif

/* for portable encoding of double values */
#if !__STD_C
extern double	frexp _ARG_((double, int*));
extern double	ldexp _ARG_((double,int));
#endif

#if !_hdr_mman && !_sys_mman
extern Void_t*	mmap _ARG_((Void_t*, size_t, int, int, int, off_t));
extern int	munmap _ARG_((Void_t*, size_t));
#endif

#if !_PACKAGE_ast

#if !__STDC__ && !_hdr_stdlib
extern void	abort _ARG_((void));
extern int	atexit _ARG_((void(*)(void)));
extern char*	getenv _ARG_((const char*));
extern void*	malloc _ARG_((size_t));
extern void*	realloc _ARG_((void*, size_t));
extern void	free _ARG_((void*));
extern size_t	strlen _ARG_((const char*));
extern char*	strcpy _ARG_((char*, const char*));

extern Void_t*	memset _ARG_((void*, int, size_t));
extern Void_t*	memchr _ARG_((const void*, int, size_t));
extern Void_t*	memccpy _ARG_((void*, const void*, int, size_t));
#ifndef memcpy
extern Void_t*	memcpy _ARG_((void*, const void*, size_t));
#endif
#if !defined(strtod)
extern double	strtod _ARG_((const char*, char**));
#endif
#if !defined(remove)
extern int	remove _ARG_((const char*));
#endif
#endif /* !__STDC__ && !_hdr_stdlib */

#if !_hdr_unistd
extern int	close _ARG_((int));
extern ssize_t	read _ARG_((int, void*, size_t));
extern ssize_t	write _ARG_((int, const void*, size_t));
extern off_t	lseek _ARG_((int, off_t, int));
extern int	dup _ARG_((int));
extern int	isatty _ARG_((int));
extern int	wait _ARG_((int*));
extern int	pipe _ARG_((int*));
extern int	access _ARG_((const char*, int));
extern uint	sleep _ARG_((uint));
extern int	execl _ARG_((const char*, const char*,...));
#if !defined(fork)
extern int	fork _ARG_((void));
#endif
#if _lib_unlink
extern int	unlink _ARG_((const char*));
#endif

#endif /*_hdr_unistd*/

#if _lib_bcopy /*&& !_lib_memcpy*/
#ifdef __linux__
extern void	bcopy _ARG_((const void*, void*, int));
#else
extern void	bcopy _ARG_((const void*, void*, size_t));
#endif
#endif
#if _lib_bzero /*&& !_lib_memset*/
#ifdef __linux__
extern void	bzero _ARG_((void*, int));
#else
extern void	bzero _ARG_((void*, size_t));
#endif
#endif
extern time_t	time _ARG_((time_t*));
extern int	waitpid _ARG_((int,int*,int));
extern void	_exit _ARG_((int));
extern int	onexit _ARG_((void(*)(void)));

#if _sys_stat
extern int	fstat _ARG_((int, Stat_t*));
#endif

#if _lib_vfork && !_hdr_vfork && !_sys_vfork
extern pid_t	vfork _ARG_((void));
#endif /*_lib_vfork*/

#if _lib_poll
#if _lib_poll_fd_1
extern int	poll _ARG_((struct pollfd*, ulong, int));
#else
extern int	poll _ARG_((ulong, struct pollfd*, int));
#endif
#endif /*_lib_poll*/

#if _proto_open && __cplusplus
extern int	open _ARG_((const char*, int, ...));
#endif

#if _stream_peek
extern int	ioctl _ARG_((int, int, ...));
#endif /*_stream_peek*/

#if _socket_peek && !__STDC__
#ifdef macintosh
extern ssize_t	recv _ARG_((int, void*, size_t, int));
#else
extern int	recv _ARG_((int, void*, int, int));
#endif
#endif /*_socket_peek*/

#endif /* _PACKAGE_ast */

_END_EXTERNS_

#endif /*_SFHDR_H*/
