/*	=============
 *	AppendWord.hh
 *	=============
 */

#ifndef ALINE_APPENDWORD_HH
#define ALINE_APPENDWORD_HH

#if PRAGMA_ONCE
#pragma once
#endif


namespace tool
{
	
	inline std::string& operator<<( std::string& str, const std::string& appendage )
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

