#include	"sfhdr.h"

/*	Write with discipline.
**
**	Written by Kiem-Phong Vo (02/11/91)
*/

/* hole preserving writes */
#if __STD_C
static ssize_t sfoutput(Sfio_t* f, reg char* buf, reg size_t n)
#else
static ssize_t sfoutput(f,buf,n)
Sfio_t*		f;
reg char*	buf;
reg size_t	n;
#endif
{	reg char	*sp, *wbuf, *endbuf;
	reg ssize_t	s, w, wr;

	s = w = 0;
	wbuf = buf;
	endbuf = buf+n;
	while(n > 0)
	{	if(n < _Sfpage) /* no hole possible */
		{	buf += n;
			s = n = 0;
		}
		else while(n >= _Sfpage)
		{	/* see if a hole of 0's starts here */
			sp = buf+1;
			if(buf[0] == 0 && buf[_Sfpage-1] == 0)
			{	/* check byte at a time until int-aligned */
				while(((Sfulong_t)sp)%sizeof(int))
				{	if(*sp != 0)
						goto chk_hole;
					sp += 1;
				}

				/* check using int to speed up */
				while(sp < endbuf)
				{	if(*((int*)sp) != 0)
						goto chk_hole;
					sp += sizeof(int);
				}

				/* check the remaining bytes */
				if(sp > endbuf)
				{	sp -= sizeof(int);
					while(sp < endbuf)
					{	if(*sp != 0)
							goto chk_hole;
						sp += 1;
					}
				}
			}

		chk_hole:
			if((size_t)(s = sp-buf) >= _Sfpage) /* found a hole */
				break;

			/* skip a dirty page */
			n -= _Sfpage;
			buf += _Sfpage;
		}

		/* write out current dirty pages */
		if(buf > wbuf)
		{	if(n < _Sfpage)
			{	buf = endbuf;
				n = s = 0;
			}
			if((wr = write(f->file,wbuf,buf-wbuf)) > 0)
			{	w += wr;
				f->bits &= ~SF_HOLE;
			}
			if(wr != (buf-wbuf))
				break;
			wbuf = buf;
		}

		/* seek to a rounded boundary within the hole */
		if((size_t)s >= _Sfpage)
		{	s = (s/_Sfpage)*_Sfpage;
			if(SFSK(f,(Sfoff_t)s,1,NIL(Sfdisc_t*)) < 0)
				break;
			w += s;
			n -= s;
			wbuf = (buf += s);
			f->bits |= SF_HOLE;

			if(n > 0)
			{	/* next page must be dirty */
				s = n <= _Sfpage ? 1 : _Sfpage;
				buf += s;
				n -= s;
			}
		}
	}

	return w > 0 ? w : -1;
}

#if __STD_C
ssize_t sfwr(reg Sfio_t* f, reg const Void_t* buf, reg size_t n, reg Sfdisc_t* disc)
#else
ssize_t sfwr(f,buf,n,disc)
reg Sfio_t*	f;
reg Void_t*	buf;
reg size_t	n;
reg Sfdisc_t*	disc;
#endif
{
	reg ssize_t	w;
	reg Sfdisc_t*	dc;
	reg int		local, oerrno;

	GETLOCAL(f,local);
	if(!local && !(f->mode&SF_LOCK))
		return -1;

	for(;;)
	{	/* stream locked by sfsetfd() */
		if(!(f->flags&SF_STRING) && f->file < 0)
			return 0;

		/* clear current error states */
		f->flags &= ~(SF_EOF|SF_ERROR);

		dc = disc;
		if(f->flags&SF_STRING)	/* total required buffer */
			w = n + (f->next - f->data);
		else
		{	/* warn that a write is about to happen */
			SFDISC(f,dc,writef,local);
			if(dc && dc->exceptf && (f->flags&SF_IOCHECK) )
			{	reg int	rv;
				if(local)
					SETLOCAL(f);
				if((rv = _sfexcept(f,SF_WRITE,n,dc)) > 0)
					n = rv;
				else if(rv < 0)
				{	f->flags |= SF_ERROR;
					return rv;
				}
			}

			if(f->extent >= 0)
			{	/* make sure we are at the right place to write */
				if(f->flags&SF_APPENDWR)
				{	if(!_has_oflags)
					{	/* writing at the end of stream */
						if(f->here != f->extent ||
						   (f->flags&SF_SHARE))
							f->here = f->extent =
								SFSK(f,(Sfoff_t)0,2,dc);
					}
				}
				else
				{	if(f->flags&SF_SHARE)
					{	if(!(f->flags&SF_PUBLIC))
							f->here = SFSK(f,f->here,0,dc);
						else	f->here = SFSK(f,(Sfoff_t)0,1,dc);
					}
				}
			}

			oerrno = errno;
			errno = 0;

			if(dc && dc->writef)
				w = (*(dc->writef))(f,buf,n,dc);
			else if(SFISNULL(f))
				w = n;
			else if(f->flags&SF_WHOLE)
				goto do_write;
			else if(n >= _Sfpage && !(f->flags&(SF_SHARE|SF_APPENDWR)) &&
				f->here == f->extent && (f->here%_Sfpage) == 0)
			{	if((w = sfoutput(f,(char*)buf,n)) <= 0)
					goto do_write;
			}
			else
			{
			do_write:
				if((w = write(f->file,(char*)buf,n)) > 0)
					f->bits &= ~SF_HOLE;
			}

			if(errno == 0)
				errno = oerrno;

			if(w > 0)
			{	if(local)
				{	f->here += w;
					if(f->extent >= 0 && f->here > f->extent)
						f->extent = f->here;
				}

				if(_has_oflags && f->extent >= 0 &&
				   (f->flags&SF_APPENDWR) )
					f->here = f->extent = SFSK(f,(Sfoff_t)0,2,dc);

				return w;
			}
		}

		if(local)
			SETLOCAL(f);
		switch(_sfexcept(f,SF_WRITE,w,dc))
		{
		case SF_ECONT :
			goto do_continue;
		case SF_EDONE :
			return local ? 0 : w;
		case SF_EDISC :
			if(!local && !(f->flags&SF_STRING))
				goto do_continue;
			/* else fall thru */
		case SF_ESTACK :
			return -1;
		}

	do_continue:
		for(dc = f->disc; dc; dc = dc->disc)
			if(dc == disc)
				break;
		disc = dc;
	}
}
