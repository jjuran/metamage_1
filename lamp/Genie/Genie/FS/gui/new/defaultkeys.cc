/*
	gui/new/defaultkeys.cc
	----------------------
*/

#include "Genie/FS/gui/new/defaultkeys.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// Pedestal
#include "Pedestal/View.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/nodes/fixed_dir.hh"
#include "vfs/primitives/lookup.hh"
#include "vfs/primitives/touch.hh"

// Genie
#include "Genie/FS/Views.hh"
#include "Genie/FS/resolve.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	class DefaultKey_Handler : public Ped::View
	{
		private:
			const FSTree* itsKey;
		
		public:
			DefaultKey_Handler( const FSTree* key ) : itsKey( key )
			{
			}
			
			bool KeyDown( const EventRecord& event );
	};
	
	static void Update( const FSTree* that, const char* name )
	{
		const FSTree* window = GetViewWindowKey( that );
		
		ASSERT( window != NULL );
		
		FSTreePtr link = lookup( window, name );
		
		FSTreePtr target = resolve( link.get() );
		
		touch( target.get() );
	}
	
	bool DefaultKey_Handler::KeyDown( const EventRecord& event )
	{
		if ( event.modifiers & (controlKey | rightControlKey) )
		{
			return false;
		}
		
		const char c = event.message & charCodeMask;
		
		switch ( c )
		{
			case kEnterCharCode:
			case kReturnCharCode:
				Update( itsKey, "accept" );
				
				return true;
			
			case kEscapeCharCode:
				switch ( const char keyCode = (event.message & keyCodeMask) >> 8 )
				{
					// Escape
					case 0x35:
						Update( itsKey, "cancel" );
						
						return true;
					
					// Clear
					case 0x47:
					default:
						break;
				}
				break;
			
			default:
				break;
		}
		
		return false;
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		return new DefaultKey_Handler( delegate );
	}
	
	
	FSTreePtr New_defaultkeys( const FSTree*        parent,
	                           const plus::string&  name,
	                           const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView );
	}
	
}

