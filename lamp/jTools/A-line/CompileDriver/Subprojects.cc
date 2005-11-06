/*	==============
 *	Subprojects.cc
 *	==============
 */

#include "CompileDriver/Subprojects.hh"


namespace CompileDriver
{
	
	static std::vector< N::FSDirSpec > gSubprojects;
	
	void AddSubproject( const N::FSDirSpec& location )
	{
		gSubprojects.push_back( location );
	}
	
	std::vector< N::FSDirSpec >& Subprojects()
	{
		return gSubprojects;
	}
	
}

