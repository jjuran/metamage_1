#include	"sfhdr.h"

/*	Delete all pending data in the buffer
**
**	Written by Kiem-Phong Vo (07/08/91)
*/

#if __STD_C
int sfpurge(reg Sfio_t* f)
#else
int sfpurge(f)
reg Sfio_t*	f;
#endif
{
	reg int	mode;

	if((mode = f->mode&SF_RDWR) != (int)f->mode && _sfmode(f,mode,0) < 0)
		return -1;

	if(f->disc == _Sfudisc)
		(void)sfclose((*_Sfstack)(f,NIL(Sfio_t*)));

	/* cannot purge read string streams */
	if((f->flags&SF_STRING) && (f->mode&SF_READ) )
		goto done;

	SFLOCK(f,0);

	/* if memory map must be a read stream, pretend data is gone */
#ifdef MAP_TYPE
	if(f->bits&SF_MMAP)
	{	f->here -= f->endb - f->next;
		if(f->data)
		{	(void)munmap((caddr_t)f->data,f->endb-f->data);
			SFSK(f,f->here,0,f->disc);
			f->endb = f->endr = f->endw =
			f->next = f->data = NIL(uchar*);
		}

		SFOPEN(f,0);
		return 0;
	}
#endif

	switch(f->mode&~SF_LOCK)
	{
	default :
		return -1;
	case SF_WRITE :
		f->next = f->data;
		if(!(f->bits&SF_PROCESS) || !(f->flags&SF_READ) || !(f->mode&SF_WRITE) )
			break;

		/* 2-way pipe, must clear read buffer */
		(void)_sfmode(f,SF_READ,1);
		/* fall through */
	case SF_READ:
		if(f->extent >= 0 && f->endb > f->next)
		{	f->here -= f->endb-f->next;
			SFSK(f,f->here,0,f->disc);
		}
		f->endb = f->next = f->data;
		break;
	}

	SFOPEN(f,0);

done:
	if((f->flags&SF_IOCHECK) && f->disc && f->disc->exceptf)
		(void)(*f->disc->exceptf)(f,SF_PURGE,NIL(Void_t*),f->disc);

	return 0;
}
