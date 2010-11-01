/*	========
 *	Stack.cc
 *	========
 */

#include "Pedestal/Stack.hh"

// Pedestal
#include "Pedestal/Quasimode.hh"


namespace Pedestal
{
	
	void Stack::Install( const Rect& bounds )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			view.Install( bounds );
		}
	}
	
	void Stack::Uninstall()
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			view.Uninstall();
		}
	}
	
	void Stack::SetBounds( const Rect& bounds )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			view.SetBounds( bounds );
		}
	}
	
	void Stack::Draw( const Rect& bounds, bool erasing )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = count;  i > 0;  --i )
		{
			View& view = GetNthView( i );
			
			erasing = erasing  &&  i == count;
			
			view.Draw( bounds, erasing );
		}
	}
	
	void Stack::Activate( bool activating )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			view.Activate( activating );
		}
	}
	
	View* Stack::AdvanceFocus( View* current, bool backward )
	{
		const unsigned count = ViewCount();
		
		const unsigned first = backward ? count : 1;
		const unsigned last  = backward ? 1 : count;
		
		const int increment = backward ? -1 : 1;
		
		for ( unsigned i = first;  i != last + increment;  i += increment )
		{
			View& view = GetNthView( i );
			
			View* next = view.AdvanceFocus( current, backward );
			
			if ( next != current )
			{
				ASSERT( (next == NULL)  !=  (current == NULL) );
				
				if ( next != NULL )
				{
					return next;
				}
				
				current = next;
			}
		}
		
		return current;
	}
	
	bool Stack::MouseDown( const EventRecord& event )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			if ( view.MouseDown( event ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	bool Stack::KeyDown( const EventRecord& event )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			if ( view.KeyDown( event ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	bool Stack::HitTest( const EventRecord& event )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			if ( view.HitTest( event ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	void Stack::Idle( const EventRecord& event )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			view.Idle( event );
		}
	}
	
	bool Stack::SetCursor( const EventRecord& event, RgnHandle mouseRgn )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			if ( view.HitTest( event ) )
			{
				return view.SetCursor( event, mouseRgn );
				
				break;
			}
		}
		
		return false;
	}
	
	bool Stack::UserCommand( CommandCode code )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			if ( view.UserCommand( code ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	boost::intrusive_ptr< Quasimode >
	//
	Stack::EnterShiftSpaceQuasimode( const EventRecord& event )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			if ( boost::intrusive_ptr< Quasimode > mode = view.EnterShiftSpaceQuasimode( event ) )
			{
				return mode;
			}
		}
		
		return View::EnterShiftSpaceQuasimode( event );
	}
	
}

