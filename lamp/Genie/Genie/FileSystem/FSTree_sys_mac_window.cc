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
	
	struct Property_WindowName
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
	
	struct Property_WindowPosition
	{
		typedef std::string Result;
		
		Result Get( N::WindowRef window ) const
		{
			Point position = Ped::GetWindowPosition( window );
			
			std::string result = NN::Convert< std::string >( position.h );
			
			result += ",";
			
			result += NN::Convert< std::string >( position.v );
			
			return result;
		}
		
		void Set( N::WindowRef window, const std::string& value )
		{
			Ped::SetWindowPosition( window, ReadPoint( value ) );
		}
	};
	
	struct Property_WindowSize
	{
		typedef std::string Result;
		
		Result Get( N::WindowRef window ) const
		{
			Point size = Ped::GetWindowSize( window );
			
			std::string result = NN::Convert< std::string >( size.h );
			
			result += "x";
			
			result += NN::Convert< std::string >( size.v );
			
			return result;
		}
		
		void Set( N::WindowRef window, const std::string& value )
		{
			Ped::SetWindowSize( window, ReadPoint( value ) );
		}
	};
	
	template < class Property >
	class sys_mac_window_REF_Query
	{
		private:
			typedef N::WindowRef Key;
			
			Key itsKey;
		
		public:
			sys_mac_window_REF_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string Get() const
			{
				std::string output = NN::Convert< std::string >( Property().Get( itsKey ) ) + "\n";
				
				return output;
			}
			
			void Set( const std::string& value )
			{
				Property().Set( itsKey, value );
			}
	};
	
	template < class Get >
	static FSTreePtr Query_Factory( const FSTreePtr&                parent,
	                                const std::string&              name,
	                                WindowRef_KeyName_Traits::Key   key )
	{
		typedef sys_mac_window_REF_Query< Get > Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name, Query( key ) ) );
	}
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&                parent,
	                                   const std::string&              name,
	                                   WindowRef_KeyName_Traits::Key   key )
	{
		typedef sys_mac_window_REF_Query< Property > Query;
		
		typedef FSTree_PseudoFile< Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name, Query( key ) ) );
	}
	
	const Functional_Traits< WindowRef_KeyName_Traits::Key >::Mapping sys_mac_window_REF_Mappings[] =
	{
		{ "name", &Property_Factory< Property_WindowName     > },
		{ "pos",  &Property_Factory< Property_WindowPosition > },
		{ "size", &Property_Factory< Property_WindowSize     > },
		
		{ NULL, NULL }
	};
	
}

