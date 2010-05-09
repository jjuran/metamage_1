/*	=================
 *	ProjectDotConf.hh
 *	=================
 */

#ifndef COMPILEDRIVER_PROJECTDOTCONF_HH
#define COMPILEDRIVER_PROJECTDOTCONF_HH

// Standard C++
#include <vector>

// plus
#include "plus/string.hh"


namespace tool
{
	
	typedef plus::string ConfigKey;
	
	typedef std::vector< plus::string > ConfigValues;
	
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

