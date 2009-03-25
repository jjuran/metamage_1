#include	"sfhdr.h"
#include	"stdio.h"
#undef getc

#if __STD_C
int getc(FILE* f)
#else
int getc(f)
FILE*	f;
#endif
{
	return __sf_fgetc(f);
}
