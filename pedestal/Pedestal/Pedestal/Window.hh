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
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	struct NewWindowContext
	{
		const Rect&         bounds;
		ConstStr255Param    title;
		bool                visible;
		N::WindowRef        behind;
		bool                goAwayFlag;
		
		NewWindowContext( const Rect&         bounds,
			              ConstStr255Param    title,
			              bool                visible    = true,
			              N::WindowRef        behind     = kFirstWindowOfClass,
			              bool                goAwayFlag = true )
		:
			bounds    ( bounds     ),
			title     ( title      ),
			visible   ( visible    ),
			behind    ( behind     ),
			goAwayFlag( goAwayFlag )
		{}
	};
	
	NN::Owned< N::WindowRef > CreateWindow( const Rect& bounds,
	                                        ConstStr255Param title,
	                                        bool visible,
	                                        N::WindowDefProcID procID,
	                                        N::WindowRef behind,
	                                        bool goAwayFlag,
	                                        N::RefCon refCon );
	
	inline NN::Owned< N::WindowRef > CreateWindow( const NewWindowContext&  context,
	                                               N::WindowDefProcID       procID,
	                                               N::RefCon                refCon )
	{
		return CreateWindow( context.bounds,
		                     context.title,
		                     context.visible,
		                     procID,
		                     context.behind,
		                     context.goAwayFlag,
		                     refCon );
	}
	
	void DrawWindow( N::WindowRef window );
	
	void InvalidateGrowBox( N::WindowRef window );
	
	
	enum
	{
		kPedestalWindowKind = 1024
	};
	
	
	template < bool hasGrowIcon >
	struct DefProcID_HasGrowIcon
	{
		bool HasGrowIcon() const  { return hasGrowIcon; }
	};
	
	template < N::WindowDefProcID defProcID >  struct DefProcID_Traits;
	
	template <>  struct DefProcID_Traits< N::documentProc    > : DefProcID_HasGrowIcon< true  >  {};
	template <>  struct DefProcID_Traits< N::dBoxProc        > : DefProcID_HasGrowIcon< false >  {};
	template <>  struct DefProcID_Traits< N::plainDBox       > : DefProcID_HasGrowIcon< false >  {};
	template <>  struct DefProcID_Traits< N::altDBoxProc     > : DefProcID_HasGrowIcon< false >  {};
	template <>  struct DefProcID_Traits< N::noGrowDocProc   > : DefProcID_HasGrowIcon< false >  {};
	template <>  struct DefProcID_Traits< N::movableDBoxProc > : DefProcID_HasGrowIcon< false >  {};
	template <>  struct DefProcID_Traits< N::zoomDocProc     > : DefProcID_HasGrowIcon< true  >  {};
	template <>  struct DefProcID_Traits< N::zoomNoGrow      > : DefProcID_HasGrowIcon< false >  {};
	
	class WindowCloseHandler
	{
		public:
			virtual void operator()( N::WindowRef window ) const = 0;
	};
	
	class WindowResizeHandler
	{
		public:
			virtual void operator()( N::WindowRef window, short h, short v ) const = 0;
	};
	
	class WindowRefOwner
	{
		private:
			NN::Owned< N::WindowRef > windowRef;
		
		public:
			WindowRefOwner( NN::Owned< N::WindowRef > windowRef ) : windowRef( windowRef )  {}
			
			N::WindowRef Get()      const  { return windowRef; }
			operator N::WindowRef() const  { return Get();     }
			
	};
	
	class ClosableWindow
	{
		private:
			boost::shared_ptr< WindowCloseHandler > itsCloseHandler;
		
		protected:
			// Non-sliceable
			~ClosableWindow();
		
		public:
			ClosableWindow( const boost::shared_ptr< WindowCloseHandler >& handler ) : itsCloseHandler( handler )  {}
			
			void SetCloseHandler( const boost::shared_ptr< WindowCloseHandler >& handler )
			{
				itsCloseHandler = handler;
			}
			
			void Close( N::WindowRef window )  { return (*itsCloseHandler)( window ); }
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
			
			void Resize( N::WindowRef window, short h, short v )
			{
				if ( const WindowResizeHandler* handler = itsResizeHandler.get() )
				{
					(*handler)( window, h, v );
				}
				else
				{
					Nitrogen::SizeWindow( window, h, v, true );
				}
			}
	};
	
	class Quasimode;
	
	class WindowBase : public ClosableWindow, public ResizableWindow
	{
		public:
			WindowBase( const boost::shared_ptr< WindowCloseHandler >& handler ) : ClosableWindow( handler )  {}
			
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
			WindowCore( const boost::shared_ptr< WindowCloseHandler >&  handler,
			            NN::Owned< N::WindowRef >                       window ) : WindowBase    ( handler ),
			                                                                       WindowRefOwner( window  )
			{
			}
	};
	
	template < class Type, N::WindowDefProcID defProcID = N::documentProc >
	class Window : private DefProcID_Traits< defProcID >,
	               public  WindowCore
	{
		private:
			Type mySubView;
		
		public:
			typedef Type SubViewType;
			typedef typename Type::Initializer Initializer;
			
			Window( const NewWindowContext&                         context,
			        const boost::shared_ptr< WindowCloseHandler >&  handler,
			        Initializer                                     init );
			
			Type const& SubView() const  { return mySubView; }
			Type      & SubView()        { return mySubView; }
			
			void Idle       ( const EventRecord& event  )  { SubView().Idle( event );              }
			bool KeyDown    ( const EventRecord& event  )  { return SubView().KeyDown( event );    }
			void Activate   ( bool activating           )  { SubView().Activate( activating    );  InvalidateGrowBox( Get() ); }
			void Resized    ( short width, short height )  { SubView().Resize  ( width, height );  InvalidateGrowBox( Get() ); }
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
	
	template < class Type, N::WindowDefProcID defProcID >
	inline Window< Type, defProcID >::Window( const NewWindowContext&                         context,
	                                          const boost::shared_ptr< WindowCloseHandler >&  handler,
	                                          Initializer                                     init = Initializer() )
	:
		WindowCore( handler,
		            CreateWindow( context,
		                          defProcID,
		                          static_cast< WindowBase* >( this ) ) ),
		mySubView ( N::GlobalToLocal( context.bounds ), init )
	{
	}
	
	template < class Type, N::WindowDefProcID defProcID >
	inline void Window< Type, defProcID >::MouseDown( const EventRecord& event )
	{
		// FIXME:  The window may want clicks even if it's not in front.
		if ( Get() != N::FrontWindow() )
		{
			N::SelectWindow( Get() );
		}
		else
		{
			SubView().MouseDown( event );
		}
	}
	
	template < class Type, N::WindowDefProcID defProcID >
	inline void Window< Type, defProcID >::Update()
	{
		SubView().Update();
		
		if ( DefProcID_Traits< defProcID >::HasGrowIcon() )
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
				
				void operator()( N::WindowRef /*window*/ ) const
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
			N::SelectWindow( itsWindow->Get() );
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
				
				void operator()( N::WindowRef window ) const
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

