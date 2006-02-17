#include	"sfhdr.h"
#include	"stdio.h"

/*	Read formatted data from a stream
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
int _stdscanf(const char* form, ...)
#else
int _stdscanf(va_alist)
va_dcl
#endif
{
	va_list		args;
	reg int		rv;

#if __STD_C
	va_start(args,form);
#else
	reg char	*form;	/* scanning format */
	va_start(args);
	form = va_arg(args,char*);
#endif

	rv = sfvscanf(sfstdin,form,args);
	va_end(args);
	return rv;
}
