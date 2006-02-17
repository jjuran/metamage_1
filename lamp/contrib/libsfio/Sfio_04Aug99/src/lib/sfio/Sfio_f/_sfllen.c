#include	"sfhdr.h"

#undef sfllen

#if __STD_C
int sfllen(reg Sflong_t v)
#else
int sfllen(v)
reg Sflong_t	v;
#endif
{
	return __sf_llen(v);
}
