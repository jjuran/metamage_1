#include	"sfhdr.h"

#undef sfgetl

#if __STD_C
Sflong_t sfgetl(reg Sfio_t* f)
#else
Sflong_t sfgetl(f)
reg Sfio_t*	f;
#endif
{
	return __sf_getl(f);
}
