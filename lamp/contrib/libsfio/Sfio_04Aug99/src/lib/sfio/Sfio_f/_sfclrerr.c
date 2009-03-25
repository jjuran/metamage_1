#include	"sfhdr.h"

#undef sfclrerr

#if __STD_C
int sfclrerr(reg Sfio_t* f)
#else
int sfclrerr(f)
reg Sfio_t*	f;
#endif
{
	return __sf_clrerr(f);
}
