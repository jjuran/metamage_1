#include	"sfhdr.h"

#undef sferror

#if __STD_C
int sferror(reg Sfio_t* f)
#else
int sferror(f)
reg Sfio_t*	f;
#endif
{
	return __sf_error(f);
}
