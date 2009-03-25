#include	"sfhdr.h"

#undef sfslen

#if __STD_C
ssize_t sfslen(void)
#else
ssize_t sfslen()
#endif
{
	return __sf_slen();
}
