/*
	gestalt.cc
	----------
*/

#include "MacVFS/file/gestalt.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/functions/resolve_pathname.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	class gestalt_device : public filehandle
	{
		public:
			gestalt_device( int flags ) : filehandle( flags )
			{
			}
			
			node_ptr GetFile();
			
			void IOCtl( unsigned long request, int* argp );
	};
	
	
	node_ptr gestalt_device::GetFile()
	{
		return resolve_absolute_path( STR_LEN( "/dev/gestalt" ) );
	}
	
	void gestalt_device::IOCtl( unsigned long request, int* argp )
	{
		long value;
		
		const OSErr err = ::Gestalt( request, &value );
		
		switch ( err )
		{
			case noErr:
				break;
			
			default:  // shouldn't happen
			case gestaltUndefSelectorErr:  p7::throw_errno( EINVAL );
			case gestaltUnknownErr      :  p7::throw_errno( ENXIO  );
		}
		
		if ( argp != NULL )
		{
			*argp = value;
		}
	}
	
	filehandle_ptr open_gestalt( const node* that, int flags, mode_t mode )
	{
		return new gestalt_device( flags );
	}
	
}

