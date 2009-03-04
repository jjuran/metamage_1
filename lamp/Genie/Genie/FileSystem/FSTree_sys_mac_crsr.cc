/*	======================
 *	FSTree_sys_mac_crsr.cc
 *	======================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FileSystem/FSTree_sys_mac_crsr.hh"

// Mac OS
#include <CursorDevices.h>

// ClassicToolbox
//#include "ClassicToolbox/CursorDevices.h"

// Genie
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/Scribes.hh"


namespace Nitrogen
{
	
	static CursorDevicePtr CursorDeviceNextDevice( CursorDevicePtr device = NULL )
	{
		ThrowOSStatus( ::CursorDeviceNextDevice( &device ) );
		
		return device;
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	struct GetCursorLocation
	{
		typedef std::string Result;
		
		static Result Get( const CursorDevice& device, bool binary )
		{
			if ( device.whichCursor == NULL )
			{
				throw FSTree_Property::Undefined();
			}
			
			const CursorData& data = *device.whichCursor;
			
			return Freeze< Point_Scribe< ',' > >( data.where, binary );
		}
	};
	
	template < class Accessor >
	struct sys_mac_desktop_Property
	{
		static std::string Read( const FSTree* that, bool binary )
		{
			CursorDevicePtr device = N::CursorDeviceNextDevice();
			
			if ( device == NULL )
			{
				throw FSTree_Property::Undefined();
			}
			
			return Accessor::Get( *device, binary );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_mac_desktop_Property< Accessor > Property;
		
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Read ) );
	}
	
	
	const FSTree_Premapped::Mapping sys_mac_crsr_Mappings[] =
	{
		{ "location", &Property_Factory< GetCursorLocation > },
		
		{ NULL, NULL }
	};
	
}

#endif

