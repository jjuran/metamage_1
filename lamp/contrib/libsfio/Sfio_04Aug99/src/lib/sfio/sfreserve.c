#include	"sfhdr.h"

/*	Reserve a segment of data or buffer.
**
**	Written by Kiem-Phong Vo (01/15/93).
*/

#if __STD_C
Void_t* sfreserve(reg Sfio_t* f, ssize_t size, int lock)
#else
Void_t* sfreserve(f,size,lock)
reg Sfio_t*	f;	/* file to peek */
ssize_t		size;	/* size of peek */
int		lock;	/* >0 to lock stream and not advance pointer */
#endif
{
	reg ssize_t	n, sz;
	reg Sfrsrv_t*	frs;
	reg int		mode;

	/* initialize io states */
	frs = NIL(Sfrsrv_t*);
	_Sfi = f->val = -1;

	if((sz = size) == 0 && lock != 0)
	{	/* only return the current status and possibly lock stream */
		if((f->mode&SF_RDWR) != f->mode && _sfmode(f,0,0) < 0)
			return NIL(Void_t*);

		SFLOCK(f,0);
		if((n = f->endb - f->next) < 0)
			n = 0;
		if(lock > 0 && !f->data)
			frs = _sfrsrv(f,0);
		goto done;
	}
	if(sz < 0)
		sz = -sz;

	/* iterate until get to a stream that has data or buffer space */
	for(;;)
	{	/* prefer read mode so that data is always valid */
		if(!(mode = (f->flags&SF_READ)) )
			mode = SF_WRITE;
		if((int)f->mode != mode && _sfmode(f,mode,0) < 0)
		{	n = -1;
			goto done;
		}

		SFLOCK(f,0);

		if((n = f->endb - f->next) < 0)	/* possible for string+rw */
			n = 0;

		if(n > 0 && n >= sz)	/* all done */
			break;

		/* do a buffer refill or flush */
		if(f->mode&SF_WRITE)
			(void)SFFLSBUF(f, -1);
		else if(lock > 0 && f->extent < 0 && (f->flags&SF_SHARE) )
		{	if(n == 0) /* peek-read only if there is no buffered data */
			{	f->mode |= SF_RV;
				(void)SFFILBUF(f, sz == 0 ? -1 : (sz-n) );
			}
			if((n = f->endb - f->next) < sz)
			{	if(f->mode&SF_PKRD)
				{	f->endb = f->endr = f->next;
					f->mode &= ~SF_PKRD;
				}
				goto done;
			}
		}
		else	(void)SFFILBUF(f, sz == 0 ? -1 : (sz-n) );

		/* now have data */
		if((n = f->endb - f->next) > 0)
			break;
		else if(n < 0)
			n = 0;

		/* this test fails only if unstacked to an opposite stream */
		if((f->mode&mode) != 0)
			break;
	}

	if(n > 0 && n < sz && (f->mode&mode) != 0 )
	{	/* try to accomodate request size */	
		if(f->flags&SF_STRING)
		{	if((f->mode&SF_WRITE) && (f->flags&SF_MALLOC) )
			{	(void)SFWR(f,f->next,sz,f->disc);
				n = f->endb - f->next;
			}
		}
		else if(f->mode&SF_WRITE)
		{	if(lock > 0 && (frs = _sfrsrv(f,sz)) )
				n = sz;
		}
		else if(lock <= 0 && f->extent >= 0 && (frs = _sfrsrv(f,sz)) )
		{	reg Sfio_t*	push;

			/* avoid stack popping */
			push = f->push; f->push = NIL(Sfio_t*);

			if((n = SFREAD(f,(Void_t*)frs->data,sz)) < sz )
			{	if(n <= 0)
					n = f->endb - f->next;
				else
				{	if((f->bits&SF_MMAP) || n > f->size )
					{	(void)SFSEEK(f,(Sfoff_t)(-n),1);
						n = (size_t)(f->extent - f->here) +
							(f->endb - f->next);
					}
					else
					{	memcpy((Void_t*)f->data,
							(Void_t*)frs->data,n);
						f->endb = (f->next = f->data) + n;
					}
				}
				frs = NIL(Sfrsrv_t*);
			}

			f->push = push;
		}
	}

done:
	/* return true buffer size */
	_Sfi = f->val = n;

	SFOPEN(f,0);

	if((sz > 0 && n < sz) || (n == 0 && lock <= 0) )
		return NIL(Void_t*);
	else
	{	reg Void_t* rsrv = frs ? (Void_t*)frs->data : (Void_t*)f->next;

		if(rsrv)
		{	if(lock > 0)
			{	f->mode |= SF_PEEK;
				f->endr = f->endw = f->data;
			}
			else if(rsrv == (Void_t*)f->next)
				f->next += (size >= 0 ? size : n);
		}

		return rsrv;
	}
}
