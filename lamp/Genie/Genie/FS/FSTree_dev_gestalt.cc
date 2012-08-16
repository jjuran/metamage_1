/*	=====================
 *	FSTree_dev_gestalt.cc
 *	=====================
 */

#include "Genie/FS/FSTree_dev_gestalt.hh"

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

// Genie
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/IO/Device.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class GestaltDeviceHandle : public DeviceHandle
	{
		public:
			GestaltDeviceHandle( int flags ) : DeviceHandle( flags )
			{
			}
			
			FSTreePtr GetFile()  { return ResolveAbsolutePath( STR_LEN( "/dev/gestalt" ) ); }
			
			unsigned int SysPoll()  { return 0; }
			
			void IOCtl( unsigned long request, int* argp );
	};
	
	
	void GestaltDeviceHandle::IOCtl( unsigned long request, int* argp )
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
	
	IOPtr dev_gestalt::open( const FSTree* node, int flags, mode_t mode )
	{
		return new GestaltDeviceHandle( flags );
	}
	
}

