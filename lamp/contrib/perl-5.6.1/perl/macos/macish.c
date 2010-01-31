/*********************************************************************
Project	:	Perl5				-	
File	:	macish.c			-	Mac specific things
Author	:	Matthias Neeracher

*********************************************************************/

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

void Lamp_async_check()
{
	static unsigned long global_counter = 0;
	
	if ( ++global_counter & 0x00100000 )
	{
		// kill() is a guaranteed preemption point.
		// kill( 1, 0 ) is the canonical 'yield' idiom.
		
		kill( 1, 0 );
	}
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
	
	while ((nl = (const char *)memchr(msg, '\n', len))) {
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
	while ((line = strnstr(line+1, " line ", msg+len-line-1))) {
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
				while ((anotherat = strnstr(at+4, " at ", line-at-5)))
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

