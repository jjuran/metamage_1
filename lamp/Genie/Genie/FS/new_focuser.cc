/*
	new_focuser.cc
	--------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/new_focuser.hh"

// Nitrogen
#include "Nitrogen/MacWindows.hh"

// Pedestal
#include "Pedestal/CurrentFocus.hh"
#include "Pedestal/View.hh"
#include "Pedestal/Window.hh"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	class Focuser : public Ped::View
	{
		private:
			const FSTree* itsKey;
			
			Ped::View* itsSavedFocus;
		
		public:
			Focuser( const FSTree* key ) : itsKey( key ), itsSavedFocus()
			{
			}
			
			void Activate( bool activating );
			
			bool MouseDown( const EventRecord& event )  { return false; }
			bool KeyDown( const EventRecord& event );
	};
	
	
	static Ped::View& RootView()
	{
		namespace N = Nitrogen;
		
		Ped::Window* base = N::GetWRefCon( N::FrontWindow() );
		
		return *base->GetView();
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
	
	static void SwitchFocus( bool backward = false )
	{
		Ped::View& root = RootView();
		
		Ped::View* current = Ped::Get_Focus();
		
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
			Ped::Set_Focus( AutoFocus( itsSavedFocus ) );
		}
		else
		{
			itsSavedFocus = Ped::Get_Focus();
			
			Ped::Reset_Focus();
		}
	}
	
	bool Focuser::KeyDown( const EventRecord& event )
	{
		const short disqualifyingModifiers = controlKey | rightControlKey
		                                   | optionKey  | rightOptionKey
		                                   | cmdKey;
		
		const bool advance = (event.modifiers & disqualifyingModifiers) == 0;
		
		const char c = event.message & charCodeMask;
		
		if ( advance  &&  c == kTabCharCode )
		{
			const bool shift = event.modifiers & (shiftKey | rightShiftKey);
			
			SwitchFocus( shift );
			
			return true;
		}
		
		if ( Ped::View* focus = Ped::Get_Focus() )
		{
			return focus->KeyDown( event );
		}
		
		return false;
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		return seize_ptr( new Focuser( delegate ) );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
	}
	
	
	static const FSTree_Premapped::Mapping local_mappings[] =
	{
		{ NULL, NULL }
	};
	
	FSTreePtr New_focuser( const FSTreePtr& parent, const plus::string& name )
	{
		return seize_ptr( new FSTree_new_View( parent,
		                                       name,
		                                       &CreateView,
		                                       local_mappings,
		                                       &DestroyDelegate ) );
	}
	
}

