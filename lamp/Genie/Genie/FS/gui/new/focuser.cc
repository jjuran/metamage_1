/*
	gui/new/focuser.cc
	------------------
*/

#include "Genie/FS/gui/new/focuser.hh"

// Nitrogen
#include "Nitrogen/MacWindows.hh"

// Pedestal
#include "Pedestal/CurrentFocus.hh"
#include "Pedestal/View.hh"
#include "Pedestal/Window.hh"
#include "Pedestal/WindowStorage.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node/types/fixed_dir.hh"

// Genie
#include "Genie/FS/focusable_views.hh"
#include "Genie/FS/Views.hh"
#include "Genie/FS/gui/port/ADDR.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	class Focuser : public Ped::View
	{
		private:
			const vfs::node* itsKey;
		
		public:
			Focuser( const vfs::node* key ) : itsKey( key )
			{
			}
			
			Ped::View* GetFocus() const;
			
			void Activate( bool activating );
			
			bool MouseDown( const EventRecord& event )  { return false; }
			bool KeyDown( const EventRecord& event );
	};
	
	
	Ped::View* Focuser::GetFocus() const
	{
		const vfs::node* focus = get_port_focus( GetViewWindowKey( itsKey ) );
			
		return get_focusable_view( focus );
	}
	
	
	static Ped::View& RootView()
	{
		return *Ped::get_window_view( FrontWindow() );
	}
	
	static Ped::View* AutoFocus( Ped::View* current )
	{
		Ped::View& root = RootView();
		
		// Input to AdvanceFocus() is either the current focus or NULL
		
		Ped::View* next = root.AdvanceFocus( current );
		
		if ( current != NULL  &&  next != current )
		{
			// We already have a valid focus
			return current;
		}
		
		// next will only match a non-null current if the view isn't found.
		// Such a view exists, but is outside the hierarchy.  This shouldn't be.
		
		// next and current match and are non-null if the focus was removed.
		
		const bool current_focus_removed = current != NULL  &&  next == current;
		
		if ( current_focus_removed )
		{
			// The current focus doesn't exist anymore.
			
			// Try again starting with NULL to hit the first/last field.
			
			next = root.AdvanceFocus( NULL );
		}
		
		if ( next != NULL )
		{
			next->Focus();
		}
		
		return next;
	}
	
	static void SwitchFocus( Ped::View* current, bool backward = false )
	{
		Ped::View& root = RootView();
		
		// Input to AdvanceFocus() is either the current focus or NULL
		
		Ped::View* next = root.AdvanceFocus( current, backward );
		
		// next will only match a non-null current if the view isn't found.
		// Such a view exists, but is outside the hierarchy.  This shouldn't be.
		
		// next and current match and are non-null if the focus was removed.
		
		const bool current_focus_removed = current != NULL  &&  next == current;
		
		const bool need_to_wrap_around = current != NULL  &&  next == NULL;
		
		if ( current_focus_removed )
		{
			current = NULL;
		}
		
		const bool need_to_retry = current_focus_removed  ||  need_to_wrap_around;
		
		if ( need_to_retry )
		{
			// current_focus_removed:
			// The current focus doesn't exist anymore.
			
			// need_to_wrap_around:
			// A focus exists, but no field follows/precedes it.  This occurs if
			// you advance/retreat from the last/first or only field.
			
			// Try again starting with NULL to hit the first/last field.
			
			next = root.AdvanceFocus( NULL, backward );
			
			if ( need_to_wrap_around )
			{
				ASSERT( next != NULL );
			}
		}
		
		if ( next == current )
		{
			// There aren't multiple focusable views -- only one or zero.
			// Do nothing.
			
			return;
		}
		
		if ( current != NULL )
		{
			current->Blur();
		}
		
		Ped::Set_Focus( next );
		
		next->Focus();
		next->Cue();
	}
	
	void Focuser::Activate( bool activating )
	{
		if ( activating )
		{
			Ped::Set_Focus( AutoFocus( GetFocus() ) );
		}
		else
		{
			Ped::Reset_Focus();
		}
	}
	
	bool Focuser::KeyDown( const EventRecord& event )
	{
		Ped::View* focus = GetFocus();
		
		const UInt16 disqualifyingModifiers = controlKey | rightControlKey
		                                    | optionKey  | rightOptionKey
		                                    | cmdKey;
		
		const bool advance = (event.modifiers & disqualifyingModifiers) == 0;
		
		const char c = event.message;
		
		if ( advance  &&  c == kTabCharCode )
		{
			const bool shift = event.modifiers & (shiftKey | rightShiftKey);
			
			SwitchFocus( focus, shift );
			
			return true;
		}
		
		if ( focus != NULL )
		{
			return focus->KeyDown( event );
		}
		
		return false;
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		return new Focuser( delegate );
	}
	
	
	vfs::node_ptr New_focuser( const vfs::node*     parent,
	                           const plus::string&  name,
	                           const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView );
	}
	
}
