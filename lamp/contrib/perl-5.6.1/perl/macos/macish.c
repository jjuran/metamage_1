/*********************************************************************
Project	:	Perl5				-	
File	:	macish.c			-	Mac specific things
Author	:	Matthias Neeracher

*********************************************************************/

#define MAC_CONTEXT
#define MP_EXT
#define MP_INIT(x) = x

#include "EXTERN.h"
#include "perl.h"
#undef Move

#if !defined(__MACTYPES__) && !defined(__cplusplus)

enum Boolean
{
	false,
	true
};
typedef enum Boolean Boolean;

#endif

/* Borrowed from msdos.c
 * Just pretend that everyone is a superuser
 */
/* DISPATCH_START */
#define ROOT_UID	0
#define ROOT_GID	0

int
(setuid)(Uid_t uid)
{ 
	return (uid==ROOT_UID?0:-1);
}

int
(setgid)(Gid_t gid)
{ 
	return (gid==ROOT_GID?0:-1); 
}


char * sys_errlist[] = {
	"No error",
	"Operation not permitted",
	"No such file or directory",
	"No such process",
	"Interrupted system call",
	"Input/output error",
	"Device not configured",
	"Argument list too long",
	"Exec format error",
	"Bad file descriptor",
	"No child processes",
	"Operation would block",
	"Cannot allocate memory",
	"Permission denied",
	"Bad address",
	"Block device required",
	"Device busy",
	"File exists",
	"Cross-device link",
	"Operation not supported by device",
	"Not a directory",
	"Is a directory",
	"Invalid argument",
	"Too many open files in system",
	"Too many open files",
	"Inappropriate ioctl for device",
	"Text file busy",
	"File too large",
	"No space left on device",
	"Illegal seek",
	"Read-only file system",
	"Too many links",
	"Broken pipe",
	"Numerical argument out of domain",
	"Result too large",
	"(unknown)",
	"Operation now in progress",
	"Operation already in progress",
	"Socket operation on non-socket",
	"Destination address required",
	"Message too long",
	"Protocol wrong type for socket",
	"Protocol not available",
	"Protocol not supported",
	"Socket type not supported",
	"Operation not supported on socket",
	"Protocol family not supported",
	"Address family not supported by protocol family",
	"Address already in use",
	"Can't assign requested address",
	"Network is down",
	"Network is unreachable",
	"Network dropped connection on reset",
	"Software caused connection abort",
	"Connection reset by peer",
	"No buffer space available",
	"Socket is already connected",
	"Socket is not connected",
	"Can't send after socket shutdown",
	"Too many references: can't splice",
	"Connection timed out",
	"Connection refused",
	"Too many levels of symbolic links",
	"File name too long",
	"Host is down",
	"No route to host",
	"Directory not empty",
	"Too many processes",
	"Too many users",
	"Disc quota exceeded",
	"Stale NFS file handle",
	"Too many levels of remote in path",
	"RPC struct is bad",
	"RPC version wrong",
	"RPC prog. not avail",
	"Program version wrong",
	"Bad procedure for program",
	"No locks available",
	"Function not implemented",
	"Inappropriate file type or format",
	0
};


static clock_t gMacPerl_StartClock = 0;

clock_t MacPerl_times(struct tms * t)
{
	t->tms_utime = clock() - gMacPerl_StartClock;
	t->tms_stime = 0;
	t->tms_cutime = 0;
	t->tms_cstime = 0;
	
	return t->tms_utime;
}

void MacPerl_init()
{
	gMacPerl_StartClock = clock();
}

static const char * strnstr(const char * msg, const char * str, int len)
{
	char s1 = *str++;
	
	while (len-- > 0)
		if (*msg++ == s1) {
			const char * s = str;
			const char * m = msg;
			
			while (*s && *s == *m)
				++s, ++m;
			
			if (!*s)
				return msg-1;
		}
	return NULL;
}

static void WriteMsgLn(PerlIO * io, const char * msg, size_t len, Boolean start)
{
	if (start)
		PerlIO_write(io, "# ", 2);
	PerlIO_write(io, msg, len);
}

static void WriteMsg(PerlIO * io, const char * msg, size_t len, Boolean start)
{
	const char * nl;
	
	while (nl = (const char *)memchr(msg, '\n', len)) {
		WriteMsgLn(io, msg, nl-msg+1, start);
		start 	= true;
		len		= msg+len-nl-1;
		msg 	= nl+1;
	}
	if (len)
		WriteMsgLn(io, msg, len, start);
}

static Boolean gMacPerl_ErrorFormat = false;

void MacPerl_WriteMsg(void * io, const char * msg, size_t len)
{
	const char * line= msg;
	const char * at;

	if (!gMacPerl_ErrorFormat) {
		PerlIO_write(io, msg, len);
		return;
	}

	/* Look for " line \d" */
	while (line = strnstr(line+1, " line ", msg+len-line-1)) {
		/* Invariants:
		 * To process: [msg, msg+len[
		 * msg < line < msg+len
		 */
		if (line[6] >= '0' && line[6] <= '9') {
			/* Got line, now look for end of line number */
			const char * endline = line+7;
			const char * newline;
			
			while (*endline >= '0' && *endline <= '9' && endline < msg+len)
				++endline;
			if (*endline == ' ' && endline < msg+len)
				++endline;
			for (newline = endline; *newline != '\n' && newline < msg+len; ++newline)
				;
			if (*newline == '\n' && newline < msg+len)
				++newline;
				
			/* Got it, now look for preceding " at ." length reduced by 4 because file name
			 * must be at least 1 character long.
			 */
			at	= strnstr(msg, " at ", line-msg-4);
	
			if (at) {
				/* msg <= at < line */
				const char * anotherat;
				
				/* Look for intervening "at". This part gives misleading results if the filename
				 * itself contains an at.
				 */
				while (anotherat = strnstr(at+4, " at ", line-at-5))
					at = anotherat;
					
				/* OK, we got them both, write the original message prefixed with # */
				WriteMsg(io, msg, at-msg, true);
				if (newline > endline) {
					switch (*endline) {
					case ',':
					case '.':
					case ';':
					case '\t':
					case '\n':
						break;
					default:
						PerlIO_write(io, " ", 1);
						break;
					}
					PerlIO_write(io, endline, newline-endline);
				}
				PerlIO_write(io, "File \'", 6);
				PerlIO_write(io, at+4, line-at-4);
				PerlIO_write(io, "\'; Line ", 8);
				PerlIO_write(io, line+6, endline-line-6);
				PerlIO_write(io, "\n", 1);
				
				len    -= newline-msg;
				msg 	= newline;
				line	= msg;
				
				if (!len)	/* We're done */
					return;
			}
		}
	}
	/* No file/line found */
	WriteMsg(io, msg, len, true);
}

