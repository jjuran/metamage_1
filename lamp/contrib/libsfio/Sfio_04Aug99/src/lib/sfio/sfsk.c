#include	"sfhdr.h"

/*	Seek function that knows discipline
**
**	Written by Kiem-Phong Vo (02/12/91)
*/
#if __STD_C
Sfoff_t sfsk(reg Sfio_t* f, reg Sfoff_t addr, reg int type, Sfdisc_t* disc)
#else
Sfoff_t sfsk(f,addr,type,disc)
reg Sfio_t*	f;
reg Sfoff_t	addr;
reg int		type;
Sfdisc_t*	disc;
#endif
{
	reg Sfoff_t	p;
	reg Sfdisc_t*	dc;
	reg ssize_t	s;
	reg int		local;

	GETLOCAL(f,local);
	if(!local && !(f->mode&SF_LOCK))
		return -1;

	for(;;)
	{	dc = disc;
		if(f->flags&SF_STRING)
		{	SFSTRSIZE(f);
			if(type == 0)
				s = (ssize_t)addr;
			else if(type == 1)
				s = (ssize_t)(addr + f->here);
			else	s = (ssize_t)(addr + f->extent);
		}
		else
		{	SFDISC(f,dc,seekf,local);
			if(dc && dc->seekf)
				p = (*(dc->seekf))(f,addr,type,dc);
			else	p = lseek(f->file,(off_t)addr,type);
			if(p >= 0)
				return p;
			s = -1;
		}

		if(local)
			SETLOCAL(f);
		switch(_sfexcept(f,SF_SEEK,s,dc))
		{
		case SF_EDISC:
		case SF_ECONT:
			if(f->flags&SF_STRING)
				return 0;
			goto do_continue;
		default:
			return -1;
		}

	do_continue:
		for(dc = f->disc; dc; dc = dc->disc)
			if(dc == disc)
				break;
		disc = dc;
	}
}
