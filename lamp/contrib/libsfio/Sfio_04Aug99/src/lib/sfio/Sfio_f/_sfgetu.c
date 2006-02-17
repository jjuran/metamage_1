#include	"sfhdr.h"

#undef sfgetu

#if __STD_C
Sfulong_t sfgetu(reg Sfio_t* f)
#else
Sfulong_t sfgetu(f)
reg Sfio_t*	f;
#endif
{
	return __sf_getu(f);
}
