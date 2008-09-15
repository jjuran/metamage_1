/*	=========================
 *	FSTree_sys_mac_desktop.cc
 *	=========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_desktop.hh"

// Nucleus
#include "Nucleus/Convert.h"

// Nitrogen
#include "Nitrogen/QuickDraw.h"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static std::string PrintableBounds( const Rect& r )
	{
		std::string result;
		
		result += NN::Convert< std::string >( r.left );
		
		result += ",";
		
		result += NN::Convert< std::string >( r.top );
		
		result += "-";
		
		result += NN::Convert< std::string >( r.right );
		
		result += ",";
		
		result += NN::Convert< std::string >( r.bottom );
		
		return result;
	}
	
	static std::string PrintableSize( int x, int y )
	{
		std::string result;
		
		result += NN::Convert< std::string >( x );
		
		result += "x";
		
		result += NN::Convert< std::string >( y );
		
		return result;
	}
	
	inline std::string PrintableSize( Point size )
	{
		return PrintableSize( size.h, size.v );
	}
	
	inline std::string PrintableSize( const Rect& bounds )
	{
		return PrintableSize( bounds.right - bounds.left, bounds.bottom - bounds.top );
	}
	
	struct GetScreenBounds
	{
		typedef std::string Result;
		
		Result operator()( const BitMap& screenBits ) const
		{
			return PrintableBounds( screenBits.bounds );
		}
	};
	
	struct GetScreenSize
	{
		typedef std::string Result;
		
		Result operator()( const BitMap& screenBits ) const
		{
			return PrintableSize( screenBits.bounds );
		}
	};
	
	template < class Get >
	class sys_mac_desktop_Query
	{
		public:
			std::string operator()() const
			{
				const BitMap& screenBits = N::GetQDGlobalsScreenBits();
				
				std::string output = Get()( screenBits );
				
				output += "\n";
				
				return output;
			}
	};
	
	template < class Get >
	static FSTreePtr Query_Factory( const FSTreePtr&    parent,
	                                const std::string&  name )
	{
		typedef sys_mac_desktop_Query< Get > Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name ) );
	}
	
	const Singleton_Mapping sys_mac_desktop_Mappings[] =
	{
		{ "bounds", &Query_Factory< GetScreenBounds > },
		{ "size",   &Query_Factory< GetScreenSize   > },
		
		{ NULL, NULL }
	};
	
}

