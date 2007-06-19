/*	=================
 *	ProjectCatalog.hh
 *	=================
 */

#ifndef COMPILEDRIVER_PROJECTCATALOG_HH
#define COMPILEDRIVER_PROJECTCATALOG_HH

// Standard C++
#include <iterator>
#include <string>
#include <vector>


namespace CompileDriver
{
	
	void ScanDirForProjects( const std::string&                                       dirPath,
	                         std::back_insert_iterator< std::vector< std::string > >  output );
	
}

#endif

