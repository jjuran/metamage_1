#include	"sfhdr.h"

/*	Invoke event handlers for a stream
**
**	Written by Kiem-Phong Vo (05/21/96)
*/
#if __STD_C
int sfraise(Sfio_t* f, int type, Void_t* data)
#else
int sfraise(f, type, data)
Sfio_t*	f;	/* stream		*/
int	type;	/* type of event	*/
Void_t*	data;	/* associated data	*/
#endif
{
	reg Sfdisc_t	*disc, *next, *d;
	reg int		local, rv;

	GETLOCAL(f,local);
	if(!SFKILLED(f) &&
	   !(local && (type == SF_NEW || type == SF_CLOSE || type == SF_FINAL)) &&
	   SFMODE(f,local) != (f->mode&SF_RDWR) && _sfmode(f,0,local) < 0)
		return -1;
	SFLOCK(f,local);

	for(disc = f->disc; disc; )
	{	next = disc->disc;

		if(disc->exceptf)
		{	SFOPEN(f,0);
			if((rv = (*disc->exceptf)(f,type,data,disc)) != 0 )
				return rv;
			SFLOCK(f,0);
		}

		if((disc = next) )
		{	/* make sure that "next" hasn't been popped */
			for(d = f->disc; d; d = d->disc)
				if(d == disc)
					break;
			if(!d)
				disc = f->disc;
		}
	}

	SFOPEN(f,local);
	return 0;
}
