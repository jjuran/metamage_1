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
#include "Genie/FileSystem/FSTree_PseudoFile.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/FileSystem/Views.hh"


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
			it->second.itsWindow.reset();
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
		const FSTree* key = that->Parent().get();
		
		return gWindowParametersMap[ key ].itsWindow != NULL;
	}
	
	static const boost::shared_ptr< Ped::UserWindow >& CreateUserWindow( const FSTree* key )
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
		
		window->SetView( std::auto_ptr< Ped::View >( new Ped::EmptyView() ) );
		
		return gWindowParametersMap[ key ].itsWindow = window;
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
	
	static Point ReadPoint( const std::string& value )
	{
		const char* p = value.c_str();
		
		long x = std::strtol( p, (char**) &p, 10 );
		
		if ( p != value.c_str()  &&  *p != '\0' )
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
		
		static Value ValueFromString( const std::string& s )
		{
			return N::Str255( s );
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
		
		static Value ValueFromString( const std::string& s )
		{
			return ReadPoint( s );
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
		
		static Value ValueFromString( const std::string& s )
		{
			return ReadPoint( s );
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
		
		static Value ValueFromString( const std::string& s )
		{
			return s.c_str()[ 0 ] != '0';
		}
	};
	
	
	template < class Accessor >
	class sys_window_REF_Property
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			sys_window_REF_Property( const Key& key ) : itsKey( key )
			{
			}
			
			std::string Get() const
			{
				WindowParametersMap::const_iterator it = gWindowParametersMap.find( itsKey );
				
				if ( it == gWindowParametersMap.end() )
				{
					return "";  // empty file
				}
				
				std::string output = Accessor::StringFromValue( Accessor::GetRef( it->second ) );
				
				output += "\n";
				
				return output;
			}
			
			void Set( const std::string& s )
			{
				Accessor::GetRef( gWindowParametersMap[ itsKey ] ) = Accessor::ValueFromString( s );
			}
	};
	
	
	class FSTree_sys_window_REF_Property_Base : public FSTree
	{
		public:
			FSTree_sys_window_REF_Property_Base( const FSTreePtr&    parent,
			                                     const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			bool IsLink() const  { return HasWindow( this ); }
			
			mode_t FilePermMode() const;
			
			std::string ReadLink() const;
			
			FSTreePtr ResolveLink() const;
	};
	
	mode_t FSTree_sys_window_REF_Property_Base::FilePermMode() const
	{
		return IsLink() ? S_IRUSR | S_IWUSR | S_IXUSR : S_IRUSR | S_IWUSR;
	}
	
	std::string FSTree_sys_window_REF_Property_Base::ReadLink() const
	{
		if ( !IsLink() )
		{
			p7::throw_errno( EINVAL );
		}
		
		return "ref/" + Name();
	}
	
	FSTreePtr FSTree_sys_window_REF_Property_Base::ResolveLink() const
	{
		return ResolvePathname( ReadLink(), Parent() );
	}
	
	
	template < class Property >
	class FSTree_sys_window_REF_Property : public FSTree_sys_window_REF_Property_Base
	{
		private:
			typedef FSTree_sys_window_REF_Property_Base Base;
			
			Property itsProperty;
		
		public:
			FSTree_sys_window_REF_Property( const FSTreePtr&    parent,
			                                const std::string&  name ) : Base( parent, name ),
			                                                             itsProperty( parent.get() )
			{
			}
			
			off_t GetEOF() const  { return itsProperty.Get().size(); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	template < class Property >
	boost::shared_ptr< IOHandle > FSTree_sys_window_REF_Property< Property >::Open( OpenFlags flags ) const
	{
		IOHandle* result = NULL;
		
		if ( flags == O_RDONLY )
		{
			result = new QueryFileHandle( shared_from_this(),
			                              flags,
			                              itsProperty.Get() );
		}
		else if ( (flags & ~O_CREAT) == (O_WRONLY | O_TRUNC) )
		{
			result = new PseudoFileHandle< Property >( shared_from_this(),
			                                           flags,
			                                           itsProperty );
		}
		else
		{
			throw poseven::errno_t( EINVAL );
		}
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	
	static void ConstructViewInWindow( const ViewFactory& factory, Ped::UserWindow& window )
	{
		N::WindowRef windowRef = window.Get();
		
		NN::Saved< N::Port_Value > savePort;
		
		N::SetPortWindowPort( windowRef );
		
		Rect bounds = N::GetPortBounds( N::GetWindowPort( windowRef ) );
		
		window.SetView( factory( bounds ) );
		
		N::InvalRect( bounds );
	}
	
	static void InvalidateCurrentWindow()
	{
		N::InvalRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
	}
	
	static void InvalidateWindowRef( N::WindowRef window )
	{
		NN::Saved< N::Port_Value > savePort;
		
		N::SetPortWindowPort( window );
		
		InvalidateCurrentWindow();
	}
	
	N::WindowRef GetWindowRef( const FSTree* key )
	{
		WindowParametersMap::const_iterator it = gWindowParametersMap.find( key );
		
		return it != gWindowParametersMap.end() ? it->second.itsWindow->Get() : NULL;
	}
	
	static void InvalidateWindow( const FSTree* key )
	{
		if ( N::WindowRef window = GetWindowRef( key ) )
		{
			InvalidateWindowRef( window );
		}
		else
		{
			p7::throw_errno( ENOENT );
		}
	}
	
	static void DestroyViewInWindow( Ped::UserWindow& window )
	{
		N::WindowRef windowRef = window.Get();
		
		NN::Saved< N::Port_Value > savePort;
		
		N::SetPortWindowPort( windowRef );
		
		window.SetView( std::auto_ptr< Ped::View >( new Ped::EmptyView() ) );
		
		InvalidateCurrentWindow();
	}
	
	
	class FSTree_sys_window_REF_ref : public FSTree
	{
		public:
			FSTree_sys_window_REF_ref( const FSTreePtr&    parent,
			                           const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			const FSTree* WindowKey() const  { return Parent().get(); }
			
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
		
		const boost::shared_ptr< Ped::UserWindow >& window = CreateUserWindow( key );
		
		if ( const boost::shared_ptr< ViewFactory >& factory = GetViewFactory( key, "view" ) )
		{
			ConstructViewInWindow( *factory, *window );
		}
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
	
	
	class FSTree_sys_window_REF_view : public FSTree
	{
		public:
			FSTree_sys_window_REF_view( const FSTreePtr&    parent,
			                            const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			const FSTree* WindowKey() const  { return Parent().get(); }
			
			bool IsDirectory() const  { return Exists(); }
			
			bool Exists() const  { return GetViewDelegate( WindowKey(), Name() ) != NULL; }
			
			void SetTimes() const;
			
			void Delete() const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup( const std::string& name ) const;
			
			FSIteratorPtr Iterate() const;
	};
	
	void FSTree_sys_window_REF_view::SetTimes() const
	{
		InvalidateWindow( WindowKey() );
	}
	
	void FSTree_sys_window_REF_view::Delete() const
	{
		const FSTree* key = WindowKey();
		
		const std::string& name = Name();
		
		if ( ViewExists( key, name ) )
		{
			RemoveViewParameters( key, name );
			
			if ( Ped::UserWindow* window = gWindowParametersMap[ key ].itsWindow.get() )
			{
				DestroyViewInWindow( *window );
			}
		}
		else
		{
			p7::throw_errno( ENOENT );
		}
	}
	
	void FSTree_sys_window_REF_view::CreateDirectory( mode_t mode ) const
	{
		const FSTree* key = WindowKey();
		
		const std::string& name = Name();
		
		if ( const boost::shared_ptr< ViewFactory >& factory = GetViewFactory( key, name ) )
		{
			AddViewWindowKey( key, name, key );
			
			WindowParametersMap::const_iterator it = gWindowParametersMap.find( key );
			
			if ( Ped::UserWindow* window = gWindowParametersMap[ key ].itsWindow.get() )
			{
				ConstructViewInWindow( *factory, *window );
			}
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
	FSTreePtr FSTree_sys_window_REF_view::Lookup( const std::string& name ) const
	{
		const FSTreePtr& delegate = GetViewDelegate( WindowKey(), Name() );
		
		if ( delegate == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return delegate->Lookup( name );
	}
	
	FSIteratorPtr FSTree_sys_window_REF_view::Iterate() const
	{
		const FSTreePtr& delegate = GetViewDelegate( WindowKey(), Name() );
		
		if ( delegate == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return delegate->Iterate();
	}
	
	
	template < class FSTree_Type >
	static FSTreePtr Factory( const FSTreePtr&    parent,
	                          const std::string&  name )
	{
		return FSTreePtr( new FSTree_Type( parent, name ) );
	}
	
	const Functional_Traits< void >::Mapping sys_window_REF_Mappings[] =
	{
		{ "ref",   &Factory< FSTree_sys_window_REF_ref >, true },
		
		{ "view",   &Factory< FSTree_sys_window_REF_view >, true },
		
		{ "title", &Factory< FSTree_sys_window_REF_Property< sys_window_REF_Property< Access_Title   > > > },
		{ "pos",   &Factory< FSTree_sys_window_REF_Property< sys_window_REF_Property< Access_Origin  > > > },
		{ "size",  &Factory< FSTree_sys_window_REF_Property< sys_window_REF_Property< Access_Size    > > > },
		{ "vis",   &Factory< FSTree_sys_window_REF_Property< sys_window_REF_Property< Access_Visible > > > },
		
		{ NULL, NULL }
	};
	
}

