#include	"sfhdr.h"

#undef sfeof

#if __STD_C
int sfeof(reg Sfio_t* f)
#else
int sfeof(f)
reg Sfio_t*	f;
#endif
{
	return __sf_eof(f);
}
