/*
	Genie/FS/sys/mac/event/post.cc
	------------------------------
*/

#include "Genie/FS/sys/mac/event/post.hh"

// Nitrogen
#include "Nitrogen/Events.hh"
#include "Nitrogen/OSStatus.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Trigger.hh"


namespace Nitrogen
{
	
	inline void PostEvent( EventKind eventNum, UInt32 eventMsg )
	{
		ThrowOSStatus( ::PostEvent( eventNum, eventMsg ) );
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	struct sys_mac_event_post_click
	{
		void operator()() const
		{
			N::PostEvent( N::mouseDown, 0 );
			N::PostEvent( N::mouseUp,   0 );
		}
	};
	
	struct sys_mac_event_post_key : writeonly_property
	{
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			for ( const char* p = begin;  p != end;  ++p )
			{
				const char c = *p;
				
				N::PostEvent( N::keyDown, c );
				
				// keyUp is usually masked
				::PostEvent( ::keyUp, c );
			}
		}
	};
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	const FSTree_Premapped::Mapping sys_mac_event_post_Mappings[] =
	{
		{ "click", &Basic_Factory< Trigger< sys_mac_event_post_click > > },
		
		{ "key",   PROPERTY( sys_mac_event_post_key ) },
		
		{ NULL, NULL }
	};
	
}

