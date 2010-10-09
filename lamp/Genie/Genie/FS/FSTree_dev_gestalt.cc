/*	=====================
 *	FSTree_dev_gestalt.cc
 *	=====================
 */

#include "Genie/FS/FSTree_dev_gestalt.hh"

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
			GestaltDeviceHandle( OpenFlags flags ) : DeviceHandle( flags )
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
	
	boost::shared_ptr< IOHandle > dev_gestalt::open( OpenFlags flags )
	{
		return seize_ptr( new GestaltDeviceHandle( flags ) );
	}
	
}

