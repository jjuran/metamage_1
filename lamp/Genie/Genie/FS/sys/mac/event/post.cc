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
	
	struct sys_mac_event_post_key
	{
		static void Read( plus::var_string& result, const FSTree* that, bool binary )
		{
			throw FSTree_Property::Undefined();
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end, bool binary )
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
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read,
		                            &Property::Write );
	}
	
	const FSTree_Premapped::Mapping sys_mac_event_post_Mappings[] =
	{
		{ "click", &Basic_Factory< Trigger< sys_mac_event_post_click > > },
		
		{ "key",   &Property_Factory< sys_mac_event_post_key   > },
		
		{ NULL, NULL }
	};
	
}

