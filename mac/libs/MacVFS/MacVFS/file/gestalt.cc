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

// mac-sys-utils
#include "mac_sys/trap_available.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	static void gestalt_ioctl( vfs::filehandle* that, unsigned long request, int* argp )
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
	
	static const vfs::general_method_set gestalt_general_methods =
	{
		NULL,
		&gestalt_ioctl,
	};
	
	static const vfs::filehandle_method_set gestalt_methods =
	{
		NULL,
		NULL,
		NULL,
		&gestalt_general_methods,
	};
	
	
	filehandle_ptr open_gestalt( const node* that, int flags, mode_t mode )
	{
		enum { _Gestalt = 0xA1AD };
		
		if ( TARGET_CPU_68K  &&  ! mac::sys::trap_available( _Gestalt ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return new filehandle( that, flags, &gestalt_methods );
	}
	
}
