/*
	Genie/FS/sys/mac/crsr.cc
	------------------------
*/

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON

#include "Genie/FS/sys/mac/crsr.hh"

// Mac OS
#ifndef __CURSORDEVICES__
#include <CursorDevices.h>
#endif

// plus
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"

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
		typedef Point Result;
		typedef Point Value;
		typedef Point Param;
		
		typedef Point_Scribe< ',' > Scribe;
		
		static Result Get( const CursorDevice& device )
		{
			if ( device.whichCursor == NULL )
			{
				throw FSTree_Property::Undefined();
			}
			
			const CursorData& data = *device.whichCursor;
			
			return data.where;
		}
		
		static void Set( CursorDevicePtr device, Param location )
		{
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
		
		static void Read( plus::var_string& result, const FSTree* that, bool binary )
		{
			CursorDevicePtr device = GetCursorDevice( that );
			
			const typename Accessor::Result data = Accessor::Get( *device );
			
			result = Freeze< Accessor::Scribe >( data, binary );
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			CursorDevicePtr device = GetCursorDevice( that );
			
			const typename Accessor::Value data = Vivify< Accessor::Scribe >( begin, end, binary );
			
			Accessor::Set( device, data );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
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

