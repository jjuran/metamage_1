#include	"sfhdr.h"

/*	Read formated data from a stream
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
int sfscanf(Sfio_t* f, const char* form, ...)
#else
int sfscanf(va_alist)
va_dcl
#endif
{
	va_list	args;
	reg int	rv;

#if __STD_C
	va_start(args,form);
#else
	reg Sfio_t*	f;
	reg char*	form;
	va_start(args);
	f = va_arg(args,Sfio_t*);
	form = va_arg(args,char*);
#endif

	rv = sfvscanf(f,form,args);
	va_end(args);
	return rv;
}

#if __STD_C
int sfvsscanf(const char* s, const char* form, va_list args)
#else
int sfvsscanf(s, form, args)
char*	s;
char*	form;
va_list	args;
#endif
{
	Sfio_t	f;

	if(!s)
		return -1;

	/* make a fake stream */
	SFCLEAR(&f);
	f.flags = SF_STRING|SF_READ;
	f.mode = SF_READ;
	f.size = strlen((char*)s);
	f.data = f.next = f.endw = (uchar*)s;
	f.endb = f.endr = f.data+f.size;

	return sfvscanf(&f,form,args);
}

#if __STD_C
int sfsscanf(const char* s, const char* form,...)
#else
int sfsscanf(va_alist)
va_dcl
#endif
{
	va_list		args;
	reg int		rv;
#if __STD_C
	va_start(args,form);
#else
	reg char*	s;
	reg char*	form;
	va_start(args);
	s = va_arg(args,char*);
	form = va_arg(args,char*);
#endif

	rv = sfvsscanf(s,form,args);
	va_end(args);

	return rv;
}
