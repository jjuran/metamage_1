/*
	Genie/FS/sys/mac/event/post.cc
	------------------------------
*/

#include "Genie/FS/sys/mac/event/post.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

#include "Nitrogen/Events.hh"

// vfs
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"
#include "vfs/node/types/trigger.hh"


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
		static void trigger( const vfs::node* that )
		{
			N::PostEvent( N::mouseDown, 0 );
			N::PostEvent( N::mouseUp,   0 );
		}
	};
	
	static const vfs::trigger_extra click_extra =
	{
		&sys_mac_event_post_click::trigger
	};
	
	static
	void sys_mac_event_post_key_set( const vfs::node* that, const char* begin, const char* end, bool binary )
	{
		for ( const char* p = begin;  p != end;  ++p )
		{
			const char c = *p;
			
			N::PostEvent( N::keyDown, c );
			
			// keyUp is usually masked
			::PostEvent( ::keyUp, c );
		}
	}
	
	static const vfs::property_params sys_mac_event_post_key_params =
	{
		vfs::no_fixed_size,
		NULL,
		&sys_mac_event_post_key_set,
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_event_post_Mappings[] =
	{
		{ "click", &vfs::trigger_factory, (void*) &click_extra },
		
		{ "key",   PROPERTY( sys_mac_event_post_key ) },
		
		{ NULL, NULL }
	};
	
}
