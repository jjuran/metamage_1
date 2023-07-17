/*********************************************************************
Project	:	Perl5				-	
File		:	macish.h			-	Mac specific things
Author	:	Matthias Neeracher

*********************************************************************/

// For some reason, NSIG is already defined, so perl thinks signal.h has been included.
#include <signal.h>

#include <ConditionalMacros.h>

extern char** environ;

/* For now we get compiler this way, until we can
   find a better way */
#if TARGET_CPU_PPC
#  define	ARCHNAME	"MacPPC"
#  if defined(__MRC__)
#    define	CC		"MrC"
#  else
#    define	CC		"MWCPPC"
#  endif
#else
#  if TARGET_RT_MAC_CFM
#    define	ARCHNAME	"MacCFM68K"
#  else
#    define	ARCHNAME	"Mac68K"
#  endif
#  if defined(__SC__)
#    define	CC		"SC"
#  else
#    define	CC		"MWC68K"
#  endif
#endif

void MacPerl_WriteMsg(void * io, const char * msg, size_t len);

#define u_short unsigned short
#define u_long  unsigned long

#define Big_time_t	unsigned long

/* These defined following should be defined in 5.8 in config.h */
#define HAS_USLEEP
#define HAS_UALARM


/* HAS_IOCTL:
 *	This symbol, if defined, indicates that the ioctl() routine is
 *	available to set I/O characteristics
 */
#define	HAS_IOCTL		/**/
 
/* HAS_UTIME:
 *	This symbol, if defined, indicates that the routine utime() is
 *	available to update the access and modification times of files.
 */
#define HAS_UTIME		/**/

/* HAS_GROUP
 *	This symbol, if defined, indicates that the getgrnam() and
 *	getgrgid() routines are available to get group entries.
 *	The getgrent() has a separate definition, HAS_GETGRENT.
 */
#undef HAS_GROUP		/**/

/* HAS_PASSWD
 *	This symbol, if defined, indicates that the getpwnam() and
 *	getpwuid() routines are available to get password entries.
 *	The getpwent() has a separate definition, HAS_GETPWENT.
 */
#undef HAS_PASSWD		/**/

#define HAS_KILL
#define HAS_WAIT
  
/* USEMYBINMODE
 *	This symbol, if defined, indicates that the program should
 *	use the routine my_binmode(FILE *fp, char iotype) to insure
 *	that a file is in "binary" mode -- that is, that no translation
 *	of bytes occurs on read or write operations.
 */
#undef USEMYBINMODE

/* Stat_t:
 *	This symbol holds the type used to declare buffers for information
 *	returned by stat().  It's usually just struct stat.  It may be necessary
 *	to include <sys/stat.h> and <sys/types.h> to get any typedef'ed
 *	information.
 */
#define Stat_t struct stat

/* USE_STAT_RDEV:
 *	This symbol is defined if this system has a stat structure declaring
 *	st_rdev
 */
#undef USE_STAT_RDEV 	/**/

/* ACME_MESS:
 *	This symbol, if defined, indicates that error messages should be 
 *	should be generated in a format that allows the use of the Acme
 *	GUI/editor's autofind feature.
 */
#undef ACME_MESS	/**/

/* UNLINK_ALL_VERSIONS:
 *	This symbol, if defined, indicates that the program should arrange
 *	to remove all versions of a file if unlink() is called.  This is
 *	probably only relevant for VMS.
 */
/* #define UNLINK_ALL_VERSIONS		/ **/

/* VMS:
 *	This symbol, if defined, indicates that the program is running under
 *	VMS.  It is currently automatically set by cpps running under VMS,
 *	and is included here for completeness only.
 */
/* #define VMS		/ **/

/* ALTERNATE_SHEBANG:
 *	This symbol, if defined, contains a "magic" string which may be used
 *	as the first line of a Perl program designed to be executed directly
 *	by name, instead of the standard Unix #!.  If ALTERNATE_SHEBANG
 *	begins with a character other then #, then Perl will only treat
 *	it as a command line if if finds the string "perl" in the first
 *	word; otherwise it's treated as the first line of code in the script.
 *	(IOW, Perl won't hand off to another interpreter via an alternate
 *	shebang sequence that might be legal Perl code.)
 */
/* #define ALTERNATE_SHEBANG "#!" / **/

#define ABORT() abort();

/*
 * fwrite1() should be a routine with the same calling sequence as fwrite(),
 * but which outputs all of the bytes requested as a single stream (unlike
 * fwrite() itself, which on some systems outputs several distinct records
 * if the number_of_items parameter is >1).
 */
#define fwrite1 fwrite

#define Stat(fname,bufptr) stat((fname),(bufptr))
#define Fstat(fd,bufptr)   fstat((fd),(bufptr))
#define Fflush(fp)         fflush(fp)
#define Mkdir(path,mode)   mkdir((path),(mode))

#ifndef PERL_SYS_INIT3
#	define PERL_SYS_INIT3(c,v,e)  PL_opargs[OP_TIME] &= ~OA_RETINTEGER; MALLOC_INIT
#endif

#ifndef PERL_SYS_TERM
#define PERL_SYS_TERM()		MALLOC_TERM
#endif

#ifdef __cplusplus
extern "C"
#endif
void relix_async_check();

#define PERL_ASYNC_CHECK()  relix_async_check()

#define PERL_WRITE_MSG_TO_CONSOLE(io, msg, len) MacPerl_WriteMsg(io, msg, len)

#define BIT_BUCKET "/dev/null"

#define dXSUB_SYS

#define CLK_TCK  CLOCKS_PER_SEC

// Casualties of the libc upgrade
#undef HAS_ACCESS
#undef HAS_GETHOSTBYADDR

