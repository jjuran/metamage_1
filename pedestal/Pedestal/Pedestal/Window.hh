/*	=========
 *	Window.hh
 *	=========
 */

#ifndef PEDESTAL_WINDOW_HH
#define PEDESTAL_WINDOW_HH

// Standard C++
#include <map>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/shared_ptr.hpp>

// Nucleus
#include "Nucleus/Owned.h"

// Nitrogen
#include "Nitrogen/MacWindows.h"

// Pedestal
#include "Pedestal/MenuItemCode.hh"
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	void ResizeWindow( Nitrogen::WindowRef window, Point newSize );
	
	
	Point GetWindowSize( Nitrogen::WindowRef window );
	
	void SetWindowSize( Nitrogen::WindowRef window, Point size );
	
	Point GetWindowPosition( Nitrogen::WindowRef window );
	
	inline void SetWindowPosition( Nitrogen::WindowRef window, Point position )
	{
		Nitrogen::MoveWindow( window, position );
	}
	
	struct NewWindowContext
	{
		const Rect&                bounds;
		ConstStr255Param           title;
		bool                       visible;
		Nitrogen::WindowDefProcID  procID;
		Nitrogen::WindowRef        behind;
		bool                       goAwayFlag;
		
		NewWindowContext( const Rect&                bounds,
			              ConstStr255Param           title,
			              bool                       visible    = true,
			              Nitrogen::WindowDefProcID  procID     = Nitrogen::documentProc,
			              Nitrogen::WindowRef        behind     = kFirstWindowOfClass,
			              bool                       goAwayFlag = true )
		:
			bounds    ( bounds     ),
			title     ( title      ),
			visible   ( visible    ),
			procID    ( procID     ),
			behind    ( behind     ),
			goAwayFlag( goAwayFlag )
		{
		}
		
		NewWindowContext( const Rect&          bounds,
			              ConstStr255Param     title,
			              bool                 visible,
			              Nitrogen::WindowRef  behind,
			              bool                 goAwayFlag = true )
		:
			bounds    ( bounds     ),
			title     ( title      ),
			visible   ( visible    ),
			procID    ( Nitrogen::documentProc ),
			behind    ( behind     ),
			goAwayFlag( goAwayFlag )
		{
		}
	};
	
	Nucleus::Owned< Nitrogen::WindowRef > CreateWindow( const Rect&                bounds,
	                                                    ConstStr255Param           title,
	                                                    bool                       visible,
	                                                    Nitrogen::WindowDefProcID  procID,
	                                                    Nitrogen::WindowRef        behind,
	                                                    bool                       goAwayFlag,
	                                                    Nitrogen::RefCon           refCon );
	
	inline Nucleus::Owned< Nitrogen::WindowRef > CreateWindow( const NewWindowContext&  context,
	                                                           Nitrogen::RefCon         refCon )
	{
		return CreateWindow( context.bounds,
		                     context.title,
		                     context.visible,
		                     context.procID,
		                     context.behind,
		                     context.goAwayFlag,
		                     refCon );
	}
	
	void InvalidateWindowGrowBox( Nitrogen::WindowRef window );
	
	
	class WindowCloseHandler
	{
		public:
			virtual void operator()( Nitrogen::WindowRef window ) const = 0;
	};
	
	class WindowResizeHandler
	{
		public:
			virtual void operator()( Nitrogen::WindowRef window, short h, short v ) const = 0;
	};
	
	class Window
	{
		private:
			boost::shared_ptr< WindowCloseHandler  > itsCloseHandler;
			boost::shared_ptr< WindowResizeHandler > itsResizeHandler;
			
			Nucleus::Owned< Nitrogen::WindowRef > itsWindowRef;
			
			Nitrogen::WindowDefProcID itsDefProcID;
		
		public:
			Window( const NewWindowContext& context );
			
			~Window();
			
			Nitrogen::WindowRef Get() const  { return itsWindowRef; }
			
			void SetCloseHandler( const boost::shared_ptr< WindowCloseHandler >& handler )
			{
				itsCloseHandler = handler;
			}
			
			void SetResizeHandler( const boost::shared_ptr< WindowResizeHandler >& handler )
			{
				itsResizeHandler = handler;
			}
			
			void Close( Nitrogen::WindowRef window )  { return (*itsCloseHandler)( window ); }
			
			void Resize( Nitrogen::WindowRef window, short h, short v );
			
			virtual boost::shared_ptr< View >& GetView() = 0;
			
			void SetView( boost::shared_ptr< View > const& view )  { GetView() = view; }
			void SetView( std::auto_ptr    < View >        view )  { GetView() = view; }
			
			bool HasGrowIcon() const  { return (itsDefProcID & 0x7) == 0; }
			
			void InvalidateGrowBox() const
			{
				if ( HasGrowIcon() )
				{
					InvalidateWindowGrowBox( Get() );
				}
			}
			
			void Activate( bool activating           );
			void Resized ( short width, short height );
			
			void MouseDown( const EventRecord& event );
			
			void Update();
	};
	
}

#endif

