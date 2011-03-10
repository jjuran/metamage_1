#include	"sfhdr.h"
#include	"stdio.h"


/*	sprintf function
**
**	Written by Kiem-Phong Vo (12/10/90)
*/


#if __STD_C
int _stdsprintf(char* s, const char* form, ...)
#else
int _stdsprintf(va_alist)
va_dcl
#endif
{
	va_list	args;
	reg int	rv;

#if __STD_C
	va_start(args,form);
#else
	reg char*	s;
	reg char*	form;
	va_start(args);
	s = va_arg(args,char*);
	form = va_arg(args,char*);
#endif

	rv = s ? sfvsprintf(s,SF_BUFSIZE,form,args) : -1;

	va_end(args);

	return rv;
}
