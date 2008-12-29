/*	=========
 *	Window.hh
 *	=========
 */

#ifndef PEDESTAL_WINDOW_HH
#define PEDESTAL_WINDOW_HH

// Standard C++
#include <map>

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
		const Rect&          bounds;
		ConstStr255Param     title;
		bool                 visible;
		Nitrogen::WindowRef  behind;
		bool                 goAwayFlag;
		
		NewWindowContext( const Rect&          bounds,
			              ConstStr255Param     title,
			              bool                 visible    = true,
			              Nitrogen::WindowRef  behind     = kFirstWindowOfClass,
			              bool                 goAwayFlag = true )
		:
			bounds    ( bounds     ),
			title     ( title      ),
			visible   ( visible    ),
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
	
	inline Nucleus::Owned< Nitrogen::WindowRef > CreateWindow( const NewWindowContext&    context,
	                                                           Nitrogen::WindowDefProcID  procID,
	                                                           Nitrogen::RefCon           refCon )
	{
		return CreateWindow( context.bounds,
		                     context.title,
		                     context.visible,
		                     procID,
		                     context.behind,
		                     context.goAwayFlag,
		                     refCon );
	}
	
	void DrawWindow( Nitrogen::WindowRef window );
	
	void InvalidateWindowGrowBox( Nitrogen::WindowRef window );
	
	
	enum
	{
		kPedestalWindowKind = 1024
	};
	
	
	template < bool hasGrowIcon >
	struct DefProcID_HasGrowIcon
	{
		bool HasGrowIcon() const  { return hasGrowIcon; }
	};
	
	template < Nitrogen::WindowDefProcID defProcID >  struct DefProcID_Traits;
	
	template <>  struct DefProcID_Traits< Nitrogen::documentProc    > : DefProcID_HasGrowIcon< true  >  {};
	template <>  struct DefProcID_Traits< Nitrogen::dBoxProc        > : DefProcID_HasGrowIcon< false >  {};
	template <>  struct DefProcID_Traits< Nitrogen::plainDBox       > : DefProcID_HasGrowIcon< false >  {};
	template <>  struct DefProcID_Traits< Nitrogen::altDBoxProc     > : DefProcID_HasGrowIcon< false >  {};
	template <>  struct DefProcID_Traits< Nitrogen::noGrowDocProc   > : DefProcID_HasGrowIcon< false >  {};
	template <>  struct DefProcID_Traits< Nitrogen::movableDBoxProc > : DefProcID_HasGrowIcon< false >  {};
	template <>  struct DefProcID_Traits< Nitrogen::zoomDocProc     > : DefProcID_HasGrowIcon< true  >  {};
	template <>  struct DefProcID_Traits< Nitrogen::zoomNoGrow      > : DefProcID_HasGrowIcon< false >  {};
	
	template < Nitrogen::WindowDefProcID defProcID >
	struct Static_DefProcID : DefProcID_Traits< defProcID >
	{
		Nitrogen::WindowDefProcID Get() const  { return defProcID; }
	};
	
	class Variable_DefProcID
	{
		private:
			Nitrogen::WindowDefProcID itsDefProcID;
		
		public:
			Variable_DefProcID() : itsDefProcID()
			{
			}
			
			Variable_DefProcID( Nitrogen::WindowDefProcID procID ) : itsDefProcID( procID )
			{
			}
			
			Nitrogen::WindowDefProcID Get() const  { return itsDefProcID; }
			
			bool HasGrowIcon() const  { return (itsDefProcID & 0x7) == 0; }
	};
	
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
	
	class WindowRefOwner
	{
		private:
			Nucleus::Owned< Nitrogen::WindowRef > itsWindowRef;
		
		public:
			WindowRefOwner( Nucleus::Owned< Nitrogen::WindowRef > windowRef ) : itsWindowRef( windowRef )
			{
			}
			
			Nitrogen::WindowRef Get() const  { return itsWindowRef; }
			
	};
	
	class ClosableWindow
	{
		private:
			boost::shared_ptr< WindowCloseHandler > itsCloseHandler;
		
		protected:
			// Non-sliceable
			~ClosableWindow();
		
		public:
			void SetCloseHandler( const boost::shared_ptr< WindowCloseHandler >& handler )
			{
				itsCloseHandler = handler;
			}
			
			void Close( Nitrogen::WindowRef window )  { return (*itsCloseHandler)( window ); }
	};
	
	class ResizableWindow
	{
		private:
			boost::shared_ptr< WindowResizeHandler > itsResizeHandler;
		
		protected:
			// Non-sliceable
			~ResizableWindow();
		
		public:
			ResizableWindow()
			{
			}
			
			ResizableWindow( const boost::shared_ptr< WindowResizeHandler >& handler ) : itsResizeHandler( handler )
			{
			}
			
			void SetResizeHandler( const boost::shared_ptr< WindowResizeHandler >& handler )
			{
				itsResizeHandler = handler;
			}
			
			void Resize( Nitrogen::WindowRef window, short h, short v );
	};
	
	class Window : public ClosableWindow, public ResizableWindow,
	               public WindowRefOwner
	{
		public:
			typedef Variable_DefProcID DefProcID;
		
		private:
			Variable_DefProcID         itsDefProcID;
			boost::shared_ptr< View >  itsView;
		
		public:
			Window( const NewWindowContext&  context,
			        DefProcID                defProcID = DefProcID() );
			
			View const& GetView() const  { return *itsView; }
			View      & GetView()        { return *itsView; }
			
			template < class ViewType >
			ViewType& SubView()  { return static_cast< ViewType& >( *itsView ); }
			
			template < class ViewType >
			const ViewType& SubView() const  { return static_cast< const ViewType& >( *itsView ); }
			
			void SetView( boost::shared_ptr< View > const& view )  { itsView = view; }
			void SetView( std::auto_ptr    < View >        view )  { itsView = view; }
			
			void InvalidateGrowBox() const
			{
				if ( itsDefProcID.HasGrowIcon() )
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

