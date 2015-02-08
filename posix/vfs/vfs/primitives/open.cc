/*
	open.cc
	-------
*/

#include "vfs/primitives/open.hh"

// POSIX
#include <fcntl.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/functions/truncate.hh"
#include "vfs/functions/access.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"


#ifndef O_EXEC
#define O_EXEC  0
#endif
#ifndef O_SEARCH
#define O_SEARCH  0
#endif


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	static inline bool writing( int flags )
	{
		flags &= O_ACCMODE;
		
		return flags == O_WRONLY  ||  flags == O_RDWR;
	}
	
	static inline int amode_from_oflags( int flags )
	{
		int amode = 0;
		
		const int accmode = flags & O_ACCMODE;
		
		switch ( accmode )
		{
			case O_RDONLY:  amode = R_OK;       break;
			case O_WRONLY:  amode = W_OK;       break;
			case O_RDWR:    amode = R_OK|W_OK;  break;
			
			default:
				break;
		}
		
		if ( flags & (O_EXEC|O_SEARCH) )
		{
			amode |= X_OK;
		}
		
		return amode;
	}
	
	filehandle_ptr open( const node& that, int flags, mode_t mode )
	{
		if ( flags & O_TRUNC  &&  !writing( flags ) )
		{
			p7::throw_errno( EINVAL );
		}
		
		const bool creating = flags & O_CREAT;
		
		if ( creating  &&  that.filemode() == 0 )
		{
			// File doesn't exist -- skip access check
		}
		else
		{
			access( that, amode_from_oflags( flags ) );
		}
		
		const node_method_set* methods = that.methods();
		
		const data_method_set* data_methods;
		
		if ( methods  &&  (data_methods = methods->data_methods) )
		{
			if ( data_methods->open )
			{
				filehandle_ptr result = data_methods->open( &that, flags, mode );
				
				if ( flags & O_TRUNC )
				{
					truncate( *result );
				}
				
				return result;
			}
		}
		
		if ( (flags & (O_CREAT|O_EXCL)) == (O_CREAT|O_EXCL) )
		{
			p7::throw_errno( EPERM );
		}
		
		throw p7::errno_t( creating ? EPERM : ENOENT );
	}
	
}
