/*	=====================
 *	FSTree_dev_gestalt.cc
 *	=====================
 */

#include "Genie/FS/FSTree_dev_gestalt.hh"

// Iota
#include "iota/strings.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"

// Genie
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/IO/Device.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
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
		N::GestaltSelector selector = N::GestaltSelector( request );
		
		long value = N::Gestalt( selector );
		
		if ( argp != NULL )
		{
			*argp = value;
		}
	}
	
	boost::shared_ptr< IOHandle > dev_gestalt::open( OpenFlags flags )
	{
		return boost::shared_ptr< IOHandle >( new GestaltDeviceHandle( flags ) );
	}
	
}

