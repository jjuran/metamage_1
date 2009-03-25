#include	"sfhdr.h"

/*	Close a stream. A file stream is synced before closing.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
int sfclose(reg Sfio_t* f)
#else
int sfclose(f)
reg Sfio_t*	f;
#endif
{
	reg int		local, ex;

	if(!f)
		return -1;

	GETLOCAL(f,local);

	if(!(f->mode&SF_INIT) &&
	   SFMODE(f,local) != (f->mode&SF_RDWR) &&
	   SFMODE(f,local) != (f->mode&(SF_READ|SF_SYNCED)) &&
	   _sfmode(f,0,local) < 0)
		return -1;

	/* closing a stack of streams */
	while(f->push)
	{	reg Sfio_t*	pop;

		if(!(pop = (*_Sfstack)(f,NIL(Sfio_t*))) )
			return -1;
		if(sfclose(pop) < 0)
		{	(*_Sfstack)(f,pop);
			return -1;
		}
	}

	/* this is from popen */
	if(f->bits&SF_PROCESS)
	{	if(local)
			SETLOCAL(f);
		return _sfpclose(f);
	}

	if(f->disc == _Sfudisc)	/* closing the ungetc stream */
		f->disc = NIL(Sfdisc_t*);
	/* sync file pointer */
	else if(f->flags&(SF_SHARE|SF_WRITE) )
		(void)sfsync(f);

	SFLOCK(f,0);

	/* zap any associated auxiliary buffer */
	(void)_sfrsrv(f,-1);

	/* raise discipline exceptions */
	if(f->disc && (ex = SFRAISE(f,local ? SF_NEW : SF_CLOSE,NIL(Void_t*))) != 0)
		return ex;

	if(!local && f->pool)
	{	/* remove from pool */
		if(f->pool == &_Sfpool)
		{	reg int	n;
			for(n = 0; n < _Sfpool.n_sf; ++n)
			{	if(_Sfpool.sf[n] != f)
					continue;
				/* found it */
				_Sfpool.n_sf -= 1;
				for(; n < _Sfpool.n_sf; ++n)
					_Sfpool.sf[n] = _Sfpool.sf[n+1];
				break;
			}
		}
		else
		{	f->mode &= ~SF_LOCK;	/**/ASSERT(_Sfpmove);
			if((*_Sfpmove)(f,-1) < 0)
			{	SFOPEN(f,0);
				return -1;
			}
			f->mode |= SF_LOCK;
		}
	}

	if(f->data && (!local || (f->flags&SF_STRING) || (f->bits&SF_MMAP) ) )
	{	/* free buffer */
#ifdef MAP_TYPE
		if(f->bits&SF_MMAP)
			SFMUNMAP(f,f->data,f->endb-f->data);
		else
#endif
		if(f->flags&SF_MALLOC)
			free(f->data);

		f->data = NIL(uchar*);
		f->size = -1;
	}

	/* zap the file descriptor */
	if(_Sfnotify)
		(*_Sfnotify)(f,SF_CLOSE,f->file);
	if(f->file >= 0 && !(f->flags&SF_STRING))
		CLOSE(f->file);
	f->file = -1;

	SFKILL(f);
	f->flags &= SF_STATIC;
	f->here = 0;
	f->extent = -1;
	f->endb = f->endr = f->endw = f->next = f->data;

	if(!local)
	{	if(f->disc && (ex = SFRAISE(f,SF_FINAL,NIL(Void_t*))) != 0 )
			return ex;

		f->disc = NIL(Sfdisc_t*);
		if(!(f->flags&SF_STATIC) )
			SFFREE(f);
	}

	return 0;
}
