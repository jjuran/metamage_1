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
#include "Genie/FileSystem/FSTree_Property.hh"


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
		
		static Result Get( const BitMap& screenBits )
		{
			return PrintableBounds( screenBits.bounds );
		}
	};
	
	struct GetScreenSize
	{
		typedef std::string Result;
		
		static Result Get( const BitMap& screenBits )
		{
			return PrintableSize( screenBits.bounds );
		}
	};
	
	template < class Accessor >
	struct sys_mac_desktop_Property
	{
		static std::string Read( const FSTree* )
		{
			const BitMap& screenBits = N::GetQDGlobalsScreenBits();
			
			return NN::Convert< std::string >( Accessor::Get( screenBits ) );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_mac_desktop_Property< Accessor > Property;
		
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Read ) );
	}
	
	const FSTree_Premapped::Mapping sys_mac_desktop_Mappings[] =
	{
		{ "bounds", &Property_Factory< GetScreenBounds > },
		{ "size",   &Property_Factory< GetScreenSize   > },
		
		{ NULL, NULL }
	};
	
}

