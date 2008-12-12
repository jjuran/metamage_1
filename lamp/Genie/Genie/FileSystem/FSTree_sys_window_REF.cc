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
	
	
	typedef std::map< const FSTree*, N::Str255 > WindowTitleMap;
	
	typedef std::map< const FSTree*, Point > WindowSizeMap;
	
	typedef std::map< const FSTree*, boost::shared_ptr< Ped::UserWindow > > WindowMap;
	
	static WindowTitleMap  gWindowTitleMap;
	static WindowSizeMap   gWindowSizeMap;
	
	static WindowMap gWindowMap;
	
	
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
		gWindowMap.erase( key );
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
	
	
	static WindowMap::const_iterator FindWindow( const FSTree* that )
	{
		const FSTree* key = that->Parent().get();
		
		return gWindowMap.find( key );
	}
	
	static inline bool HasWindow( const FSTree* that )
	{
		return FindWindow( that ) != gWindowMap.end();
	}
	
	class EmptyView : public Ped::View
	{
		public:
			void Update();
	};
	
	void EmptyView::Update()
	{
		N::EraseRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
	}
	
	static const boost::shared_ptr< Ped::UserWindow >& CreateUserWindow( const FSTree* key )
	{
		WindowTitleMap::const_iterator the_title = gWindowTitleMap.find( key );
		WindowSizeMap ::const_iterator the_size  = gWindowSizeMap .find( key );
		
		if ( the_title == gWindowTitleMap.end()  ||  the_size == gWindowSizeMap.end() )
		{
			p7::throw_errno( EPERM );
		}
		
		ConstStr255Param title = the_title->second;
		
		Rect bounds = { 0 };
		
		reinterpret_cast< Point* >( &bounds )[1] = the_size->second;
		
		CenterWindowRect( bounds );
		
		Ped::NewWindowContext context( bounds, title );
		
		boost::shared_ptr< Ped::UserWindow > window( new Ped::UserWindow( context, N::documentProc ) );
		
		boost::shared_ptr< Ped::WindowCloseHandler > closeHandler( new UserWindowCloseHandler( key ) );
		
		window->SetCloseHandler( closeHandler );
		
		window->SetView( std::auto_ptr< Ped::View >( new EmptyView() ) );
		
		return gWindowMap[ key ] = window;
	}
	
	void RemoveUserWindow( const FSTree* key )
	{
		CloseUserWindow( key );
		
		gWindowTitleMap.erase( key );
		gWindowSizeMap .erase( key );
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
		
		typedef std::map< const FSTree*, Value > Map;
		
		static Map& GetMap()  { return gWindowTitleMap; }
		
		static std::string StringFromValue( const Value& v )
		{
			return NN::Convert< std::string >( v );
		}
		
		static Value ValueFromString( const std::string& s )
		{
			return N::Str255( s );
		}
	};
	
	struct Access_Size
	{
		typedef Point Value;
		
		typedef std::map< const FSTree*, Value > Map;
		
		static Map& GetMap()  { return gWindowSizeMap; }
		
		static std::string StringFromValue( const Value& size )
		{
			return WritePoint( size, "x" );
		}
		
		static Value ValueFromString( const std::string& s )
		{
			return ReadPoint( s );
		}
	};
	
	
	template < class Accessor >
	class sys_window_REF_Property
	{
		private:
			typedef typename Accessor::Map Map;
			
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			sys_window_REF_Property( const Key& key ) : itsKey( key )
			{
			}
			
			std::string Get() const
			{
				const Map& map = Accessor::GetMap();
				
				Map::const_iterator it = map.find( itsKey );
				
				if ( it == map.end() )
				{
					return "";  // empty file
				}
				
				std::string output = Accessor::StringFromValue( it->second );
				
				output += "\n";
				
				return output;
			}
			
			void Set( const std::string& s )
			{
				Accessor::GetMap()[ itsKey ] = Accessor::ValueFromString( s );
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
		
		factory( window );
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
		WindowMap::const_iterator it = gWindowMap.find( key );
		
		return it != gWindowMap.end() ? it->second->Get() : NULL;
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
	
	static void DestroyViewInWindow( const ViewFactory& factory, Ped::UserWindow& window )
	{
		N::WindowRef windowRef = window.Get();
		
		NN::Saved< N::Port_Value > savePort;
		
		N::SetPortWindowPort( windowRef );
		
		window.SetView( std::auto_ptr< Ped::View >( new EmptyView() ) );
		
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
		
		if ( const boost::shared_ptr< ViewFactory >& factory = GetViewFactory( key ) )
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
		WindowMap::const_iterator it = FindWindow( this );
		
		if ( it == gWindowMap.end() )
		{
			p7::throw_errno( EINVAL );
		}
		
		N::WindowRef windowPtr = it->second->Get();
		
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
			
			bool Exists() const  { return GetViewDelegate( WindowKey() ) != NULL; }
			
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
		if ( const boost::shared_ptr< ViewFactory >& factory = GetViewFactory( WindowKey() ) )
		{
			const FSTree* key = WindowKey();
			
			RemoveViewFactory ( key );
			RemoveViewDelegate( key );
			
			WindowMap::const_iterator it = FindWindow( this );
			
			if ( it != gWindowMap.end() )
			{
				const boost::shared_ptr< Ped::UserWindow >& window = it->second;
				
				DestroyViewInWindow( *factory, *window );
			}
		}
		else
		{
			p7::throw_errno( ENOENT );
		}
	}
	
	void FSTree_sys_window_REF_view::CreateDirectory( mode_t mode ) const
	{
		if ( const boost::shared_ptr< ViewFactory >& factory = GetViewFactory( WindowKey() ) )
		{
			WindowMap::const_iterator it = FindWindow( this );
			
			if ( it != gWindowMap.end() )
			{
				const boost::shared_ptr< Ped::UserWindow >& window = it->second;
				
				ConstructViewInWindow( *factory, *window );
			}
		}
		else
		{
			p7::throw_errno( ENOENT );
		}
	}
	
	FSTreePtr FSTree_sys_window_REF_view::Lookup( const std::string& name ) const
	{
		const FSTreePtr& delegate = GetViewDelegate( WindowKey() );
		
		if ( delegate == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return delegate->Lookup( name );
	}
	
	FSIteratorPtr FSTree_sys_window_REF_view::Iterate() const
	{
		const FSTreePtr& delegate = GetViewDelegate( WindowKey() );
		
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
		return MakeFSTree( new FSTree_Type( parent, name ) );
	}
	
	const Functional_Traits< void >::Mapping sys_window_REF_Mappings[] =
	{
		{ "ref",   &Factory< FSTree_sys_window_REF_ref >, true },
		
		{ "view",   &Factory< FSTree_sys_window_REF_view >, true },
		
		{ "title", &Factory< FSTree_sys_window_REF_Property< sys_window_REF_Property< Access_Title > > > },
		{ "size",  &Factory< FSTree_sys_window_REF_Property< sys_window_REF_Property< Access_Size  > > > },
		
		{ NULL, NULL }
	};
	
}

