/*	=========
 *	Window.hh
 *	=========
 */

#ifndef PEDESTAL_WINDOW_HH
#define PEDESTAL_WINDOW_HH

// Standard C++
#include <map>

// Boost
#include <boost/enable_shared_from_this.hpp>
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
	
	class WindowClosure : public boost::enable_shared_from_this< WindowClosure >
	{
		public:
			virtual bool RequestWindowClosure( N::WindowRef window ) = 0;
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
	
	class WindowBase
	{
		public:
			virtual WindowClosure& Closure() = 0;
			
			virtual void Idle       ( const EventRecord& event           ) = 0;
			virtual void MouseDown  ( const EventRecord& event           ) = 0;
			virtual bool KeyDown    ( const EventRecord& event           ) = 0;
			virtual void Update     (                                    ) = 0;
			virtual void Activate   ( bool activating                    ) = 0;
			virtual bool SetCursor  ( Point location, RgnHandle mouseRgn ) = 0;
			virtual void Resized    ( const Rect& newBounds              ) = 0;
			virtual bool UserCommand( MenuItemCode code                  ) = 0;
	};
	
	class ClosableWindow
	{
		private:
			boost::shared_ptr< WindowClosure > itsClosure;
		
		public:
			ClosableWindow( WindowClosure& closure ) : itsClosure( closure.shared_from_this() )  {}
			
			WindowClosure& Closure()  { return *itsClosure; }
			
			void SetCloseHandler( const boost::shared_ptr< WindowClosure >& handler )
			{
				itsClosure = handler;
			}
	};
	
	template < class Type, N::WindowDefProcID defProcID = N::documentProc >
	class Window : private DefProcID_Traits< defProcID >,
	               public  WindowBase,
	               public  ClosableWindow,
	               public  WindowRefOwner
	{
		private:
			Type mySubView;
		
		public:
			typedef Type SubViewType;
			typedef typename Type::Initializer Initializer;
			
			WindowClosure& Closure()  { return ClosableWindow::Closure(); }
			
			Window( const NewWindowContext&  context,
			        WindowClosure&           closure,
			        Initializer              init );
			
			Type const& SubView() const  { return mySubView; }
			Type      & SubView()        { return mySubView; }
			
			void Idle       ( const EventRecord& event )  { SubView().Idle( event );              }
			bool KeyDown    ( const EventRecord& event )  { return SubView().KeyDown( event );    }
			void Activate   ( bool activating          )  { SubView().Activate( activating );     }
			void Resized    ( const Rect& newBounds    )  { SubView().Resize( newBounds );        }
			bool UserCommand( MenuItemCode code        )  { return SubView().UserCommand( code ); }
			
			bool SetCursor( Point      location,
			                RgnHandle  mouseRgn )
			{
				return SubView().SetCursor( location, mouseRgn );
			}
			
			void MouseDown( const EventRecord& event );
			
			void Update();
			
	};
	
	template < class Type, N::WindowDefProcID defProcID >
	inline Window< Type, defProcID >::Window( const NewWindowContext&  context,
	                                          WindowClosure&           closure,
	                                          Initializer              init = Initializer() )
	:
		ClosableWindow( closure ),
		WindowRefOwner( CreateWindow( context,
		                              defProcID,
		                              static_cast< WindowBase* >( this ) ) ),
		mySubView     ( N::GlobalToLocal( context.bounds ), init )
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
			WindowStorage         fWindow;
		
		public:
			SingleWindowOwner( WindowClosure& closure )
			:
				fWindow( new Window( closure ) )
			{}
			
			Window const& Get() const  { return *fWindow.get(); }
			Window      & Get()        { return *fWindow.get(); }
	};
	
	// A unique window such as a modeless about box that's sometimes open and sometimes not.
	template < class Window >
	class UniqueWindowOwner
	{
		typedef std::auto_ptr< Window > WindowStorage;
		
		class Closure : public WindowClosure
		{
			private:
				WindowStorage& storage;
			
			public:
				Closure( WindowStorage& storage ) : storage( storage )  {}
				
				bool RequestWindowClosure( N::WindowRef /*window*/ )
				{
					// assert( storage.get() );
					// assert( storage->Get() == window );
					
					storage.reset( NULL );
					
					return true;
				}
		};
		
		protected:
			WindowStorage  fWindow;
			Closure        fClosure;
		
		public:
			UniqueWindowOwner() : fClosure( fWindow )  {}
			
			Window& Get()  { return *fWindow.get(); }
			
			void Show();
	};
	
	template < class Window >
	inline void UniqueWindowOwner< Window >::Show()
	{
		if ( fWindow.get() )
		{
			N::SelectWindow( fWindow->Get() );
		}
		else
		{
			fWindow.reset( new Window( fClosure ) );
		}
	}
	
	// A collection of windows of the same type.  Requests to close are always accepted.
	template < class Window >
	class WindowsOwner
	{
		typedef std::map< ::WindowRef, boost::shared_ptr< Window > > WindowStorage;
		
		class Closure : public WindowClosure
		{
			private:
				WindowStorage& windows;
			
			public:
				Closure( WindowStorage& windows )
				:
					windows( windows  )
				{}
				
				bool RequestWindowClosure( N::WindowRef window )
				{
					typename WindowStorage::iterator found = windows.find( window );
					
					if ( found != windows.end() )
					{
						windows.erase( found );
					}
					
					return true;
				}
		};
		
		protected:
			WindowStorage  fWindows;
			Closure        fClosure;
		
		public:
			WindowsOwner() : fClosure( fWindows )  {}
			
			void NewWindow()
			{
				Window* window = new Window( fClosure );
				
				fWindows[ window->Get() ] = boost::shared_ptr< Window >( window );
			}
	};
	
}

#endif

