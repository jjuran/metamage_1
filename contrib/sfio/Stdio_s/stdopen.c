#include	"sfhdr.h"
#include "stdio.h"

/*	Open a stream given a file descriptor.
**
**	Written by Kiem-Phong Vo (06/27/90).
*/

Sfio_t* _stdopen(int fd, const char* mode)
{
	int		sflags;

	if(fd < 0 || (sflags = _sftype(mode,NIL(int*))) == 0)
		return NIL(Sfio_t*);
	else	return sfnew(NIL(Sfio_t*),NIL(Void_t*),(size_t)SF_UNBOUND,fd,sflags);
}
