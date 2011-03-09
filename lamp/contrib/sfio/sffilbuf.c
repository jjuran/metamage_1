#include	"sfhdr.h"

/*	Fill the buffer of a stream with data.
**	If n < 0, sffilbuf() attempts to fill the buffer if it's empty.
**	If n == 0, if the buffer is not empty, just return the first byte;
**		otherwise fill the buffer and return the first byte.
**	If n > 0, even if the buffer is not empty, try a read to get as
**		close to n as possible. n is reset to -1 if stack pops.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
int _sffilbuf(reg Sfio_t* f, reg int n)
#else
int _sffilbuf(f,n)
reg Sfio_t*	f;	/* fill the read buffer of this stream */
reg int		n;	/* see above */
#endif
{
	reg ssize_t	r;
	reg int		first, local, rcrv, rc;

	GETLOCAL(f,local);

	/* any peek data must be preserved across stacked streams */
	rcrv = f->mode&(SF_RC|SF_RV|SF_LOCK);
	rc = f->getr;

	for(first = 1;; first = 0, (f->mode &= ~SF_LOCK) )
	{	/* check mode */
		if(SFMODE(f,local) != SF_READ && _sfmode(f,SF_READ,local) < 0)
			return -1;
		SFLOCK(f,local);

		/* current extent of available data */
		if((r = f->endb-f->next) > 0)
		{	/* on first iteration, n is amount beyond current buffer;
			   afterward, n is the exact amount requested */
			if((first && n <= 0) || (!first && n <= r) ||
			   (f->flags&SF_STRING))
				break;

			/* try shifting left to make room for new data */
			if(!(f->bits&SF_MMAP) && f->next > f->data &&
			   n > (f->size - (f->endb-f->data)) )
			{	uchar*	copy;

				if(f->extent < 0 || f->size < _Sfpage)
					copy = f->next;
				else	/* try keeping alignment */
				{	Sfoff_t	a = ((f->here-r)/_Sfpage)*_Sfpage;
					if(a < (f->here-r) &&
					   a > (f->here - (f->endb-f->data)) )
						copy = f->endb - (f->here-a);
					else	break;
				}

				memcpy((char*)f->data, (char*)copy, f->endb-copy);
				f->next = f->data + (f->next - copy);
				f->endb = f->data + (f->endb - copy);
			}
		}
		else if(!(f->flags&SF_STRING) && !(f->bits&SF_MMAP) )
			f->next = f->endb = f->endr = f->data;

		if(f->bits&SF_MMAP)
			r = n > 0 ? n : f->size;
		else if(!(f->flags&SF_STRING) )
		{	/* make sure we read no more than required */
			r = f->size - (f->endb - f->data);
			if(n > 0 && r > n && f->extent < 0 && (f->flags&SF_SHARE) )
				r = n;
		}

		/* SFRD takes care of discipline read and stack popping */
		f->mode |= rcrv;
		f->getr = rc;
		if((r = SFRD(f,f->endb,r,f->disc)) >= 0)
		{	r = f->endb - f->next;
			break;
		}
	}

	SFOPEN(f,local);
	return (n == 0) ? (r > 0 ? (int)(*f->next++) : EOF) : (int)r;
}
