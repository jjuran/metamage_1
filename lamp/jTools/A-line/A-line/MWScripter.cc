/*	=============
 *	MWScripter.cc
 *	=============
 */

#include "A-line/MWScripter.hh"


namespace ALine
{
	
	string& operator<<( string& str, const string& appendage )
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

