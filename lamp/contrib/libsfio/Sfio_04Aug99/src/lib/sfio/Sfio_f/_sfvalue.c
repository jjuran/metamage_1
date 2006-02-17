#include	"sfhdr.h"

#undef sfvalue

#if __STD_C
ssize_t sfvalue(reg Sfio_t* f)
#else
ssize_t sfvalue(f)
reg Sfio_t*	f;
#endif
{
	return __sf_value(f);
}
