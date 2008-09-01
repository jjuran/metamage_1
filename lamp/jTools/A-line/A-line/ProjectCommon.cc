/*	================
 *	ProjectCommon.cc
 *	================
 */

#include "A-line/ProjectCommon.hh"

// C++
#include <algorithm>
#include <map>
#include <vector>

// A-line
#include "A-line/BuildCommon.hh"
#include "A-line/Includes.hh"
#include "A-line/Project.hh"
#include "CompileDriver/ProjectCatalog.hh"


namespace ALine
{
	
	Project& GetProject( const std::string& project_name, Platform platform )
	{
		return *GetProjectConfig( project_name, platform ).get_refined_data();
	}
	
	
	time_t Project::RecursivelyLatestDate( const std::string& includePath ) const
	{
		DateMap::const_iterator it = its_dates.find( includePath );
		
		if ( it != its_dates.end() )
		{
			// Already stored
			return it->second;
		}
		else
		{
			// Not stored yet
			std::string path = FindIncludeRecursively( includePath );
			
			if ( path.empty() )
			{
				// We can't display this warning any more because it may be a
				// case where a system include file was included as a user include.
				// Furthermore, we were getting false positives on nonexistent
				// headers that were conditionalized for other platforms.
				// Anyway, the compiler is the ultimate judge of whether an
				// error has occurred, and it will say so.
				
				//Io::Err << "  Missing include " << q( includePath ) << "\n";
				
				return 0;  // FIXME
			}
			
			return RecursivelyLatestDate( includePath, path );
		}
	}
	
	time_t Project::RecursivelyLatestDate( const std::string&  includePath,
	                                       const std::string&  pathname ) const
	{
		const std::vector< std::string >& includes = GetIncludes( pathname ).user;
		
		time_t modDate = ModifiedDate( pathname );
		
		// For each included file
		std::vector< std::string >::const_iterator it, end = includes.end();
		
		for ( it = includes.begin();  it != end;  ++it )
		{
			time_t incDate = RecursivelyLatestDate( *it );
			
			modDate = std::max( modDate, incDate );
		}
		
		its_dates[ includePath ] = modDate;
		
		return modDate;
	}
	
}

