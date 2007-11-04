#ifndef _SFSTDIO_H	/* protect against multiple #includes */
#define _SFSTDIO_H	1

#ifdef macintosh
/* Prevent including standard stdio header */
#ifdef __MWERKS__
#define __cstdio__	1
#else
#define __STDIO__	1
#endif
#endif

#ifdef __cplusplus
extern "C"
#endif
int rename( const char* from, const char* to );

#define stdin		sfstdin
#define stdout		sfstdout
#define stderr		sfstderr
#define BUFSIZ		SF_BUFSIZE
#undef FILE
#define FILE		Sfio_t

#include		<sfio.h>

#define _IOFBF		0
#define _IONBF		1
#define _IOLBF		2
#define L_ctermid	32
#define L_cuserid	32
#define P_tmpdir	"/tmp/"
#define L_tmpnam	(sizeof(P_tmpdir)+32)

#define fpos_t		Sfoff_t

_BEGIN_EXTERNS_
#if _BLD_sfio && defined(__EXPORT__)
#define extern	__EXPORT__
#endif
#if !_BLD_sfio && defined(__IMPORT__) && defined(__EXPORT__)
#define extern	__IMPORT__
#endif

extern char*	ctermid _ARG_((char*));
extern char*	cuserid _ARG_((char*));
extern char*	tmpnam _ARG_((char*));
extern char*	tempnam _ARG_((const char*, const char*));
#ifndef remove
extern int	remove _ARG_((const char*));
#endif
extern void	perror _ARG_((const char*));
extern Sfio_t*	_stdopen _ARG_((int, const char*));
extern char*	_stdgets _ARG_((Sfio_t*, char*, int n, int isgets));
extern int	_stdprintf _ARG_((const char*, ...));
extern int	_stdsprintf _ARG_((char*, const char*, ...));
extern int	_stdscanf _ARG_((const char*, ...));
extern int	_stdsetvbuf _ARG_((Sfio_t*, char*, int, size_t));

#undef extern
_END_EXTERNS_

#define _SFSIZEOF(s)	(sizeof(s) != sizeof(char*) ? sizeof(s) : BUFSIZ)

#define	printf			_stdprintf
#define sprintf			_stdsprintf
#define fprintf			sfprintf
#define snprintf		sfsprintf
#define vfprintf		sfvprintf
#define vsnprintf		sfvsprintf

#define scanf			_stdscanf
#define _doscan			sfvscanf
#define sscanf			sfsscanf
#define fscanf			sfscanf
#define vfscanf			sfvscanf
#define vsscanf			sfvsscanf

#define fdopen			_stdopen
#define setvbuf			_stdsetvbuf


#define __sf_freopen(fl,m,f)	sfopen((f),(fl),(m))
#define __sf_fopen(fl,m)	sfopen((Sfio_t*)0,(fl),(m))
#define __sf_popen(cmd,m)	sfpopen((Sfio_t*)0,(cmd),(m))
#define __sf_tmpfile()		sftmp(0)
#define __sf_fclose(f)		sfclose(f)
#define __sf_pclose(f)		sfclose(f)

#define __sf_fwrite(p,s,n,f)	((_SF_(f)->val = sfwrite((f),(p),(s)*(n))) <= 0 ? \
						 0 : _SF_(f)->val/(s) )
#define __sf_putw(w,f)		(_SF_(f)->val = (int)(w), \
				 sfwrite((f),&_SF_(f)->val,sizeof(int)) <= 0 ? 1 : 0)
#define __sf_fputc(c,f)		sfputc((f),(c))
#define __sf_putchar(c)		sfputc(sfstdout,(c))
#define __sf_fputs(s,f)		sfputr((f),(s),-1)
#define __sf_puts(s)		sfputr(sfstdout,(s),'\n')
#define __sf_vprintf(fmt,a)	sfvprintf(sfstdout,(fmt),(a))
#define __sf_doprnt(fmt,a,f)	sfvprintf((f),(fmt),(a))
#define __sf_vsprintf(s,fmt,a)	sfvsprintf((s),_SFSIZEOF(s),(fmt),(a) )

#define __sf_fread(p,s,n,f)	((_SF_(f)->val = sfread((f),(p),(s)*(n))) <= 0 ? \
						 0 : _SF_(f)->val/(s) )
#define __sf_getw(f)		(sfread((f),&_SF_(f)->val,sizeof(int)) == sizeof(int) ?\
				  _SF_(f)->val : -1 )
#define __sf_fgetc(f)		sfgetc(f)
#define __sf_getchar()		sfgetc(sfstdin)
#define __sf_ungetc(c,f)	sfungetc((f),(c))
#define __sf_fgets(s,n,f)	_stdgets((f),(s),(n),0)
#define __sf_gets(s)		_stdgets(sfstdin,(s),_SFSIZEOF(s),1)
#define __sf_vscanf(fmt,a)	sfvscanf(sfstdin,(fmt),(a))

#define __sf_fflush(f)		sfsync(f)
#define __sf_fpurge(f)		sfpurge(f)
#define __sf_fseek(f,o,t)	(sfseek((f),(Sfoff_t)(o),(t)) < 0 ? -1 : 0)
#define __sf_rewind(f)		sfseek((f),(Sfoff_t)0,0)
#define __sf_ftell(f)		(long)sftell(f)
#define __sf_fgetpos(f,pos)	((*(pos) = (fpos_t)sftell(f)) >= 0 ? 0 : -1)
#define __sf_fsetpos(f,pos)	((fpos_t)sfseek(f,(Sfoff_t)(*(pos)),0) \
					!= (*pos) ? -1 : 0)
#define __sf_setbuf(f,b)	(void)sfsetbuf((f),(b),(b) ? BUFSIZ : 0)
#define __sf_setbuffer(f,b,n)	(sfsetbuf((f),(b),(n)) ? 0 : -1)
#define __sf_setlinebuf(f)	sfset((f),SF_LINE,1)

#define __sf_feof(f)		sfeof(f)
#define __sf_ferror(f)		sferror(f)
#define __sf_clearerr(f)	(void)(sfclrerr(f),sfclrlock(f))

#if defined(__INLINE__) && !BLD_sfio
__INLINE__ FILE* freopen(const char* fl, const char* m, FILE* f)
							{ return __sf_freopen(fl,m,f);	}
__INLINE__ FILE* fopen(const char* fl, const char* m)	{ return __sf_fopen(fl,m);	}
__INLINE__ FILE* popen(const char* cmd, const char* m)	{ return __sf_popen(cmd,m);	}
__INLINE__ FILE* tmpfile()				{ return __sf_tmpfile();	}
__INLINE__ int fclose(FILE* f)				{ return __sf_fclose(f);	}
__INLINE__ int pclose(FILE* f)				{ return __sf_pclose(f);	}

__INLINE__ size_t fwrite(const void* p, size_t s, size_t n, FILE* f)
							{ return __sf_fwrite(p,s,n,f);	}
__INLINE__ int putw(int w, FILE* f)			{ return __sf_putw(w,f);	}
__INLINE__ int fputc(int c, FILE* f)			{ return __sf_fputc(c,f);	}
__INLINE__ int putc(int c, FILE* f)			{ return __sf_fputc(c,f);	}
__INLINE__ int putchar(int c)				{ return __sf_putchar(c);	}
__INLINE__ int fputs(const char* s, FILE* f)		{ return __sf_fputs(s,f);	}
__INLINE__ int puts(const char* s)			{ return __sf_puts(s);		}
__INLINE__ int vprintf(const char* fmt, va_list a)	{ return __sf_vprintf(fmt,a);	}
__INLINE__ int _doprnt(const char* fmt, va_list a, FILE* f)
							{ return __sf_doprnt(fmt,a,f);	}
__INLINE__ int vsprintf(char* s, const char* fmt, va_list a)
							{ return __sf_vsprintf(s,fmt,a);}

__INLINE__ size_t fread(void* p, size_t s, size_t n, FILE* f)
							{ return __sf_fread(p,s,n,f);	}
__INLINE__ int getw(FILE* f)				{ return __sf_getw(f);		}
__INLINE__ int fgetc(FILE* f)				{ return __sf_fgetc(f);		}
__INLINE__ int getc(FILE* f)				{ return __sf_fgetc(f);		}
__INLINE__ int getchar(void)				{ return __sf_getchar();	}
__INLINE__ int ungetc(int c, FILE* f)			{ return __sf_ungetc(c,f);	}
__INLINE__ char* fgets(char* s, int n, FILE* f)		{ return __sf_fgets(s,n,f);	}
__INLINE__ char* gets(char* s)				{ return __sf_gets(s);		}
__INLINE__ int vscanf(const char* fmt, va_list a)	{ return __sf_vscanf(fmt,a);	}

__INLINE__ int fflush(FILE* f)				{ return __sf_fflush(f);	}
__INLINE__ int fpurge(FILE* f)				{ return __sf_fpurge(f);	}
__INLINE__ int fseek(FILE* f, long o, int t)		{ return __sf_fseek(f,o,t);	}
__INLINE__ void rewind(FILE* f)				{ (void) __sf_rewind(f);	}
__INLINE__ long ftell(FILE* f)				{ return __sf_ftell(f);		}
__INLINE__ int fgetpos(FILE* f, fpos_t* pos)		{ return __sf_fgetpos(f,pos);	}
__INLINE__ int fsetpos(FILE* f, fpos_t* pos)		{ return __sf_fsetpos(f,pos);	}
__INLINE__ void setbuf(FILE* f, char* b)		{ (void) __sf_setbuf(f,b);	}
__INLINE__ int setbuffer(FILE* f, char* b, int n)	{ return __sf_setbuffer(f,b,n);	}
__INLINE__ int setlinebuf(FILE* f)			{ return __sf_setlinebuf(f);	}

__INLINE__ int fileno(FILE* f)				{ return __sf_fileno(f);	}
__INLINE__ int feof(FILE* f)				{ return __sf_feof(f);		}
__INLINE__ int ferror(FILE* f)				{ return __sf_ferror(f);	}
__INLINE__ void clearerr(FILE* f)			{ (void) __sf_clearerr(f);	}

#else

#define freopen(fl,m,f)					( __sf_freopen(fl,m,f)		)
#define fopen(fl,m)					( __sf_fopen(fl,m)		)
#define popen(cmd,m)					( __sf_popen(cmd,m)		)
#define tmpfile()					( __sf_tmpfile()		)
#define fclose(f)					( __sf_fclose(f)		)
#define pclose(f)					( __sf_pclose(f)		)

#define fwrite(p,s,n,f)					( __sf_fwrite(p,s,n,f)		)
#define putw(w,f)					( __sf_putw(w,f)		)
#define fputc(c,f)					( __sf_fputc(c,f)		)
#define putc(c,f)					( __sf_fputc(c,f)		)
#define putchar(c)					( __sf_putchar(c)		)
#define fputs(s,f)					( __sf_fputs(s,f)		)
#define puts(s)						( __sf_puts(s)			)
#define vprintf(fmt,a)					( __sf_vprintf(fmt,a)		)
#define _doprnt(fmt,a,f) 				( __sf_doprnt(fmt,a,f)		)
#define vsprintf(s,fmt,a)				( __sf_vsprintf(s,fmt,a)	)

#define fread(p,s,n,f)					( __sf_fread(p,s,n,f)		)
#define fgetc(f)					( __sf_fgetc(f)			)
#define getw(f)						( __sf_getw(f)			)
#define getc(f)						( __sf_fgetc(f)			)
#define getchar()					( __sf_getchar()		)
#define ungetc(c,f)					( __sf_ungetc(c,f)		)
#define fgets(s,n,f)					( __sf_fgets(s,n,f)		)
#define gets(s)						( __sf_gets(s)			)
#define vscanf(fmt,a)					( __sf_vscanf(fmt,a)		)

#define fflush(f)					( __sf_fflush(f)		)
#define fpurge(f)					( __sf_fpurge(f)		)
#define fseek(f,o,t)					( __sf_fseek(f,o,t)		)
#define rewind(f)					( __sf_rewind(f)		)
#define ftell(f)					( __sf_ftell(f)			)
#define fgetpos(f,pos)					( __sf_fgetpos(f,pos)		)
#define fsetpos(f,pos)					( __sf_fsetpos(f,pos)		)
#define setbuf(f,b)					( __sf_setbuf(f,b)		)
#define setbuffer(f,b,n) 				( __sf_setbuffer(f,b,n)		)
#define setlinebuf(f)					( __sf_setlinebuf(f)		)

#define fileno(f)					( __sf_fileno(f)		)
#define feof(f)						( __sf_feof(f)			)
#define ferror(f)					( __sf_ferror(f)		)
#define clearerr(f)					( __sf_clearerr(f)		)

#endif

#endif /* _SFSTDIO_H */
