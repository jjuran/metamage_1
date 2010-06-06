/*	=========
 *	Window.hh
 *	=========
 */

#ifndef PEDESTAL_WINDOW_HH
#define PEDESTAL_WINDOW_HH

// Debug
#include "debug/boost_assert.hh"

// plus
#include "plus/ref_count.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// nucleus
#include "nucleus/owned.hh"

// Nitrogen
#include "Mac/Windows/Types/WindowDefProcID.hh"
#include "Mac/Windows/Types/WindowRef.hh"

#include "Nitrogen/RefCon.hh"

// Pedestal
#include "Pedestal/MenuItemCode.hh"
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	void ResizeWindow( WindowRef window, Point newSize );
	
	
	Point GetWindowSize( WindowRef window );
	
	void SetWindowSize( WindowRef window, Point size );
	
	Point GetWindowPosition( WindowRef window );
	
	inline void SetWindowPosition( WindowRef window, Point position )
	{
		::MoveWindow( window, position.h, position.v, false );
	}
	
	struct NewWindowContext
	{
		const Rect&           bounds;
		ConstStr255Param      title;
		bool                  visible;
		Mac::WindowDefProcID  procID;
		WindowRef             behind;
		bool                  goAwayFlag;
		
		NewWindowContext( const Rect&           bounds,
			              ConstStr255Param      title,
			              bool                  visible    = true,
			              Mac::WindowDefProcID  procID     = Mac::documentProc,
			              WindowRef             behind     = kFirstWindowOfClass,
			              bool                  goAwayFlag = true )
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
			              WindowRef            behind,
			              bool                 goAwayFlag = true )
		:
			bounds    ( bounds     ),
			title     ( title      ),
			visible   ( visible    ),
			procID    ( Mac::documentProc ),
			behind    ( behind     ),
			goAwayFlag( goAwayFlag )
		{
		}
	};
	
	nucleus::owned< WindowRef > CreateWindow( const Rect&           bounds,
	                                          ConstStr255Param      title,
	                                          bool                  visible,
	                                          Mac::WindowDefProcID  procID,
	                                          WindowRef             behind,
	                                          bool                  goAwayFlag,
	                                          Nitrogen::RefCon      refCon );
	
	inline nucleus::owned< WindowRef > CreateWindow( const NewWindowContext&  context,
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
	
	void InvalidateWindowGrowBox( WindowRef window );
	
	
	class WindowCloseHandler : public plus::ref_count< WindowCloseHandler >
	{
		public:
			virtual void operator()( WindowRef window ) const = 0;
	};
	
	class WindowResizeHandler : public plus::ref_count< WindowResizeHandler >
	{
		public:
			virtual void operator()( WindowRef window, short h, short v ) const = 0;
	};
	
	class Window : public plus::ref_count< Window >
	{
		private:
			boost::intrusive_ptr< WindowCloseHandler  > itsCloseHandler;
			boost::intrusive_ptr< WindowResizeHandler > itsResizeHandler;
			
			nucleus::owned< WindowRef > itsWindowRef;
			
			Mac::WindowDefProcID itsDefProcID;
		
		public:
			Window( const NewWindowContext& context );
			
			~Window();
			
			WindowRef Get() const  { return itsWindowRef; }
			
			void SetCloseHandler( const boost::intrusive_ptr< WindowCloseHandler >& handler )
			{
				itsCloseHandler = handler;
			}
			
			void SetResizeHandler( const boost::intrusive_ptr< WindowResizeHandler >& handler )
			{
				itsResizeHandler = handler;
			}
			
			void Close( WindowRef window )  { return (*itsCloseHandler)( window ); }
			
			void Resize( WindowRef window, short h, short v );
			
			virtual boost::intrusive_ptr< View >& GetView() = 0;
			
			void SetView( boost::intrusive_ptr< View > const& view )  { GetView() = view; }
			
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

