#include	"sfhdr.h"

/*	Read an unsigned long value coded in a portable format.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
Sfulong_t _sfgetu(reg Sfio_t* f)
#else
Sfulong_t _sfgetu(f)
reg Sfio_t*	f;
#endif
{
	reg Sfulong_t	v;
	reg uchar	*s, *ends, c;
	reg int		p;

	if(f->mode != SF_READ && _sfmode(f,SF_READ,0) < 0)
		return (ulong)(-1L);

	SFLOCK(f,0);

	v = SFUVALUE(f->val);
	for(;;)
	{	if(SFRPEEK(f,s,p) <= 0)
		{	f->flags |= SF_ERROR;
			v = (ulong)(-1L);
			goto done;
		}
		for(ends = s+p; s < ends;)
		{	c = *s++;
			v = (v << SF_UBITS) | SFUVALUE(c);
			if(!(c&SF_MORE))
			{	f->next = s;
				goto done;
			}
		}
		f->next = s;
	}
done:
	SFOPEN(f,0);
	return v;
}
