#include	"sfhdr.h"

/*	Internal function to do a hard read.
**	This knows about discipline and memory mapping, peek read.
**
**	Written by Kiem-Phong Vo (02/11/91)
*/

/* synchronize unseekable write streams */
#if __STD_C
static void _sfwrsync(void)
#else
static void _sfwrsync()
#endif
{	reg Sfpool_t*	p;
	reg Sfio_t*	f;
	reg int		n;

	/* sync all pool heads */
	for(p = _Sfpool.next; p; p = p->next)
	{	if(p->n_sf <= 0)
			continue;
		f = p->sf[0];
		if(!SFFROZEN(f) && f->next > f->data &&
		   (f->mode&SF_WRITE) && f->extent < 0 )
			(void)_sfflsbuf(f,-1);
	}

	/* and all the ones in the discrete pool */
	for(n = 0; n < _Sfpool.n_sf; ++n)
	{	f = _Sfpool.sf[n];
		if(!SFFROZEN(f) && f->next > f->data &&
		   (f->mode&SF_WRITE) && f->extent < 0 )
			(void)_sfflsbuf(f,-1);
	}
}

#if __STD_C
ssize_t sfrd(reg Sfio_t* f, reg Void_t* buf, reg size_t n, Sfdisc_t* disc)
#else
ssize_t sfrd(f,buf,n,disc)
reg Sfio_t*	f;
reg Void_t*	buf;
reg size_t	n;
Sfdisc_t*	disc;
#endif
{
	reg Sfoff_t	r;
	reg Sfdisc_t*	dc;
	reg int		local, rcrv, dosync, oerrno;

	GETLOCAL(f,local);
	if((rcrv = f->mode & (SF_RC|SF_RV)) )
		f->mode &= ~(SF_RC|SF_RV);

	if((f->mode&SF_PKRD) || (!local && !(f->mode&SF_LOCK)) )
		return -1;

	for(dosync = 0;;)
	{	/* stream locked by sfsetfd() */
		if(!(f->flags&SF_STRING) && f->file < 0)
			return 0;

		f->flags &= ~(SF_EOF|SF_ERROR);

		dc = disc;
		if(f->flags&SF_STRING)
		{	if((r = (f->data+f->extent) - f->next) < 0)
				r = 0;
			if(r > 0)
				return (ssize_t)r;
			else	goto do_except;
		}

		/* warn that a read is about to happen */
		SFDISC(f,dc,readf,local);
		if(dc && dc->exceptf && (f->flags&SF_IOCHECK) )
		{	reg int	rv;
			if(local)
				SETLOCAL(f);
			if((rv = _sfexcept(f,SF_READ,n,dc)) > 0)
				n = rv;
			else if(rv < 0)
			{	f->flags |= SF_ERROR;
				return (ssize_t)rv;
			}
		}

#ifdef MAP_TYPE
		if(f->bits&SF_MMAP)
		{	reg ssize_t	a, mmsz, round;
			reg uchar*	data;
			reg int		prot, flags;
			Stat_t		st;

			/* determine if we have to copy data to buffer */
			if((uchar*)buf >= f->data && (uchar*)buf <= f->endb)
			{	n += f->endb - f->next;
				buf = NIL(char*);
			}

			/* actual seek location */
			if((f->flags&(SF_SHARE|SF_PUBLIC)) == (SF_SHARE|SF_PUBLIC) &&
			   (r = SFSK(f,(Sfoff_t)0,1,dc)) != f->here)
				f->here = r;
			else	f->here -= f->endb-f->next;

			/* before mapping, make sure we have data to map */
			if((f->flags&SF_SHARE) || (size_t)(r = f->extent-f->here) < n)
			{	if((r = fstat(f->file,&st)) < 0)
					goto do_except;
				if((r = (f->extent = st.st_size) - f->here) <= 0 )
				{	r = 0;	/* eof */
					goto do_except;
				}
			}

			/* make sure current position is page aligned */
			if((a = (size_t)(f->here%_Sfpage)) != 0)
			{	f->here -= a;
				r += a;
			}

			/* map minimal requirement */
			if(r > (round = (1 + (n+a)/f->size)*f->size) )
				r = round;

			mmsz = 0;
			if((data = f->data) &&
			   (!_mmap_fixed || r != (mmsz = f->endb-data)) )
			{	f->endb = f->endr = f->endw =
				f->next = f->data = NIL(uchar*);
				SFMUNMAP(f,data,mmsz);
				data = NIL(uchar*);
			}

			prot = PROT_READ | ((f->flags&SF_BUFCONST) ? 0 : PROT_WRITE);
			flags = (f->flags&SF_BUFCONST) ? MAP_SHARED : MAP_PRIVATE;
			for(;;)
			{	f->data = (uchar*) mmap((caddr_t)data, (size_t)r, prot,
						(flags|(data ? MAP_FIXED : MAP_VARIABLE)),
						f->file, (off_t)f->here);
				if((caddr_t)f->data != (caddr_t)(-1))
					break;
				if(data)
				{	(void)munmap((caddr_t)data,mmsz);
					f->endb = f->endr = f->endw =
					f->next = f->data = data = NIL(uchar*);
				}
				if((size_t)(r >>= 1) < (_Sfpage*SF_NMAP) ||
				   (errno != EAGAIN && errno != ENOMEM) )
					break;
			}

			if((caddr_t)f->data != (caddr_t)(-1) )
			{	if(f->bits&SF_SEQUENTIAL)
					SFMMSEQON(f,data,r);
				f->next = f->data+a;
				f->endr = f->endb = f->data+r;
				f->endw = f->data;
				f->here += r;

				/* if share-public, make known our seek location */
				if((f->flags&(SF_SHARE|SF_PUBLIC)) ==
				   (SF_SHARE|SF_PUBLIC))
					(void)SFSK(f,f->here,0,dc);

				if(buf)
				{	if(n > (size_t)(r-a))
						n = (ssize_t)(r-a);
					memcpy(buf,f->next,n);
					f->next += n;
					return n;
				}
				else	return f->endb-f->next;
			}
			else
			{	r = -1;
				f->here += a;
				f->data = NIL(uchar*);

				/* reset seek pointer to its physical location */
				(void)SFSK(f,f->here,0,dc);

				/* make a buffer */
				(void)SFSETBUF(f,(Void_t*)f->tiny,(size_t)SF_UNBOUND);

				if(!buf)
				{	buf = (Void_t*)f->data;
					n = f->size;
				}
			}
		}
#endif

		/* sync unseekable write streams to prevent deadlock */
		if(!dosync && f->extent < 0)
		{	dosync = 1;
			_sfwrsync();
		}

		/* make sure file pointer is right */
		if(f->extent >= 0 && (f->flags&SF_SHARE) )
		{	if(!(f->flags&SF_PUBLIC) )
				f->here = SFSK(f,f->here,0,dc);
			else	f->here = SFSK(f,(Sfoff_t)0,1,dc);
		}

		oerrno = errno;
		errno = 0;

		if(dc && dc->readf)
		{	int	share = f->flags&SF_SHARE;

			if(rcrv) /* pass on rcrv for possible continuations */
				f->mode |= rcrv;
				/* tell readf that no peeking necessary */
			else	f->flags &= ~SF_SHARE;

			r = (*(dc->readf))(f,buf,n,dc);

			/* reset flags */
			if(rcrv)
				f->mode &= ~rcrv;
			else	f->flags |= share;
		}
		else if(SFISNULL(f))
			r = 0;
		else if(f->extent < 0 && (f->flags&SF_SHARE) && rcrv)
		{	/* try peek read */
			r = sfpkrd(f->file, (char*)buf, n,
				    (rcrv&SF_RC) ? (int)f->getr : -1,
				    -1L, (rcrv&SF_RV) ? 1 : 0);
			if(r > 0)
			{	if(rcrv&SF_RV)
					f->mode |= SF_PKRD;
				else	f->mode |= SF_RC;
			}
		}
		else	r = read(f->file,buf,n);

		if(errno == 0 )
			errno = oerrno;

		if(local && r > 0)
		{	if(!(f->mode&SF_PKRD))
			{	f->here += r;
				if(f->extent >= 0 && f->extent < f->here)
					f->extent = f->here;
			}
			if((uchar*)buf >= f->data && (uchar*)buf < f->data+f->size)
				f->endb = f->endr = ((uchar*)buf) + r;
		}

		if(r > 0)
			return (ssize_t)r;

	do_except:
		if(local)
			SETLOCAL(f);
		switch(_sfexcept(f,SF_READ,(ssize_t)r,dc))
		{
		case SF_ECONT :
			goto do_continue;
		case SF_EDONE :
			return local ? 0 : (ssize_t)r;
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
