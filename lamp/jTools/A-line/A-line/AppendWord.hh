/*	=============
 *	AppendWord.hh
 *	=============
 */

#ifndef ALINE_APPENDWORD_HH
#define ALINE_APPENDWORD_HH

// plus
#include "plus/var_string.hh"


namespace tool
{
	
	inline plus::string& operator<<( plus::var_string& str, const plus::string& appendage )
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

#endif

