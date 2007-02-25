/*	=============
 *	MWScripter.cc
 *	=============
 */

#include "A-line/MWScripter.hh"


namespace ALine
{
	
	std::string& operator<<( std::string& str, const std::string& appendage )
	{
		if ( appendage.size() > 0 )
		{
			if ( str.size() > 0 )
			{
				str += " ";
			}
			
			str += appendage;
		}
		
		return str;
	}
	
}

