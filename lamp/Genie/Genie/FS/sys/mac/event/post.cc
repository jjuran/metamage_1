/*
	Genie/FS/sys/mac/event/post.cc
	------------------------------
*/

#include "Genie/FS/sys/mac/event/post.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

#include "Nitrogen/Events.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Trigger.hh"
#include "Genie/FS/property.hh"


namespace Nitrogen
{
	
	inline void PostEvent( EventKind eventNum, UInt32 eventMsg )
	{
		Mac::ThrowOSStatus( ::PostEvent( eventNum, eventMsg ) );
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	struct sys_mac_event_post_click
	{
		static void trigger( const FSTree* node )
		{
			N::PostEvent( N::mouseDown, 0 );
			N::PostEvent( N::mouseUp,   0 );
		}
	};
	
	static const trigger_extra click_extra =
	{
		&sys_mac_event_post_click::trigger
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
	
	const vfs::fixed_mapping sys_mac_event_post_Mappings[] =
	{
		{ "click", &trigger_factory, (void*) &click_extra },
		
		{ "key",   PROPERTY( sys_mac_event_post_key ) },
		
		{ NULL, NULL }
	};
	
}

