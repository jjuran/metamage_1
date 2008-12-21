/*	========================
 *	FSTree_sys_window_REF.cc
 *	========================
 */

#include "Genie/FileSystem/FSTree_sys_window_REF.hh"

// Nucleus
#include "Nucleus/Saved.h"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/EmptyView.hh"
#include "Pedestal/UserWindow.hh"

// Genie
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/FileSystem/Views.hh"
#include "Genie/IO/Terminal.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	static const Point gZeroPoint = { 0 };
	
	struct WindowParameters
	{
		N::Str255  itsTitle;
		Point      itsOrigin;
		Point      itsSize;
		bool       itIsVisible;
		
		boost::shared_ptr< Ped::UserWindow >  itsWindow;
		
		boost::weak_ptr< IOHandle >  itsTerminal;
		
		WindowParameters() : itsOrigin( gZeroPoint ),
		                     itsSize  ( gZeroPoint ),
		                     itIsVisible( true )
		{
		}
	};
	
	typedef std::map< const FSTree*, WindowParameters > WindowParametersMap;
	
	static WindowParametersMap gWindowParametersMap;
	
	
	static void CenterWindowRect( Rect& bounds )
	{
		// Pre-conditions:  bounds is set to { 0, 0, v, h }
		
		const short topMargin = ::GetMBarHeight() + 18;  // FIXME:  Calculate title bar height
		
		BitMap screenBits = N::GetQDGlobalsScreenBits();
		
		short spareWidth = screenBits.bounds.right - bounds.right;
		
		short spareHeight = screenBits.bounds.bottom - bounds.bottom - topMargin;
		
		::OffsetRect( &bounds,
		              spareWidth / 2,
		              topMargin + spareHeight / 3 );
	}
	
	static void CloseUserWindow( const FSTree* key )
	{
		WindowParametersMap::iterator it = gWindowParametersMap.find( key );
		
		if ( it != gWindowParametersMap.end() )
		{
			WindowParameters& params = it->second;
			
			if ( params.itsTerminal.expired() )
			{
				// tty file is not open for this window, just close the window
				params.itsWindow.reset();
			}
			else
			{
				const boost::shared_ptr< IOHandle >& handle = params.itsTerminal.lock();
				
				TerminalHandle& terminal( IOHandle_Cast< TerminalHandle >( *handle ) );
				
				terminal.Disconnect();
			}
		}
	}
	
	class UserWindowCloseHandler : public Ped::WindowCloseHandler
	{
		private:
			const FSTree* itsKey;
		
		public:
			UserWindowCloseHandler( const FSTree* key ) : itsKey( key )
			{
			}
			
			void operator()( N::WindowRef ) const  { CloseUserWindow( itsKey ); }
	};
	
	
	static bool HasWindow( const FSTree* that )
	{
		const FSTree* key = that->ParentRef().get();
		
		return gWindowParametersMap[ key ].itsWindow != NULL;
	}
	
	static void CreateUserWindow( const FSTree* key, std::auto_ptr< Ped::View > view )
	{
		WindowParametersMap::const_iterator it = gWindowParametersMap.find( key );
		
		if ( it == gWindowParametersMap.end() )
		{
			p7::throw_errno( EPERM );
		}
		
		const WindowParameters& params = it->second;
		
		ConstStr255Param title = params.itsTitle;
		
		Rect bounds = { 0, 0, 90, 120 };
		
		if ( params.itsSize.h || params.itsSize.v )
		{
			reinterpret_cast< Point* >( &bounds )[1] = params.itsSize;
		}
		
		if ( params.itsOrigin.h || params.itsOrigin.v )
		{
			::OffsetRect( &bounds, params.itsOrigin.h, params.itsOrigin.v );
		}
		else
		{
			CenterWindowRect( bounds );
		}
		
		Ped::NewWindowContext context( bounds, title, params.itIsVisible );
		
		boost::shared_ptr< Ped::UserWindow > window( new Ped::UserWindow( context, N::documentProc ) );
		
		boost::shared_ptr< Ped::WindowCloseHandler > closeHandler( new UserWindowCloseHandler( key ) );
		
		window->SetCloseHandler( closeHandler );
		
		window->SetView( view );
		
		gWindowParametersMap[ key ].itsWindow = window;
	}
	
	void RemoveUserWindow( const FSTree* key )
	{
		CloseUserWindow( key );
		
		gWindowParametersMap.erase( key );
		
		RemoveAllViewParameters( key );
	}
	
	
	static inline bool is_integer( const char* s )
	{
		return std::isdigit( s[ s[0] == '-' ] );
	}
	
	static Point ReadPoint( const char* string )
	{
		const char* p = string;
		
		long x = std::strtol( p, (char**) &p, 10 );
		
		if ( p != string  &&  *p != '\0' )
		{
			while ( *++p )
			{
				if ( is_integer( p ) )
				{
					long y = std::strtol( p, NULL, 10 );
					
					Point result = { y, x };
					
					return result;
				}
			}
		}
		
		throw p7::errno_t( EINVAL );
	}
	
	static std::string WritePoint( Point point, const char* separator )
	{
		std::string result = NN::Convert< std::string >( point.h );
		
		result += separator;
		
		result += NN::Convert< std::string >( point.v );
		
		return result;
	}
	
	
	struct Access_Title
	{
		typedef N::Str255 Value;
		
		static Value const& GetRef( WindowParameters const& params )  { return params.itsTitle; }
		static Value      & GetRef( WindowParameters      & params )  { return params.itsTitle; }
		
		static std::string StringFromValue( const Value& v )
		{
			return NN::Convert< std::string >( v );
		}
		
		static Value ValueFromString( const char* begin, const char* end )
		{
			return N::Str255( begin, end - begin );
		}
	};
	
	struct Access_Origin
	{
		typedef Point Value;
		
		static Value const& GetRef( WindowParameters const& params )  { return params.itsOrigin; }
		static Value      & GetRef( WindowParameters      & params )  { return params.itsOrigin; }
		
		static std::string StringFromValue( const Value& origin )
		{
			return WritePoint( origin, "," );
		}
		
		static Value ValueFromString( const char* begin, const char* end )
		{
			return ReadPoint( begin );
		}
	};
	
	struct Access_Size
	{
		typedef Point Value;
		
		static Value const& GetRef( WindowParameters const& params )  { return params.itsSize; }
		static Value      & GetRef( WindowParameters      & params )  { return params.itsSize; }
		
		static std::string StringFromValue( const Value& size )
		{
			return WritePoint( size, "x" );
		}
		
		static Value ValueFromString( const char* begin, const char* end )
		{
			return ReadPoint( begin );
		}
	};
	
	struct Access_Visible
	{
		typedef bool Value;
		
		static Value const& GetRef( WindowParameters const& params )  { return params.itIsVisible; }
		static Value      & GetRef( WindowParameters      & params )  { return params.itIsVisible; }
		
		static std::string StringFromValue( const Value& vis )
		{
			return vis ? "1" : "0";
		}
		
		static Value ValueFromString( const char* begin, const char* end )
		{
			return begin[ 0 ] != '0';
		}
	};
	
	
	template < class Accessor >
	struct sys_window_REF_Property
	{
		static std::string Read( const FSTree* that )
		{
			const FSTree* key = GetViewKey( that );
			
			WindowParametersMap::const_iterator it = gWindowParametersMap.find( key );
			
			if ( it == gWindowParametersMap.end() )
			{
				throw FSTree_Property::Undefined();
			}
			
			return Accessor::StringFromValue( Accessor::GetRef( it->second ) );
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end )
		{
			const FSTree* key = GetViewKey( that );
			
			Accessor::GetRef( gWindowParametersMap[ key ] ) = Accessor::ValueFromString( begin, end );
		}
	};
	
	
	class FSTree_sys_window_REF_Property : public FSTree_Property
	{
		public:
			FSTree_sys_window_REF_Property( const FSTreePtr&    parent,
			                                const std::string&  name,
			                                ReadHook            readHook,
			                                WriteHook           writeHook )
			:
				FSTree_Property( parent,
				                 name,
				                 readHook,
				                 writeHook )
			{
			}
			
			bool IsLink() const  { return HasWindow( this ); }
			
			mode_t FilePermMode() const;
			
			std::string ReadLink() const;
			
			FSTreePtr ResolveLink() const;
	};
	
	mode_t FSTree_sys_window_REF_Property::FilePermMode() const
	{
		return IsLink() ? S_IRUSR | S_IWUSR | S_IXUSR : FSTree_Property::FilePermMode();
	}
	
	std::string FSTree_sys_window_REF_Property::ReadLink() const
	{
		if ( !IsLink() )
		{
			p7::throw_errno( EINVAL );
		}
		
		return "ref/" + Name();
	}
	
	FSTreePtr FSTree_sys_window_REF_Property::ResolveLink() const
	{
		return ResolvePathname( ReadLink(), ParentRef() );
	}
	
	
	static void InvalidateWindowRef( N::WindowRef window )
	{
		ASSERT( window != NULL );
		
		if ( ::IsPortVisibleRegionEmpty( N::GetWindowPort( window ) ) )
		{
			return;
		}
		
		NN::Saved< N::Port_Value > savePort;
		
		N::SetPortWindowPort( window );
		
		N::InvalRect( N::GetPortBounds( N::GetWindowPort( window ) ) );
	}
	
	static N::WindowRef GetWindowRef( const FSTree* key )
	{
		WindowParametersMap::const_iterator it = gWindowParametersMap.find( key );
		
		if ( it != gWindowParametersMap.end() )
		{
			const boost::shared_ptr< Ped::UserWindow >& window = it->second.itsWindow;
			
			if ( window.get() != NULL )
			{
				return window->Get();
			}
		}
		
		return NULL;
	}
	
	bool InvalidateWindow( const FSTree* key )
	{
		if ( N::WindowRef window = GetWindowRef( key ) )
		{
			InvalidateWindowRef( window );
			
			return true;
		}
		
		return false;
	}
	
	
	class FSTree_sys_window_REF_ref : public FSTree
	{
		public:
			FSTree_sys_window_REF_ref( const FSTreePtr&    parent,
			                           const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			const FSTree* WindowKey() const  { return ParentRef().get(); }
			
			bool Exists() const  { return HasWindow( this ); }
			
			bool IsLink() const  { return Exists(); }
			
			void SetTimes() const;
			
			void Delete() const;
			
			std::string ReadLink() const;
			
			FSTreePtr ResolveLink() const;
	};
	
	
	void FSTree_sys_window_REF_ref::SetTimes() const
	{
		const FSTree* key = WindowKey();
		
		std::auto_ptr< Ped::View > view = MakeView( key, "view" );
		
		if ( view.get() == NULL )
		{
			view.reset( new Ped::EmptyView );
		}
		
		CreateUserWindow( key, view );
	}
	
	void FSTree_sys_window_REF_ref::Delete() const
	{
		const FSTree* key = WindowKey();
		
		CloseUserWindow( key );
	}
	
	std::string FSTree_sys_window_REF_ref::ReadLink() const
	{
		N::WindowRef windowPtr = GetWindowRef( WindowKey() );
		
		if ( windowPtr == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		std::string result = "/sys/app/window/";
		
		result += Pointer_KeyName_Traits< ::WindowRef >::NameFromKey( windowPtr );
		
		return result;
	}
	
	FSTreePtr FSTree_sys_window_REF_ref::ResolveLink() const
	{
		return ResolvePathname( ReadLink() );
	}
	
	
	class FSTree_sys_window_REF_view : public FSTree_View
	{
		public:
			FSTree_sys_window_REF_view( const FSTreePtr&    parent,
			                            const std::string&  name ) : FSTree_View( parent, name )
			{
			}
			
			void SetView( std::auto_ptr< Ped::View > view ) const;
			
			void AddCustomParameters( std::auto_ptr< Ped::View > view ) const
			{
				SetView( view );
			}
			
			void DeleteCustomParameters() const;
	};
	
	void FSTree_sys_window_REF_view::SetView( std::auto_ptr< Ped::View > view ) const
	{
		const FSTree* parent = ParentKey();
		
		if ( Ped::UserWindow* window = gWindowParametersMap[ parent ].itsWindow.get() )
		{
			window->SetView( view );
		}
	}
	
	void FSTree_sys_window_REF_view::DeleteCustomParameters() const
	{
		SetView( std::auto_ptr< Ped::View >( new Ped::EmptyView ) );
	}
	
	
	class FSTree_sys_window_REF_tty : public FSTree
	{
		public:
			FSTree_sys_window_REF_tty( const FSTreePtr&    parent,
			                           const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			const FSTree* WindowKey() const  { return ParentRef().get(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	boost::shared_ptr< IOHandle >
	//
	FSTree_sys_window_REF_tty::Open( OpenFlags flags ) const
	{
		boost::shared_ptr< IOHandle > result( new TerminalHandle( Pathname() ) );
		
		gWindowParametersMap[ WindowKey() ].itsTerminal = result;
		
		return result;
	}
	
	
	template < class FSTree_Type >
	static FSTreePtr Factory( const FSTreePtr&    parent,
	                          const std::string&  name )
	{
		return FSTreePtr( new FSTree_Type( parent, name ) );
	}
	
	template < class Accessor >
	static FSTreePtr PropertyFactory( const FSTreePtr&    parent,
	                                  const std::string&  name )
	{
		typedef sys_window_REF_Property< Accessor > Property;
		
		return FSTreePtr( new FSTree_sys_window_REF_Property( parent,
		                                                      name,
		                                                      &Property::Read,
		                                                      &Property::Write ) );
	}
	
	const Functional_Traits< void >::Mapping sys_window_REF_Mappings[] =
	{
		{ "ref",   &Factory< FSTree_sys_window_REF_ref >, true },
		
		{ "view",  &Factory< FSTree_sys_window_REF_view >, true },
		
		{ "tty",   &Factory< FSTree_sys_window_REF_tty > },
		
		{ "title", &PropertyFactory< Access_Title   > },
		{ "pos",   &PropertyFactory< Access_Origin  > },
		{ "size",  &PropertyFactory< Access_Size    > },
		{ "vis",   &PropertyFactory< Access_Visible > },
		
		{ NULL, NULL }
	};
	
}

