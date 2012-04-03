#include	"sfhdr.h"
static char*	Version = "\n@(#)sfio (AT&T Labs - kpv) 1998-05-01\0\n";

#if _PACKAGE_ast
#include <sig.h>
#include <wait.h>
#endif

/*	Switch the given stream to a desired mode
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
static void _sfcleanup(void)
#else
static void _sfcleanup()
#endif
{
	reg Sfpool_t*	p;
	reg Sfio_t*	f;
	reg int		n;
	reg int		pool;

	sfsync(NIL(Sfio_t*));

	/* unbuffer all write streams so that any new output 
	   from other atexit functions will be done immediately.
	   This can be inefficient for applications that do lots of
	   output at the end but it's correct.
	*/
	for(p = &_Sfpool; p; p = p->next)
	{	for(n = 0; n < p->n_sf; ++n)
		{	f = p->sf[n];
			if(SFFROZEN(f) || (f->flags&SF_STRING) )
				continue;

			pool = f->mode&SF_POOL;
			f->mode &= ~SF_POOL;
			if((f->flags&SF_WRITE) && !(f->mode&SF_WRITE))
				(void)_sfmode(f,SF_WRITE,0);
			if(((f->bits&SF_MMAP) && f->data) ||
			   ((f->mode&SF_WRITE) && f->next == f->data) )
				sfsetbuf(f,NIL(Void_t*),0);
			f->mode |= pool;
		}
	}

	/* set this so that no more buffering is allowed */
	_Sfexiting = 1001;
}

/* put into discrete pool */
#if __STD_C
int _sfsetpool(Sfio_t* f)
#else
int _sfsetpool(f)
Sfio_t*	f;
#endif
{
	reg Sfpool_t*	p;
	reg Sfio_t**	array;
	reg int		n, rv;

	if(!_Sfcleanup)
	{	_Sfcleanup = _sfcleanup;
		(void)atexit(_sfcleanup);
	}

	if(!(p = f->pool) )
		p = f->pool = &_Sfpool;

	if(p->mode&SF_LOCK) /* only one manipulator at a time */
		return -1;

	p->mode |= SF_LOCK;
	rv = -1;

	if(p->n_sf >= p->s_sf)
	{	if(p->s_sf == 0) /* initialize pool array */
		{	p->s_sf = sizeof(p->array)/sizeof(p->array[0]);
			p->sf = p->array;
		}
		else	/* allocate a larger array */
		{	n = (p->sf != p->array ? p->s_sf : (p->s_sf/4 + 1)*4) + 4;
			if(!(array = (Sfio_t**)malloc(n*sizeof(Sfio_t*))) )
				goto done;

			/* move old array to new one */
			memcpy((Void_t*)array,(Void_t*)p->sf,p->n_sf*sizeof(Sfio_t*));
			if(p->sf != p->array)
				free((Void_t*)p->sf);

			p->sf = array;
			p->s_sf = n;
		}
	}

	/* always add at end of array because if this was done during some sort
	   of walk thru all streams, we'll want the new stream to be seen.
	*/
	p->sf[p->n_sf++] = f;
	rv = 0;

done:
	p->mode &= ~SF_LOCK;
	return rv;
}

/* to create auxiliary buffer */
#define SF_RSRV	128
static Sfrsrv_t*	_Sfrsrv;

#if __STD_C
Sfrsrv_t* _sfrsrv(reg Sfio_t* f, reg ssize_t size)
#else
Sfrsrv_t* _sfrsrv(f,size)
reg Sfio_t*	f;
reg ssize_t	size;	/* closing if size < 0 */
#endif
{
	reg Sfrsrv_t	*last, *frs;

	for(last = NIL(Sfrsrv_t*), frs = _Sfrsrv; frs; last = frs, frs = frs->next)
		if(frs->sf == f)
			break;
	if(frs)
	{	/* unhook it from current position */
		if(last)
			last->next = frs->next;
		else	_Sfrsrv = frs->next;
	}

	if(size < 0)	/* closing stream */
	{	if(frs)
			free(frs);
		return NIL(Sfrsrv_t*);
	}

	/* inserting, make buffer if nothing yet */
	size = ((size + SF_RSRV-1)/SF_RSRV)*SF_RSRV;
	if(!frs || size > frs->size)
	{	if(!(last = (Sfrsrv_t*)malloc(size+sizeof(Sfrsrv_t))))
			size = -1;
		else
		{	if(frs)
			{	if(frs->slen > 0)
					memcpy(last,frs,sizeof(Sfrsrv_t)+frs->slen);
				free(frs);
			}
			frs = last;
			frs->size = size;
			frs->slen = 0;
		}
	}

	if(frs)
	{	if(size > 0)
			frs->slen = 0;
		frs->sf = f;
		frs->next = _Sfrsrv;
		_Sfrsrv = frs;
	}

	return size >= 0 ? frs : NIL(Sfrsrv_t*);
}

/* to keep track of process or unseekable read/write streams */
typedef struct _sfp_
{
	struct _sfp_*	next;	/* link list			*/
	int		pid;	/* process id			*/
	Sfio_t*		sf;	/* stream associated with	*/
	uchar*		rdata;	/* read data being cached	*/
	int		ndata;	/* size of cached data		*/
	int		size;	/* buffer size			*/
	int		file;	/* saved file descriptor	*/
} Sfpopen_t;
static Sfpopen_t*	_Sfprocess;

#if __STD_C
int _sfpopen(reg Sfio_t* f, int fd, int pid)
#else
int _sfpopen(f, fd, pid)
reg Sfio_t*	f;
int		fd;
int		pid;
#endif
{
	reg Sfpopen_t* p = (Sfpopen_t*)Version;	/* shut up unuse warning */

	for(p = _Sfprocess; p; p = p->next)
		if(p->sf == f)
			return 0;

	if(!(p = (Sfpopen_t*)malloc(sizeof(Sfpopen_t))) )
		return -1;

	f->bits |= SF_PROCESS;

	p->pid = pid;
	p->sf = f;
	p->size = p->ndata = 0;
	p->rdata = NIL(uchar*);
	p->file = fd;
	p->next = _Sfprocess;
	_Sfprocess = p;

	return 0;
}

#if __STD_C
int _sfpclose(reg Sfio_t* f)
#else
int _sfpclose(f)
reg Sfio_t*	f;	/* stream to close */
#endif
{
	reg Sfpopen_t	*last, *p;
	int		pid, status;

	if(!f)
	{	/* before exec-ing, close all pipe ends */
		for(p = _Sfprocess; p; p = p->next)
		{	if(p->file >= 0)
				CLOSE(p->file);
			CLOSE(p->sf->file);
		}
		return 0;
	}

	/* find the associated process structure */
	for(last = NIL(Sfpopen_t*), p = _Sfprocess; p ; last = p, p = p->next)
		if(f == p->sf)
			break;
	if(!p)
		return -1;

	f->bits &= ~SF_PROCESS;
	if(sfclose(f) < 0)
	{	f->bits |= SF_PROCESS;
		return -1;
	}

	/* delete from process table */
	if(last)
		last->next = p->next;
	else	_Sfprocess = p->next;
	if(p->rdata)
		free((char*)p->rdata);

	if(p->pid < 0)
	{	free((char*)p);
		return 0;
	}

	/* close the associated streams */
	if(p->file >= 0)
		CLOSE(p->file);

	/* wait for process termination */
#if _PACKAGE_ast
	sigcritical(1);
#endif
	while ((pid = waitpid(p->pid,&status,0)) == -1 && errno == EINTR)
		;
#if _PACKAGE_ast
	sigcritical(0);
#endif

	free(p);
	return (pid == -1 ? -1 : status);
}

#if __STD_C
static int _sfpmode(Sfio_t* f, int type)
#else
static int _sfpmode(f,type)
Sfio_t*	f;
int	type;
#endif
{	reg Sfpopen_t	*last, *p;

	/* find the associated process structure. We must be careful here
	   with respect to a stacked stream since it takes on the identity
	   of the stack base.
	*/
	for(last = NIL(Sfpopen_t*), p = _Sfprocess; p; last = p, p = p->next)
		if((f->push ? f->push : f) == p->sf)
			break;
	if(!p)
		return -1;

	/* move-to-front heuristic */
	if(last)
	{	last->next = p->next;
		p->next = _Sfprocess;
		_Sfprocess = p;
	}

	if(type == SF_WRITE)
	{	/* save unread data */
		p->ndata = f->endb-f->next;
		if(p->ndata > p->size)
		{	if(p->rdata)
				free((char*)p->rdata);
			if((p->rdata = (uchar*)malloc(p->ndata)) )
				p->size = p->ndata;
			else
			{	p->size = 0;
				return -1;
			}
		}
		if(p->ndata > 0)
			memcpy((Void_t*)p->rdata,(Void_t*)f->next,p->ndata);
		f->endb = f->data;
	}
	else
	{	/* restore read data */
		if(p->ndata > f->size)	/* may lose data!!! */
			p->ndata = f->size;
		if(p->ndata > 0)
		{	memcpy((Void_t*)f->data,(Void_t*)p->rdata,p->ndata);
			f->endb = f->data+p->ndata;
			p->ndata = 0;
		}
	}

	/* switch file descriptor */
	if(p->pid >= 0)
	{	type = f->file;
		f->file = p->file;
		p->file = type;
	}

	return 0;
}

#if __STD_C
void _sfswap(Sfio_t* f1, Sfio_t* f2, int stack)
#else
void _sfswap(f1, f2, stack)
Sfio_t*	f1;
Sfio_t*	f2;
int	stack;
#endif
{
	reg Sfrsrv_t	*r, *r1, *r2;
	reg Sfpopen_t	*p, *p1, *p2;

	/* switch sfreserve or sfgetr identifier */
	r1 = r2 = NIL(Sfrsrv_t*);
	for(r = _Sfrsrv; r; r = r->next)
	{	if(r->sf == f1)
			r1 = r;
		if(r->sf == f2)
			r2 = r;
	}
	if(r1)
		r1->sf = f2;
	if(r2)
		r2->sf = f1;

	/* if called from sfstack, we already did this */
	if(!stack)
	{	/* and for sfpopen buffer */
		p1 = p2 = NIL(Sfpopen_t*);
		for(p = _Sfprocess; p; p = p->next)
		{	if(p->sf == f1)
				p1 = p;
			if(p->sf == f2)
				p2 = p;
		}
		if(p1)
			p1->sf = f2;
		if(p2)
			p2->sf = f1;
	}
}

#if __STD_C
int _sfmode(reg Sfio_t* f, reg int wanted, reg int local)
#else
int _sfmode(f, wanted, local)
reg Sfio_t*	f;	/* change r/w mode and sync file pointer for this stream */
reg int		wanted;	/* desired mode */
reg int		local;	/* a local call */
#endif
{
	reg int		n;
	reg Sfoff_t	addr;
	reg int		rv = 0;

	if((!local && SFFROZEN(f)) || (!(f->flags&SF_STRING) && f->file < 0))
	{	if(local || !f->disc || !f->disc->exceptf)
		{	local = 1;
			goto err_notify;
		}

		for(;;)
		{	if((rv = (*f->disc->exceptf)(f,SF_LOCKED,0,f->disc)) < 0)
				return rv;
			if((!local && SFFROZEN(f)) ||
			   (!(f->flags&SF_STRING) && f->file < 0) )
			{	if(rv == 0)
				{	local = 1;
					goto err_notify;
				}
				else	continue;
			}
			else	break;
		}
	}

	if(f->mode&SF_GETR)
	{	f->mode &= ~SF_GETR;
#ifdef MAP_TYPE
		if((f->bits&SF_MMAP) && (f->tiny[0] += 1) >= (4*SF_NMAP) )
		{	/* turn off mmap to avoid page faulting */
			sfsetbuf(f,(Void_t*)f->tiny,(size_t)SF_UNBOUND);
			f->tiny[0] = 0;
		}
		else
#endif
		if(f->getr)
		{	f->next[-1] = f->getr;
			f->getr = 0;
		}
	}

	if(f->mode&SF_STDIO) /* synchronizing with stdio pointers */
		(*_Sfstdio)(f);

	if(f->disc == _Sfudisc && wanted == SF_WRITE &&
	   sfclose((*_Sfstack)(f,NIL(Sfio_t*))) < 0 )
	{	local = 1;
		goto err_notify;
	}

	if(f->mode&SF_POOL)
	{	/* move to head of pool */
		if(f == f->pool->sf[0] || (*_Sfpmove)(f,0) < 0 )
		{	local = 1;
			goto err_notify;
		}
		f->mode &= ~SF_POOL;
	}

	SFLOCK(f,local);

	/* buffer initialization */
	wanted &= SF_RDWR;
	if(f->mode&SF_INIT)
	{	
		if(!f->pool && _sfsetpool(f) < 0)
		{	rv = -1;
			goto done;
		}

		if(wanted == 0)
			goto done;

		if(wanted != (int)(f->mode&SF_RDWR) && !(f->flags&wanted) )
			goto err_notify;

		if((f->flags&SF_STRING) && f->size >= 0 && f->data)
		{	f->mode &= ~SF_INIT;
			f->extent = ((f->flags&SF_READ) || (f->bits&SF_BOTH)) ?
					f->size : 0;
			f->here = 0;
			f->endb = f->data + f->size;
			f->next = f->endr = f->endw = f->data;
			if(f->mode&SF_READ)
				f->endr = f->endb;
			else	f->endw = f->endb;
		}
		else
		{	n = f->flags;
			(void)SFSETBUF(f,f->data,f->size);
			f->flags |= (n&SF_MALLOC);
		}
	}

	if(wanted == (int)SFMODE(f,1))
		goto done;

	switch(SFMODE(f,1))
	{
	case SF_WRITE: /* switching to SF_READ */
		if(wanted == 0 || wanted == SF_WRITE)
			break;
		if(!(f->flags&SF_READ) )
			goto err_notify;
		else if(f->flags&SF_STRING)
		{	SFSTRSIZE(f);
			f->endb = f->data+f->extent;
			f->mode = SF_READ;
			break;
		}

		/* reset buffer */
		if(f->next > f->data && SFFLSBUF(f,-1) < 0)
			goto err_notify;

		if(f->size == 0)
		{	/* unbuffered */
			f->data = f->tiny;
			f->size = sizeof(f->tiny);
		}
		f->next = f->endr = f->endw = f->endb = f->data;
		f->mode = SF_READ|SF_LOCK;

		/* restore saved read data for coprocess */
		if((f->bits&SF_PROCESS) && _sfpmode(f,wanted) < 0)
			goto err_notify;

		break;

	case (SF_READ|SF_SYNCED): /* a previously sync-ed read stream */
		if(wanted != SF_WRITE)
		{	/* just reset the pointers */
			f->mode = SF_READ|SF_LOCK;

			/* see if must go with new physical location */
			if((f->flags&(SF_SHARE|SF_PUBLIC)) == (SF_SHARE|SF_PUBLIC) &&
			   (addr = SFSK(f,0,1,f->disc)) != f->here)
			{
#ifdef MAP_TYPE
				if((f->bits&SF_MMAP) && f->data)
				{	SFMUNMAP(f,f->data,f->endb-f->data);
					f->data = NIL(uchar*);
				}
#endif
				f->endb = f->endr = f->endw = f->next = f->data;
				f->here = addr;
			}
			else
			{	addr = f->here + (f->endb - f->next);
				if(SFSK(f,addr,0,f->disc) < 0)
					goto err_notify;
				f->here = addr;
			}

			break;
		}
		/* fall thru */

	case SF_READ: /* switching to SF_WRITE */
		if(wanted != SF_WRITE)
			break;
		else if(!(f->flags&SF_WRITE))
			goto err_notify;
		else if(f->flags&SF_STRING)
		{	f->endb = f->data+f->size;
			f->mode = SF_WRITE|SF_LOCK;
			break;
		}

		/* save unread data before switching mode */
		if((f->bits&SF_PROCESS) && _sfpmode(f,wanted) < 0)
			goto err_notify;

		/* reset buffer and seek pointer */
		if(!(f->mode&SF_SYNCED) )
		{	n = f->endb - f->next;
			if(f->extent >= 0 && (n > 0 || (f->data && (f->bits&SF_MMAP))) )
			{	/* reset file pointer */
				addr = f->here - n;
				if(SFSK(f,addr,0,f->disc) < 0)
					goto err_notify;
				f->here = addr;
			}
		}

		f->mode = SF_WRITE|SF_LOCK;
#ifdef MAP_TYPE
		if(f->bits&SF_MMAP)
		{	if(f->data)
			{	(void)munmap((caddr_t)f->data,f->endb-f->data);
				f->endb = f->endr = f->endw =
				f->next = f->data = NIL(uchar*);
			}
			(void)SFSETBUF(f,(Void_t*)f->tiny,(size_t)SF_UNBOUND);
		}
#endif
		if(f->data == f->tiny)
		{	f->endb = f->data = f->next = NIL(uchar*);
			f->size = 0;
		}
		else	f->endb = (f->next = f->data) + f->size;

		break;

	default: /* unknown case */
	err_notify:
		if(_Sfnotify)
		{	if((wanted &= SF_RDWR) == 0)
				wanted = f->flags&SF_RDWR;
			(*_Sfnotify)(f,wanted,f->file);
		}
		rv = -1;
		break;
	}

done:
	SFOPEN(f,local);
	return rv;
}
