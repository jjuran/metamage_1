/*	========================
 *	FSTree_sys_window_REF.cc
 *	========================
 */

#include "Genie/FileSystem/FSTree_sys_window_REF.hh"

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/UserWindow.hh"

// Genie
#include "Genie/FileSystem/FSTree_PseudoFile.hh"
#include "Genie/FileSystem/ResolvePathname.hh"


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
	
	class EmptyView : public Ped::View
	{
		public:
			void Update();
	};
	
	void EmptyView::Update()
	{
		N::EraseRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
	}
	
	static void CreateUserWindow( const FSTree* key )
	{
		WindowTitleMap::const_iterator the_title = gWindowTitleMap.find( key );
		WindowSizeMap ::const_iterator the_size  = gWindowSizeMap .find( key );
		
		if ( the_title == gWindowTitleMap.end()  ||  the_size == gWindowSizeMap.end() )
		{
			return;
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
		
		gWindowMap[ key ] = window;
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
	
	
	template < class Property >
	class FSTree_sys_window_REF_Property : public FSTree
	{
		private:
			Property itsProperty;
		
		public:
			FSTree_sys_window_REF_Property( const FSTreePtr&    parent,
			                                const std::string&  name ) : FSTree( parent, name ),
			                                                             itsProperty( parent.get() )
			{
			}
			
			bool IsLink() const  { return FindWindow( this ) != gWindowMap.end(); }
			
			mode_t FilePermMode() const;
			
			off_t GetEOF() const  { return itsProperty.Get().size(); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
			
			std::string ReadLink() const;
			
			FSTreePtr ResolveLink() const;
	};
	
	template < class Property >
	mode_t FSTree_sys_window_REF_Property< Property >::FilePermMode() const
	{
		return IsLink() ? S_IRUSR | S_IWUSR | S_IXUSR : S_IRUSR | S_IWUSR;
	}
	
	template < class Property >
	boost::shared_ptr< IOHandle > FSTree_sys_window_REF_Property< Property >::Open( OpenFlags flags ) const
	{
		IOHandle* result = NULL;
		
		if ( flags == O_RDONLY )
		{
			result = new QueryFileHandle( shared_from_this(), itsProperty.Get() );
		}
		else if ( (flags & ~O_CREAT) == (O_WRONLY | O_TRUNC) )
		{
			result = new PseudoFileHandle< Property >( shared_from_this(), itsProperty );
		}
		else
		{
			throw poseven::errno_t( EINVAL );
		}
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	template < class Property >
	std::string FSTree_sys_window_REF_Property< Property >::ReadLink() const
	{
		if ( !IsLink() )
		{
			p7::throw_errno( EINVAL );
		}
		
		return "ref/" + Name();
	}
	
	template < class Property >
	FSTreePtr FSTree_sys_window_REF_Property< Property >::ResolveLink() const
	{
		return ResolvePathname( ReadLink(), Parent() );
	}
	
	
	class FSTree_sys_window_REF_ref : public FSTree
	{
		public:
			FSTree_sys_window_REF_ref( const FSTreePtr&    parent,
			                           const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			bool Exists() const;
			
			bool IsLink() const  { return FindWindow( this ) != gWindowMap.end(); }
			
			void SetTimes() const;
			
			void Delete() const;
			
			std::string ReadLink() const;
			
			FSTreePtr ResolveLink() const;
	};
	
	
	bool FSTree_sys_window_REF_ref::Exists() const
	{
		return FindWindow( this ) != gWindowMap.end();
	}
	
	void FSTree_sys_window_REF_ref::SetTimes() const
	{
		const FSTree* key = Parent().get();
		
		CreateUserWindow( key );
	}
	
	void FSTree_sys_window_REF_ref::Delete() const
	{
		const FSTree* key = Parent().get();
		
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
	
	
	template < class FSTree_Type >
	static FSTreePtr Factory( const FSTreePtr&    parent,
	                          const std::string&  name )
	{
		return MakeFSTree( new FSTree_Type( parent, name ) );
	}
	
	const Functional_Traits< void >::Mapping sys_window_REF_Mappings[] =
	{
		{ "ref",   &Factory< FSTree_sys_window_REF_ref >, true },
		
		{ "title", &Factory< FSTree_sys_window_REF_Property< sys_window_REF_Property< Access_Title > > > },
		{ "size",  &Factory< FSTree_sys_window_REF_Property< sys_window_REF_Property< Access_Size  > > > },
		
		{ NULL, NULL }
	};
	
}

