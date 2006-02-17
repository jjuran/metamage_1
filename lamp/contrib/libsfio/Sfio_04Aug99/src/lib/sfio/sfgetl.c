#include	"sfhdr.h"

/*	Read a long value coded in a portable format.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
Sflong_t _sfgetl(reg Sfio_t* f)
#else
Sflong_t _sfgetl(f)
reg Sfio_t*	f;
#endif
{
	reg Sflong_t	v;
	reg uchar	*s, *ends, c;
	reg int		p;

	if(f->mode != SF_READ && _sfmode(f,SF_READ,0) < 0)
		return (Sflong_t)(-1);
	SFLOCK(f,0);

	v = (Sflong_t)f->val;
	if(!(v&SF_MORE))
	{	/* must be a small negative number */
		v = -SFSVALUE(v)-1;
		goto done;
	}

	v = SFUVALUE(v);
	for(;;)
	{	if(SFRPEEK(f,s,p) <= 0)
		{	f->flags |= SF_ERROR;
			v = (Sflong_t)(-1);
			goto done;
		}
		for(ends = s+p; s < ends;)
		{	c = *s++;
			if(c&SF_MORE)
				v = ((Sfulong_t)v << SF_UBITS) | SFUVALUE(c);
			else
			{	/* special translation for this byte */
				v = ((Sfulong_t)v << SF_SBITS) | SFSVALUE(c);
				f->next = s;
				v = (c&SF_SIGN) ? -v-1 : v;
				goto done;
			}
		}
		f->next = s;
	}
done :
	SFOPEN(f,0);
	return v;
}
