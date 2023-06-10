/*	========
 *	Stack.cc
 *	========
 */

#include "Pedestal/Stack.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef CGGEOMETRY_H_
#ifndef __CGGEOMETRY__
#include <CGGeometry.h>
#endif
#endif

// debug
#include "debug/assert.hh"


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
	
	void Stack::DrawInContext( CGContextRef context, CGRect bounds )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = count;  i > 0;  --i )
		{
			View& view = GetNthView( i );
			
			view.DrawInContext( context, bounds );
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
	
	void Stack::KeyUp( const EventRecord& event )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			view.KeyUp( event );
		}
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
	
	bool Stack::SetCursor( const EventRecord& event )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			if ( view.HitTest( event ) )
			{
				return view.SetCursor( event );
				
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
	
	TextEdit*
	//
	Stack::EnterShiftSpaceQuasimode( const EventRecord& event )
	{
		const unsigned count = ViewCount();
		
		for ( unsigned i = 1;  i <= count;  ++i )
		{
			View& view = GetNthView( i );
			
			if ( TextEdit* textedit = view.EnterShiftSpaceQuasimode( event ) )
			{
				return textedit;
			}
		}
		
		return View::EnterShiftSpaceQuasimode( event );
	}
	
}
