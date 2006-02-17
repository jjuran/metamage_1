#include	"sfhdr.h"

/*	Open a stream given a file descriptor.
**
**	Written by Kiem-Phong Vo (06/27/90).
*/

#if __STD_C
Sfio_t* _stdopen(reg int fd, reg const char* mode)
#else
Sfio_t* _stdopen(fd,mode)
reg int		fd;
reg char*	mode;
#endif
{
	int		sflags;

	if(fd < 0 || (sflags = _sftype(mode,NIL(int*))) == 0)
		return NIL(Sfio_t*);
	else	return sfnew(NIL(Sfio_t*),NIL(Void_t*),(size_t)SF_UNBOUND,fd,sflags);
}
