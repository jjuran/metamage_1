#include	"sfhdr.h"

#undef sfgetc

#if __STD_C
int sfgetc(reg Sfio_t* f)
#else
int sfgetc(f)
reg Sfio_t*	f;
#endif
{
	return __sf_getc(f);
}
