#include	"sfhdr.h"

#undef sfputd

#if __STD_C
int sfputd(reg Sfio_t* f, Sfdouble_t d)
#else
int sfputd(f,d)
reg Sfio_t*	f;
reg Sfdouble_t	d;
#endif
{
	return __sf_putd(f,d);
}
