/*	=========
 *	Window.hh
 *	=========
 */

#ifndef GENIE_IO_WINDOW_HH
#define GENIE_IO_WINDOW_HH

// Standard C++
#include <string>

// Nitrogen
#include "Nitrogen/MacWindows.h"
#include "Nitrogen/Str.h"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	class WindowHandle : public IOHandle
	{
		public:
			virtual ~WindowHandle()  {}
			
			static TypeCode Type()  { return kWindowType; }
			
			virtual TypeCode ActualType() const  { return Type(); }
			
			virtual void IOCtl( unsigned long request, int* argp );
			
			virtual Nitrogen::WindowRef GetWindowRef() const = 0;
			
			Nitrogen::Str255 GetTitle() const;
			
			void SetTitle( ConstStr255Param title );
			
			Point GetPosition() const;
			
			void SetPosition( Point position );
	};
	
}

#endif

