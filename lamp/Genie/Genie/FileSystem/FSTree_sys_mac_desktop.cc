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
	
	
	class sys_mac_desktop_Query
	{
		public:
			std::string operator()() const
			{
				const BitMap& screenBits = N::GetQDGlobalsScreenBits();
				
				const Rect& bounds = screenBits.bounds;
				
				std::string output = NN::Convert< std::string >( bounds.right - bounds.left );
				
				output += "x";
				
				output += NN::Convert< std::string >( bounds.bottom - bounds.top );
				
				output += "\n";
				
				return output;
			}
	};
	
	template < class Query >
	static FSTreePtr Query_Factory( const FSTreePtr&    parent,
	                                const std::string&  name )
	{
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name ) );
	}
	
	const Singleton_Mapping sys_mac_desktop_Mappings[] =
	{
		{ "size", &Query_Factory< sys_mac_desktop_Query > },
		
		{ NULL, NULL }
	};
	
}

