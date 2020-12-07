/*	=================
 *	ProjectDotConf.hh
 *	=================
 */

#ifndef ALINE_PROJECTDOTCONF_HH
#define ALINE_PROJECTDOTCONF_HH

// Standard C++
#include <vector>

// plus
#include "plus/string.hh"

// A-line
#include "A-line/StringVector.hh"


namespace tool
{
	
	typedef plus::string ConfigKey;
	typedef StringVector ConfigValues;
	
	struct DotConfLine
	{
		int lineNumber;
		ConfigKey key;
		ConfigValues values;
	};
	
	typedef std::vector< DotConfLine > DotConfData;
	
	void ReadProjectDotConf( const char* pathname, DotConfData& data );
	
}

#endif
