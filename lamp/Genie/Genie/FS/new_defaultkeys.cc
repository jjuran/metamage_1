/*	==================
 *	new_defaultkeys.cc
 *	==================
 */

#include "Genie/FS/new_defaultkeys.hh"

// Mac OS
#include <Events.h>

// Pedestal
#include "Pedestal/View.hh"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_sys_window_REF.hh"
#include "Genie/FS/Views.hh"


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
		
		FSTreePtr link = window->Lookup( name );
		
		FSTreePtr target = link->ResolveLink();
		
		target->SetTimes();
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
	
	static boost::shared_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		return seize_ptr( new DefaultKey_Handler( delegate ) );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
	}
	
	
	static const FSTree_Premapped::Mapping local_mappings[] =
	{
		{ NULL, NULL }
	};
	
	FSTreePtr New_defaultkeys( const FSTreePtr& parent, const std::string& name )
	{
		return seize_ptr( new FSTree_new_View( parent,
		                                       name,
		                                       &CreateView,
		                                       local_mappings,
		                                       &DestroyDelegate ) );
	}
	
}

