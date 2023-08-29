/*
	Copy_IconSuite.cc
	-----------------
*/

#include "Genie/Utilities/Copy_IconSuite.hh"

// mac-icon-utils
#include "mac_icon/copy_IconSuite.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	n::owned< N::IconSuiteRef > Copy_IconSuite( N::IconSuiteRef iconSuite )
	{
		iconSuite = mac::icon::copy_IconSuite( iconSuite );
		
		if ( iconSuite )
		{
			return n::owned< N::IconSuiteRef >::seize( iconSuite );
		}
		
		return n::owned< N::IconSuiteRef >();
	}
	
}
