#include	"sfhdr.h"

#undef sfputc

#if __STD_C
int sfputc(reg Sfio_t* f, reg int c)
#else
int sfputc(f,c)
reg Sfio_t*	f;
reg int		c;
#endif
{
	return __sf_putc(f,c);
}
