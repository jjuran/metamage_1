#include	"sfhdr.h"

#undef sffileno

#if __STD_C
int sffileno(reg Sfio_t* f)
#else
int sffileno(f)
reg Sfio_t*	f;
#endif
{
	return __sf_fileno(f);
}
