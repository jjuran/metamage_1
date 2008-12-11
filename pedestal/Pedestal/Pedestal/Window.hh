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
			Nucleus::Owned< Nitrogen::WindowRef > windowRef;
		
		public:
			WindowRefOwner( Nucleus::Owned< Nitrogen::WindowRef > windowRef ) : windowRef( windowRef )
			{
			}
			
			Nitrogen::WindowRef Get()      const  { return windowRef; }
			operator Nitrogen::WindowRef() const  { return Get();     }
			
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
	
	class Quasimode;
	
	class WindowBase : public ClosableWindow, public ResizableWindow
	{
		public:
			virtual void Idle       ( const EventRecord& event           ) = 0;
			virtual void MouseDown  ( const EventRecord& event           ) = 0;
			virtual bool KeyDown    ( const EventRecord& event           ) = 0;
			virtual void Update     (                                    ) = 0;
			virtual void Activate   ( bool activating                    ) = 0;
			virtual bool SetCursor  ( Point location, RgnHandle mouseRgn ) = 0;
			virtual void Resized    ( short width, short height          ) = 0;
			virtual bool UserCommand( MenuItemCode code                  ) = 0;
			
			virtual boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& event ) = 0;
	};
	
	class WindowCore : public WindowBase,
	                   public WindowRefOwner
	{
		public:
			WindowCore( Nucleus::Owned< Nitrogen::WindowRef > window ) : WindowRefOwner( window  )
			{
			}
	};
	
	template < class Type, class DefProcID = Static_DefProcID< Nitrogen::documentProc > >
	class Window : public WindowCore
	{
		private:
			DefProcID  itsDefProcID;
			Type       itsSubView;
		
		public:
			typedef Type SubViewType;
			typedef typename Type::Initializer Initializer;
			
			Window( const NewWindowContext&  context,
			        DefProcID                defProcID = DefProcID(),
			        Initializer              init      = Initializer() );
			
			Window( const NewWindowContext&  context,
			        Initializer              init );
			
			Type const& SubView() const  { return itsSubView; }
			Type      & SubView()        { return itsSubView; }
			
			void InvalidateGrowBox() const
			{
				if ( itsDefProcID.HasGrowIcon() )
				{
					InvalidateWindowGrowBox( Get() );
				}
			}
			
			void Idle       ( const EventRecord& event  )  { SubView().Idle( event );              }
			bool KeyDown    ( const EventRecord& event  )  { return SubView().KeyDown( event );    }
			void Activate   ( bool activating           )  { SubView().Activate( activating    );  InvalidateGrowBox(); }
			void Resized    ( short width, short height )  { SubView().Resize  ( width, height );  InvalidateGrowBox(); }
			bool UserCommand( MenuItemCode code         )  { return SubView().UserCommand( code ); }
			
			boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& event )
			{
				return SubView().EnterShiftSpaceQuasimode( event );
			}
			
			bool SetCursor( Point      location,
			                RgnHandle  mouseRgn )
			{
				return SubView().SetCursor( location, mouseRgn );
			}
			
			void MouseDown( const EventRecord& event );
			
			void Update();
	};
	
	template < class Type, class DefProcID >
	inline Window< Type, DefProcID >::Window( const NewWindowContext&  context,
	                                          DefProcID                defProcID,
	                                          Initializer              init )
	:
		WindowCore( CreateWindow( context,
		                          defProcID.Get(),
		                          static_cast< WindowBase* >( this ) ) ),
		itsDefProcID( defProcID ),
		itsSubView  ( Nitrogen::GlobalToLocal( context.bounds ), init )
	{
	}
	
	template < class Type, class DefProcID >
	inline Window< Type, DefProcID >::Window( const NewWindowContext&  context,
	                                          Initializer              init )
	:
		WindowCore( CreateWindow( context,
		                          DefProcID().Get(),
		                          static_cast< WindowBase* >( this ) ) ),
		itsDefProcID( DefProcID() ),
		itsSubView  ( Nitrogen::GlobalToLocal( context.bounds ), init )
	{
	}
	
	template < class Type, class DefProcID >
	inline void Window< Type, DefProcID >::MouseDown( const EventRecord& event )
	{
		// FIXME:  The window may want clicks even if it's not in front.
		if ( Get() != Nitrogen::FrontWindow() )
		{
			Nitrogen::SelectWindow( Get() );
		}
		else
		{
			SubView().MouseDown( event );
		}
	}
	
	template < class Type, class DefProcID >
	inline void Window< Type, DefProcID >::Update()
	{
		SubView().Update();
		
		if ( itsDefProcID.HasGrowIcon() )
		{
			DrawWindow( Get() );
		}
	}
	
	
	// A single window that exists for the duration of the owner.
	// This could be a member of a document object.
	template < class Window >
	class SingleWindowOwner
	{
		typedef std::auto_ptr< Window > WindowStorage;
		
		protected:
			WindowStorage         itsWindow;
		
		public:
			SingleWindowOwner( const boost::shared_ptr< WindowCloseHandler >& handler ) : itsWindow( new Window( handler ) )
			{
			}
			
			Window const& Get() const  { return *itsWindow.get(); }
			Window      & Get()        { return *itsWindow.get(); }
	};
	
	// A unique window such as a modeless about box that's sometimes open and sometimes not.
	template < class Window >
	class UniqueWindowOwner
	{
		typedef std::auto_ptr< Window > WindowStorage;
		
		class CloseHandler : public WindowCloseHandler
		{
			private:
				WindowStorage& storage;
			
			public:
				CloseHandler( WindowStorage& storage ) : storage( storage )  {}
				
				void operator()( Nitrogen::WindowRef /*window*/ ) const
				{
					// assert( storage.get() );
					// assert( storage->Get() == window );
					
					storage.reset( NULL );
				}
		};
		
		protected:
			WindowStorage                            itsWindow;
			boost::shared_ptr< WindowCloseHandler >  itsCloseHandler;
		
		public:
			UniqueWindowOwner() : itsCloseHandler( new CloseHandler( itsWindow ) )  {}
			
			Window& Get()  { return *itsWindow.get(); }
			
			void Show();
	};
	
	template < class Window >
	inline void UniqueWindowOwner< Window >::Show()
	{
		if ( itsWindow.get() )
		{
			Nitrogen::SelectWindow( itsWindow->Get() );
		}
		else
		{
			itsWindow.reset( new Window( itsCloseHandler ) );
		}
	}
	
	// A collection of windows of the same type.  Requests to close are always accepted.
	template < class Window >
	class WindowsOwner
	{
		typedef std::map< ::WindowRef, boost::shared_ptr< Window > > WindowStorage;
		
		class CloseHandler : public WindowCloseHandler
		{
			private:
				WindowStorage& itsWindows;
			
			public:
				CloseHandler( WindowStorage& windows ) : itsWindows( windows  )  {}
				
				void operator()( Nitrogen::WindowRef window ) const
				{
					typename WindowStorage::iterator found = itsWindows.find( window );
					
					if ( found != itsWindows.end() )
					{
						itsWindows.erase( found );
					}
				}
		};
		
		protected:
			WindowStorage                            itsWindows;
			boost::shared_ptr< WindowCloseHandler >  itsCloseHandler;
		
		public:
			WindowsOwner() : itsCloseHandler( new CloseHandler( itsWindows ) )  {}
			
			void NewWindow()
			{
				Window* w = new Window( itsCloseHandler );
				
				boost::shared_ptr< Window > window( w );
				
				itsWindows[ w->Get() ] = window;
			}
	};
	
}

#endif

