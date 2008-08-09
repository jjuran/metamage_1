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


namespace Nucleus
{
	
	// Bust partial specialization ambiguity
	template <>
	struct Converter< std::string, std::string >: public std::unary_function< std::string, std::string >
	{
		const std::string& operator()( const std::string& input ) const
		{
			return input;
		}
	};
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
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
	
	
	struct GetWindowName
	{
		typedef N::Str255 Result;
		
		Result operator()( N::WindowRef window ) const
		{
			return N::GetWTitle( window );
		}
	};
	
	struct GetWindowPosition
	{
		typedef std::string Result;
		
		Result operator()( N::WindowRef window ) const
		{
			Point position = Ped::GetWindowPosition( window );
			
			std::string result = NN::Convert< std::string >( position.h );
			
			result += ",";
			
			result += NN::Convert< std::string >( position.v );
			
			return result;
		}
	};
	
	struct GetWindowSize
	{
		typedef std::string Result;
		
		Result operator()( N::WindowRef window ) const
		{
			Point size = Ped::GetWindowSize( window );
			
			std::string result = NN::Convert< std::string >( size.h );
			
			result += "x";
			
			result += NN::Convert< std::string >( size.v );
			
			return result;
		}
	};
	
	template < class Get >
	class sys_mac_window_REF_Query
	{
		private:
			typedef N::WindowRef Key;
			
			Key itsKey;
		
		public:
			sys_mac_window_REF_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string operator()() const
			{
				std::string output = NN::Convert< std::string >( Get()( itsKey ) ) + "\n";
				
				return output;
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
	
	const Functional_Traits< WindowRef_KeyName_Traits::Key >::Mapping sys_mac_window_REF_Mappings[] =
	{
		{ "name", &Query_Factory< GetWindowName     > },
		{ "pos",  &Query_Factory< GetWindowPosition > },
		{ "size", &Query_Factory< GetWindowSize     > },
		
		{ NULL, NULL }
	};
	
}

