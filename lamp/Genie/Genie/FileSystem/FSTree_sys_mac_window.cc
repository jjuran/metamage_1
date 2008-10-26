/*	========================
 *	FSTree_sys_mac_window.cc
 *	========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_window.hh"

// Standard C++
#include <algorithm>

// Nucleus
#include "Nucleus/Convert.h"

// Pedestal
#include "Pedestal/Window.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_PseudoFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	bool sys_mac_window_Details::KeyIsValid( const Key& key )
	{
		Sequence sequence = ItemSequence();
		
		return std::find( sequence.begin(), sequence.end(), key ) != sequence.end();
	}
	
	extern const Functional_Traits< WindowRef_KeyName_Traits::Key >::Mapping sys_mac_window_REF_Mappings[];
	
	FSTreePtr sys_mac_window_Details::GetChildNode( const FSTreePtr&    parent,
		                                            const std::string&  name,
		                                            const Key&          key )
	{
		return Premapped_Factory< Key, sys_mac_window_REF_Mappings >( parent, name, key );
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
	
	struct Access_WindowTitle
	{
		typedef N::Str255 Result;
		
		Result Get( N::WindowRef window ) const
		{
			return N::GetWTitle( window );
		}
		
		void Set( N::WindowRef window, const std::string& value )
		{
			N::SetWTitle( window, N::Str255( value ) );
		}
	};
	
	struct Access_WindowPosition
	{
		typedef std::string Result;
		
		Result Get( N::WindowRef window ) const
		{
			Point position = Ped::GetWindowPosition( window );
			
			return WritePoint( position, "," );
		}
		
		void Set( N::WindowRef window, const std::string& value )
		{
			Ped::SetWindowPosition( window, ReadPoint( value ) );
		}
	};
	
	struct Access_WindowSize
	{
		typedef std::string Result;
		
		Result Get( N::WindowRef window ) const
		{
			Point size = Ped::GetWindowSize( window );
			
			return WritePoint( size, "x" );
		}
		
		void Set( N::WindowRef window, const std::string& value )
		{
			Ped::SetWindowSize( window, ReadPoint( value ) );
		}
	};
	
	template < class Accessor >
	class sys_mac_window_REF_Property
	{
		private:
			typedef N::WindowRef Key;
			
			Key itsKey;
		
		public:
			sys_mac_window_REF_Property( const Key& key ) : itsKey( key )
			{
			}
			
			std::string Get() const
			{
				std::string output = NN::Convert< std::string >( Accessor().Get( itsKey ) ) + "\n";
				
				return output;
			}
			
			void Set( const std::string& value )
			{
				Accessor().Set( itsKey, value );
			}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&                parent,
	                                   const std::string&              name,
	                                   WindowRef_KeyName_Traits::Key   key )
	{
		typedef sys_mac_window_REF_Property< Accessor > Property;
		
		typedef FSTree_PseudoFile< Property > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name, Property( key ) ) );
	}
	
	const Functional_Traits< WindowRef_KeyName_Traits::Key >::Mapping sys_mac_window_REF_Mappings[] =
	{
		{ "title", &Property_Factory< Access_WindowTitle    > },
		{ "pos",   &Property_Factory< Access_WindowPosition > },
		{ "size",  &Property_Factory< Access_WindowSize     > },
		
		{ NULL, NULL }
	};
	
}

