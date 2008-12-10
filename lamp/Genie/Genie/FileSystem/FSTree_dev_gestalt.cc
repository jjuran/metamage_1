/*	=====================
 *	FSTree_dev_gestalt.cc
 *	=====================
 */

#include "Genie/FileSystem/FSTree_dev_gestalt.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/Device.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	class GestaltDeviceHandle : public DeviceHandle
	{
		public:
			FSTreePtr GetFile() const  { return ResolvePathname( "/dev/gestalt" ); }
			
			unsigned int SysPoll() const  { return 0; }
			
			ssize_t SysRead( char* data, std::size_t byteCount )  { return 0; }
			
			ssize_t SysWrite( const char* data, std::size_t byteCount )  { return byteCount; }
			
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
	
	boost::shared_ptr< IOHandle > FSTree_dev_gestalt::Open( OpenFlags flags ) const
	{
		return boost::shared_ptr< IOHandle >( new GestaltDeviceHandle() );
	}
	
}

