/*	=========
 *	Window.cc
 *	=========
 */

#include "UseEdit/Window.hh"

// Nucleus
#include "Nucleus/Convert.h"


namespace UseEdit
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	static Rect MakeWindowRect()
	{
		Rect screenBounds = N::GetQDGlobalsScreenBits().bounds;
		
		Rect rect = N::SetRect(0, 18, 2*4+6*80+16, 18+2*4+11*25+16);
		short mbarHeight = ::GetMBarHeight();
		short vMargin = (screenBounds.bottom - rect.bottom) - mbarHeight;
		short hMargin = (screenBounds.right - rect.right);
		
		return N::OffsetRect(rect, hMargin / 2, mbarHeight + vMargin / 3);
	}
	
	Window::Window( Ped::WindowClosure& closure, ConstStr255Param title )
	: 
		Base( Ped::NewWindowContext( MakeWindowRect(), title ), closure )
	{
	}
	
	std::string Window::GetName() const
	{
		return NN::Convert< std::string >( N::GetWTitle( Get() ) );
	}
	
	void Window::SetName( ConstStr255Param name )
	{
		N::SetWTitle( Get(), name );
	}
	
	void Window::SetName( const std::string& name )
	{
		SetName( N::Str255( name ) );
	}
	
}

