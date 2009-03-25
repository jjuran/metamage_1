#include	"sfhdr.h"

#undef sfstacked

#if __STD_C
int sfstacked(reg Sfio_t* f)
#else
int sfstacked(f)
reg Sfio_t*	f;
#endif
{
	return __sf_stacked(f);
}
