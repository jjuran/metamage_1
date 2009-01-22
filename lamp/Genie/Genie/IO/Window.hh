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

// Pedestal
#include "Pedestal/UserWindow.hh"

// Genie
#include "Genie/IO/DynamicGroup.hh"
#include "Genie/IO/Terminal.hh"


namespace Pedestal
{
	
	class WindowCloseHandler;
	class WindowResizeHandler;
	
}

namespace Genie
{
	
	class WindowHandle : public Pedestal::UserWindow,
	                     public TerminalHandle
	{
		public:
			WindowHandle( const Pedestal::NewWindowContext&  context,
			              Nitrogen::WindowDefProcID          procID,
			              const std::string&                 name );
			
			virtual ~WindowHandle();
	};
	
	
	boost::shared_ptr< Pedestal::WindowCloseHandler > GetDynamicWindowCloseHandler( DynamicGroup&     group,
	                                                                                DynamicElementID  id );
	
	template < class Handle >
	boost::shared_ptr< Pedestal::WindowCloseHandler > GetDynamicWindowCloseHandler( DynamicElementID  id )
	{
		return GetDynamicWindowCloseHandler( GetDynamicGroup< Handle >(), id );
	}
	
	boost::shared_ptr< Pedestal::WindowResizeHandler > GetDynamicWindowResizeHandler( DynamicGroup&     group,
	                                                                                  DynamicElementID  id );
	
	template < class Handle >
	boost::shared_ptr< Pedestal::WindowResizeHandler > GetDynamicWindowResizeHandler( DynamicElementID  id )
	{
		return GetDynamicWindowResizeHandler( GetDynamicGroup< Handle >(), id );
	}
	
}

#endif

