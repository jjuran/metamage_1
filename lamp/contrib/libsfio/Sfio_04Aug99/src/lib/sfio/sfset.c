#include	"sfhdr.h"

/*	Set some control flags or file descript for the stream
**
**	Written by Kiem-Phong Vo (07/16/90)
*/

#if __STD_C
int sfset(reg Sfio_t* f, reg int flags, reg int set)
#else
int sfset(f,flags,set)
reg Sfio_t*	f;
reg int		flags;
reg int		set;
#endif
{
	reg int	oflags;

	if(flags == 0 && set == 0)
		return (f->flags&SF_FLAGS);

	if((oflags = (f->mode&SF_RDWR)) != (int)f->mode && _sfmode(f,oflags,0) < 0)
		return 0;

	if(flags == 0)
		return (f->flags&SF_FLAGS);

	SFLOCK(f,0);

	/* preserve at least one rd/wr flag */
	oflags = f->flags;
	if(!(f->bits&SF_BOTH) || (flags&SF_RDWR) == SF_RDWR )
		flags &= ~SF_RDWR;

	/* make sure that mapped area has the right mode */
#ifdef MAP_TYPE
	if(f->data && (f->bits&SF_MMAP) && (flags&SF_BUFCONST) &&
	   ((set && !(f->flags&SF_BUFCONST)) || (!set && (f->flags&SF_BUFCONST)) ) )
	{	f->here -= f->endb-f->next;
		SFSK(f,f->here,0,f->disc);
		SFMUNMAP(f,f->data,f->endb-f->data);
		f->endb = f->endr = f->endw = f->next = f->data = NIL(uchar*);
	}
#endif

	/* set the flag */
	if(set)
		f->flags |=  (flags&SF_SETS);
	else	f->flags &= ~(flags&SF_SETS);

	/* must have at least one of read/write */
	if(!(f->flags&SF_RDWR))
		f->flags |= (oflags&SF_RDWR);

	if(f->extent < 0)
		f->flags &= ~SF_APPENDWR;

	/* turn to appropriate mode as necessary */
	if((flags &= SF_RDWR) )
	{	if(!set)
		{	if(flags == SF_READ)
				flags = SF_WRITE;
			else	flags = SF_READ;
		}
		if((flags == SF_WRITE && !(f->mode&SF_WRITE)) ||
		   (flags == SF_READ && !(f->mode&(SF_READ|SF_SYNCED))) )
			(void)_sfmode(f,flags,1);
	}

	/* if not shared or unseekable, public means nothing */
	if(!(f->flags&SF_SHARE) || f->extent < 0)
		f->flags &= ~SF_PUBLIC;

	SFOPEN(f,0);
	return (oflags&SF_FLAGS);
}
