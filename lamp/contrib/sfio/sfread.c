#include	"sfhdr.h"

/*	Read n bytes from a stream into a buffer
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
ssize_t sfread(reg Sfio_t* f, Void_t* buf, reg size_t n)
#else
ssize_t sfread(f,buf,n)
reg Sfio_t*	f;	/* read from this stream. 	*/
Void_t*		buf;	/* buffer to read into		*/
reg size_t	n;	/* number of bytes to be read. 	*/
#endif
{
	reg char*	s;
	reg uchar*	next;
	reg ssize_t	r;
	reg char*	begs;
	reg int		local;

	GETLOCAL(f,local);

	/* release peek lock */
	if(f->mode&SF_PEEK)
	{	if(!(f->mode&SF_READ) )
			return -1;

		if(f->mode&SF_GETR)
		{	if(((uchar*)buf + f->val) != f->next)
			{	Sfrsrv_t* frs;
				if(!(frs = _sfrsrv(f,0)) || frs->data != (uchar*)buf)
					return -1;
			}
			f->mode &= ~SF_PEEK;
			return 0;
		}
		else
		{	if((uchar*)buf != f->next)
				return -1;
			f->mode &= ~SF_PEEK;
			if(f->mode&SF_PKRD)
			{	/* actually read the data now */
				f->mode &= ~SF_PKRD;
				if(n > 0)
					n = (r = read(f->file,f->data,n)) < 0 ? 0 : r;
				f->endb = f->data+n;
				f->here += n;
			}
			f->next += n;
			f->endr = f->endb;
			return n;
		}
	}

	s = begs = (char*)buf;
	for(;; f->mode &= ~SF_LOCK)
	{	/* check stream mode */
		if(SFMODE(f,local) != SF_READ && _sfmode(f,SF_READ,local) < 0)
			return s > begs ? s-begs : -1;

		if(n <= 0)
			break;

		SFLOCK(f,local);

		/* determine current amount of buffered data */
		if((size_t)(r = f->endb - f->next) > n)
			r = n;

		if(r > 0)
		{	/* copy data already in buffer */
			next = f->next;
			MEMCPY(s,next,r);
			f->next = next;
			if((n -= r) <= 0)
				break;
		}
		else if(((f->flags&SF_SHARE) && f->extent < 0) ||
			(n >= SF_PAGE && (!(f->flags&SF_STRING) || !(f->bits&SF_MMAP))) )
		{	/* do a direct read to user's buf */
			f->next = f->endb = f->data;
			if((size_t)f->size < SF_PAGE ||
			   (f->extent >= 0 && (f->here+n) >= f->extent) ||
			   (f->extent <  0 && (f->flags&SF_SHARE) ) )
				r = n;
			else	r = n <= SF_PAGE ? n : (n/SF_PAGE)*SF_PAGE;
			if((r = SFRD(f,s,r,f->disc)) == 0)
				break;
			else if(r > 0)
			{	s += r;
				if((n -= r) <= 0)
					break;
			}
		}
		else if((f->flags&SF_STRING) || (f->bits&SF_MMAP) ||
			f->extent < 0 || (f->here%SF_PAGE) == 0)
		{	if(SFFILBUF(f,-1) <= 0)
				break;
		}
		else
		{	f->next = f->endb = f->data;
			if((r = (n/SF_PAGE + 1)*SF_PAGE) > f->size)
				r = f->size;
			if((r = SFRD(f, f->next, r, f->disc)) <= 0)
				break;
		}
	}

	SFOPEN(f,local);
	return s-begs;
}
