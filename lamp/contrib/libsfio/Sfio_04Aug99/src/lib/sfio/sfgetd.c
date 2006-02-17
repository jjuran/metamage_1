#include	"sfhdr.h"

/*	Read a portably coded double value
**
**	Written by Kiem-Phong Vo (08/05/90)
*/

#if __STD_C
Sfdouble_t sfgetd(Sfio_t* f)
#else
Sfdouble_t sfgetd(f)
Sfio_t*	f;
#endif
{
	reg uchar	*s, *ends, c;
	reg int		p, sign, exp;
	reg double	v;

	if((sign = sfgetc(f)) < 0 || (exp = (int)sfgetu(f)) < 0)
		return -1.;

	if(f->mode != SF_READ && _sfmode(f,SF_READ,0) < 0)
		return -1.;

	SFLOCK(f,0);

	v = 0.;
	for(;;)
	{	/* fast read for data */
		if(SFRPEEK(f,s,p) <= 0)
		{	f->flags |= SF_ERROR;
			v = -1.;
			goto done;
		}

		for(ends = s+p; s < ends; )
		{	c = *s++;
			v += SFUVALUE(c);
			v = ldexp(v,-SF_PRECIS);
			if(!(c&SF_MORE))
			{	f->next = s;
				goto done;
			}
		}
		f->next = s;
	}

done:
	v = ldexp(v,(sign&02) ? -exp : exp);
	if(sign&01)
		v = -v;

	SFOPEN(f,0);
	return (Sfdouble_t)v;
}
