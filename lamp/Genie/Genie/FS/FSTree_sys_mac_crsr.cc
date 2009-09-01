/*	======================
 *	FSTree_sys_mac_crsr.cc
 *	======================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FS/FSTree_sys_mac_crsr.hh"

// Mac OS
#include <CursorDevices.h>

// Nitrogen
#include "Nitrogen/OSStatus.h"

// ClassicToolbox
//#include "ClassicToolbox/CursorDevices.h"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Scribes.hh"


namespace Nitrogen
{
	
	inline CursorDevicePtr CursorDeviceNextDevice( CursorDevicePtr device = NULL )
	{
		ThrowOSStatus( ::CursorDeviceNextDevice( &device ) );
		
		return device;
	}
	
	inline void CursorDeviceMoveTo( CursorDevicePtr device, long x, long y )
	{
		ThrowOSStatus( ::CursorDeviceMoveTo( device, x, y ) );
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
		
		static void Set( CursorDevicePtr device, const char* begin, const char* end, bool binary )
		{
			Point location = Vivify< Point_Scribe<> >( begin, end, binary );
			
			N::CursorDeviceMoveTo( device, location.h, location.v );
		}
	};
	
	template < class Accessor >
	struct sys_mac_crsr_Property
	{
		static CursorDevicePtr GetCursorDevice( const FSTree* that )
		{
			CursorDevicePtr device = N::CursorDeviceNextDevice();
			
			if ( device == NULL )
			{
				throw FSTree_Property::Undefined();
			}
			
			return device;
		}
		
		static std::string Read( const FSTree* that, bool binary )
		{
			CursorDevicePtr device = GetCursorDevice( that );
			
			return Accessor::Get( *device, binary );
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			CursorDevicePtr device = GetCursorDevice( that );
			
			Accessor::Set( device, begin, end, binary );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_mac_crsr_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read,
		                            &Property::Write );
	}
	
	
	const FSTree_Premapped::Mapping sys_mac_crsr_Mappings[] =
	{
		{ "location", &Property_Factory< GetCursorLocation > },
		
		{ NULL, NULL }
	};
	
}

#endif

