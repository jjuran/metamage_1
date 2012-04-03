#include	"sfhdr.h"

/*	Set the IO pointer to a specific location in the stream
**
**	Written by Kiem-Phong Vo (03/24/98)
*/

#if __STD_C
static void newpos(Sfio_t* f, Sfoff_t p)
#else
static void newpos(f, p)
Sfio_t*	f;
Sfoff_t p;
#endif
{
#ifdef MAP_TYPE
	if((f->bits&SF_MMAP) && f->data)
	{	SFMUNMAP(f, f->data, f->endb-f->data);
		f->data = NIL(uchar*);
	}
#endif
	f->next = f->endr = f->endw = f->data;
	f->endb = (f->mode&SF_WRITE) ? f->data+f->size : f->data;
	if((f->here = p) < 0)
	{	f->extent = -1;
		f->here = 0;
	}
}

#if __STD_C
Sfoff_t sfseek(reg Sfio_t* f, reg Sfoff_t p, reg int type)
#else
Sfoff_t sfseek(f,p,type)
reg Sfio_t	*f;	/* seek to a new location in this stream */
reg Sfoff_t	p;	/* place to seek to */
int		type;	/* 0: from org, 1: from here, 2: from end */
#endif
{
	reg Sfoff_t	r, s;
	reg int		mode, local, public;

	GETLOCAL(f,local);

	/* set and initialize the stream to a definite mode */
	if((int)SFMODE(f,local) != (mode = f->mode&SF_RDWR) && _sfmode(f,mode,local) < 0 )
		return -1;

	public = ((type|f->flags)&SF_PUBLIC); type &= ~SF_PUBLIC;

	if(type < 0 || type > 2 || f->extent < 0)
		return -1;

	/* throw away ungetc data */
	if(f->disc == _Sfudisc)
		(void)sfclose((*_Sfstack)(f,NIL(Sfio_t*)));

	/* lock the stream for internal manipulations */
	SFLOCK(f,local);

	/* clear error and eof bits */
	f->flags &= ~(SF_EOF|SF_ERROR);

	while(f->flags&SF_STRING)
	{	SFSTRSIZE(f);

		if(type == 1)
			r = p + (f->next - f->data);
		else if(type == 2)
			r = p + f->extent;
		else	r = p;

		if(r >= 0 && r <= f->size)
		{	p = r;
			f->next = f->data+p;
			f->here = p;
			if(p > f->extent)
				memclear((char*)(f->data+f->extent),(int)(p-f->extent));
			goto done;
		}

		/* check exception handler, note that this may pop stream */
		if(SFSK(f,r,0,f->disc) != r)
		{	p = -1;
			goto done;
		}
	}

	if(f->mode&SF_WRITE)
	{	if(type < 2 && !public && !(f->flags&SF_SHARE) )
		{	s = f->here + (f->next - f->data);	
			r = p + (type == 0 ? 0 : s);
			if(r == s)
			{	p = r;
				goto done;
			}
		}

		if(f->next > f->data && SFSYNC(f) < 0)
		{	p = -1;
			goto done;
		}
	}

	if(type == 2)	/* seeking from the end */
	{	p = SFSK(f, p, 2, f->disc);
		newpos(f,p);
		goto done;
	}

	/* figure out the current logical stream position */
	if(public && (r = SFSK(f, 0, 1, f->disc)) != f->here)
	{	newpos(f,r);
		if(r < 0)
		{	p = -1;
			goto done;
		}
	}
	s = f->here + ((f->mode&SF_READ) ? f->next-f->endb : 0);

	/* location to seek to */
	if((p += type == 0 ? 0 : s) < 0)
		goto done;

	if(type == 1 && p == s) /* only ascertain location */
	{	if(!public && (r = SFSK(f, f->here, 0, f->disc)) != f->here)
		{	newpos(f,r);
			p = -1;
		}
		goto done;
	}

	if((f->mode&SF_WRITE) || (f->flags&SF_RDWR) == SF_RDWR )
	{	if((p = SFSK(f, p, 0, f->disc)) >= 0 )
			newpos(f,p);
		goto done;
	}

#ifdef MAP_TYPE
	if(f->bits&SF_MMAP)
	{	if(p >= (f->here - (r = f->endb-f->data)) && p < f->here)
			f->next = f->endb - (f->here - p);
		else
		{	newpos(f,p);

			if(p >= s || p < (s-r))
				f->here = p;
			else /* small back seek is bad for mmap */
			{	SFSETBUF(f,(Void_t*)f->tiny,(size_t)SF_UNBOUND);
				if((p%_Sfpage) != 0)
				{	f->here = s;
					goto do_align;
				}
				if((r = SFSK(f,p,0,f->disc)) != f->here)
				{	newpos(f,r);
					p = -1;
				}
			}
		}

		goto done;
	}
#endif

	if(p > f->here) /* move forward by filling rest of buffer */
	{	if(f->next == f->endb)
			f->endb = f->endr = f->next = f->data;
		if(p < (s + (f->size - (f->endb-f->data))))
			(void)SFFILBUF(f,-1);
	}

	if(p <= f->here && p >= (f->here - (f->endb-f->data)) )
	{	/* still in buffer boundary, move pointer only */
		f->next = f->endb - (f->here-p);
		goto done;
	}

do_align:
	/* seek to a rounded boundary to improve read performance.
	** The use of SF_PAGE and _Sfpage below is to optimize
	** seeks & small reads. Reading ahead large amounts after a seek
	** could be too aggressive and wasteful. So we may defer this to
	** sfread() and other input functions unless non-alignment forces
	** us to have to read here.
	*/
	if((p%SF_PAGE == 0) || f->size < _Sfpage || p >= f->extent)
		r = p;
	else	r = (p/_Sfpage)*_Sfpage;

	if(r != f->here)
	{	if(SFSK(f,r,0,f->disc) == r)
		{	f->next = f->endr = f->endb = f->data;
			f->here = r;
			if(r == p)	/* defer reading */
				goto done;
		}
		else
		{	p = f->here = SFSK(f,p,0,f->disc);
			f->next = f->endr = f->endb = f->data;
			goto done;
		}
	}

	/* now read to cover p and a bit more */
	r = (((p > s ? p : s) - r)/_Sfpage + 1)*_Sfpage;

	(void)SFRD(f, f->data, r >= (Sfoff_t)f->size ? f->size : (size_t)r, f->disc);
	if(f->here < p || p < (f->here - (f->endb - f->data)) )
		p = -1;
	else	f->next = f->endb - (int)(f->here-p);

done :
	SFOPEN(f,local);
	return p;
}
