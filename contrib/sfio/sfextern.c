#include	"sfhdr.h"

/*	External variables and functions used only by Sfio
**	Written by Kiem-Phong Vo (06/27/90)
*/

/* global variables used internally to the package */
Sfext_t _Sfextern =
{	0,						/* _Sfpage	*/
	{ NIL(Sfpool_t*), 0, 0, 0, NIL(Sfio_t**) },	/* _Sfpool	*/
	NIL(Sfio_t*),					/* _Sffree	*/
	NIL(Fmt_t*),					/* _Fmtfree	*/
	NIL(int(*)_ARG_((Sfio_t*,int))),		/* _Sfpmove	*/
	NIL(Sfio_t*(*)_ARG_((Sfio_t*, Sfio_t*))),	/* _Sfstack	*/
	NIL(void(*)_ARG_((Sfio_t*, int, int))),		/* _Sfnotify	*/
	NIL(int(*)_ARG_((Sfio_t*))),			/* _Sfstdio	*/
	{ NIL(Sfread_f),				/* _Sfudisc	*/
	  NIL(Sfwrite_f),
	  NIL(Sfseek_f),
	  NIL(Sfexcept_f),
	  NIL(Sfdisc_t*)
	},
	NIL(void(*)_ARG_((void)) ),			/* _Sfcleanup	*/
	0						/* _Sfexiting	*/
};

/* accessible to application code for a few fast macro functions */
ssize_t	_Sfi = -1;

Sfio_t _Sfstdin  = SFNEW(NIL(char*),-1,0,(SF_READ|SF_STATIC),NIL(Sfdisc_t*));
Sfio_t _Sfstdout = SFNEW(NIL(char*),-1,1,(SF_WRITE|SF_STATIC),NIL(Sfdisc_t*));
Sfio_t _Sfstderr = SFNEW(NIL(char*),-1,2,(SF_WRITE|SF_STATIC),NIL(Sfdisc_t*));

__EXTERN__(ssize_t,_Sfi);
__EXTERN__(Sfio_t,_Sfstdin);
__EXTERN__(Sfio_t,_Sfstdout);
__EXTERN__(Sfio_t,_Sfstderr);
