/*	=================
 *	ProjectDotConf.hh
 *	=================
 */

#ifndef COMPILEDRIVER_PROJECTDOTCONF_HH
#define COMPILEDRIVER_PROJECTDOTCONF_HH

// Standard C++
#include <string>
#include <vector>


namespace tool
{
	
	typedef std::string ConfigKey;
	typedef std::vector< std::string > ConfigValues;
	
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

