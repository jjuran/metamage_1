#include	"sfhdr.h"

/*	Get size of a long value coded in a portable format
**
**	Written by Kiem-Phong Vo (06/27/90)
*/
#if __STD_C
int _sfllen(reg Sflong_t v)
#else
int _sfllen(v)
reg Sflong_t	v;
#endif
{
	if(v < 0)
		v = -(v+1);
	v = (Sfulong_t)v >> SF_SBITS;
	return 1 + (v > 0 ? sfulen(v) : 0);
}
