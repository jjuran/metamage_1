/*	=========================
 *	FSTree_sys_mac_gestalt.cc
 *	=========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_gestalt.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"

// Genie
#include "Genie/FileSystem/FSTree_sys_mac.hh"
#include "Genie/IO/Device.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	FSTreePtr FSTree_sys_mac_gestalt::Parent() const
	{
		return GetSingleton< FSTree_sys_mac >();
	}
	
	
	class GestaltDeviceHandle : public DeviceHandle
	{
		public:
			FSTreePtr GetFile() const  { return GetSingleton< FSTree_sys_mac_gestalt >(); }
			
			unsigned int SysPoll() const  { return 0; }
			
			int SysRead( char* data, std::size_t byteCount )  { return 0; }
			
			int SysWrite( const char* data, std::size_t byteCount )  { return byteCount; }
			
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
	
	boost::shared_ptr< IOHandle > FSTree_sys_mac_gestalt::Open( OpenFlags flags ) const
	{
		return boost::shared_ptr< IOHandle >( new GestaltDeviceHandle() );
	}
	
}

