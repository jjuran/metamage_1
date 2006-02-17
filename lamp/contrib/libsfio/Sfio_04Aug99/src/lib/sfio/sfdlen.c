#include	"sfhdr.h"

/*	Return the length of a double value if coded in a portable format
**
**	Written by Kiem-Phong Vo (08/05/90)
*/

#if __STD_C
int _sfdlen(reg Sfdouble_t v)
#else
int _sfdlen(v)
reg Sfdouble_t	v;
#endif
{
#define N_ARRAY		(16*sizeof(Sfdouble_t))
	reg int		n, w;
	reg Sfdouble_t	x;
	int		exp;

	if(v < 0)
		v = -v;

	/* make the magnitude of v < 1 */
	if(v != 0.)
		v = frexp(v,&exp);
	else	exp = 0;

	for(w = 1; w <= N_ARRAY; ++w)
	{	/* get 2^SF_PRECIS precision at a time */
		n = (int)(x = ldexp(v,SF_PRECIS));
		v = x-n;
		if(v <= 0.)
			break;
	}

	return 1 + sfulen(exp) + w;
}
