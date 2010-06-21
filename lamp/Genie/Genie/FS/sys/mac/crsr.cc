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
#ifndef __LOWMEM__
#include <LowMem.h>
#endif

// plus
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"

// ClassicToolbox
//#include "ClassicToolbox/CursorDevices.h"

// MacFeatures
#include "MacFeatures/CursorDevices.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/serialize_qd.hh"


#if TARGET_CPU_68K

short CrsrNew_CrsrCouple : 0x8ce;

#else

static short CrsrNew_CrsrCouple;

#endif


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
	
	
	struct GetCursorLocation : serialize_Point
	{
		static Point Get( const CursorDevice* device )
		{
			if ( TARGET_CPU_68K  &&  device == NULL )
			{
				return ::LMGetRawMouseLocation();
			}
			
			if ( device->whichCursor == NULL )
			{
				throw FSTree_Property::Undefined();
			}
			
			const CursorData& data = *device->whichCursor;
			
			return data.where;
		}
		
		static void Set( CursorDevicePtr device, const Point& location )
		{
			if ( TARGET_CPU_68K  &&  device == NULL )
			{
				::LMSetRawMouseLocation( location );
				::LMSetMouseTemp       ( location );
				
				CrsrNew_CrsrCouple = -1;
				
				return;
			}
			
			N::CursorDeviceMoveTo( device, location.h, location.v );
		}
	};
	
	template < class Accessor >
	struct sys_mac_crsr_Property : readwrite_property
	{
		static const std::size_t fixed_size = Accessor::fixed_size;
		
		typedef typename Accessor::result_type result_type;
		
		static CursorDevicePtr GetCursorDevice( const FSTree* that )
		{
			static const bool has_CDM = MacFeatures::Has_CursorDevices();
			
			if ( !has_CDM )
			{
				return NULL;
			}
			
			CursorDevicePtr device = N::CursorDeviceNextDevice();
			
			if ( device == NULL )
			{
				throw FSTree_Property::Undefined();
			}
			
			return device;
		}
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			CursorDevicePtr device = GetCursorDevice( that );
			
			const result_type data = Accessor::Get( device );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
		
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			CursorDevicePtr device = GetCursorDevice( that );
			
			const result_type data = Accessor::reconstruct::apply( begin, end, binary );
			
			Accessor::Set( device, data );
		}
	};
	
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< sys_mac_crsr_Property< prop > >::value
	
	const FSTree_Premapped::Mapping sys_mac_crsr_Mappings[] =
	{
		{ "location", PROPERTY( GetCursorLocation ) },
		
		{ NULL, NULL }
	};
	
}

#endif

