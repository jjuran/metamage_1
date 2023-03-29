/*
	gui/new/defaultkeys.cc
	----------------------
*/

#include "Genie/FS/gui/new/defaultkeys.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// Pedestal
#include "Pedestal/View.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/primitives/lookup.hh"
#include "vfs/primitives/resolve.hh"
#include "vfs/primitives/touch.hh"

// relix-kernel
#include "relix/api/root.hh"

// Genie
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	class DefaultKey_Handler : public Ped::View
	{
		private:
			const vfs::node* itsKey;
		
		public:
			DefaultKey_Handler( const vfs::node* key ) : itsKey( key )
			{
			}
			
			bool KeyDown( const EventRecord& event );
	};
	
	static void Update( const vfs::node* that, const char* name )
	{
		const vfs::node* window = GetViewWindowKey( that );
		
		ASSERT( window != NULL );
		
		vfs::node_ptr link = lookup( *window, name );
		
		vfs::node_ptr target = resolve( *relix::root(), *link );
		
		touch( *target );
	}
	
	bool DefaultKey_Handler::KeyDown( const EventRecord& event )
	{
		if ( event.modifiers & (controlKey | rightControlKey) )
		{
			return false;
		}
		
		const char c = event.message;
		
		switch ( c )
		{
			case kEnterCharCode:
			case kReturnCharCode:
				Update( itsKey, "accept" );
				
				return true;
			
			case kEscapeCharCode:
				switch ( const char keyCode = event.message >> 8 )
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
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		return new DefaultKey_Handler( delegate );
	}
	
	
	vfs::node_ptr New_defaultkeys( const vfs::node*     parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView );
	}
	
}
