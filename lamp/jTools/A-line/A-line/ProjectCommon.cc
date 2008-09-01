/*	================
 *	ProjectCommon.cc
 *	================
 */

#include "A-line/ProjectCommon.hh"

// C++
#include <algorithm>
#include <map>
#include <set>
#include <vector>

// A-line
#include "A-line/BuildCommon.hh"
#include "A-line/Includes.hh"
#include "A-line/Project.hh"
#include "CompileDriver/ProjectCatalog.hh"


namespace ALine
{
	
	// maps (search-dir-relative) include paths to modification dates
	typedef std::map< std::string, time_t > DateMap;
	
	static DateMap gDates;
	
	
	Project& GetProject( const std::string& project_name, Platform platform )
	{
		return *GetProjectConfig( project_name, platform ).get_refined_data();
	}
	
	
	time_t RecursivelyLatestDate( const Project& project, const std::string& includePath )
	{
		DateMap::const_iterator it = gDates.find( includePath );
		
		if ( it != gDates.end() )
		{
			// Already stored
			return it->second;
		}
		else
		{
			// Not stored yet
			std::string path = project.FindIncludeRecursively( includePath );
			
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
			
			return RecursivelyLatestDate( project, includePath, path );
		}
	}
	
	/*
	static void FixNullFileType( const FSSpec& file )
	{
	#if !TARGET_API_MAC_CARBON  // FIXME:  This is a hack -- check Gestalt
		
		CInfoPBRec pb;
		
		N::FSpGetCatInfo( file, pb );
		
		FInfo& fInfo = pb.hFileInfo.ioFlFndrInfo;
		
		if ( fInfo.fdType == ::OSType( 0 ) )
		{
			fInfo.fdCreator = 'R*ch';  // FIXME:  Should be configurable
			fInfo.fdType    = 'TEXT';
			
			pb.hFileInfo.ioDirID   = file.parID;
			pb.hFileInfo.ioNamePtr = const_cast< FSSpec& >( file ).name;
			
			//N::FSpSetFileInfo( pb );
			N::ThrowOSStatus( ::PBSetCatInfoSync( &pb ) );
		}
		
	#endif
	}
	*/
	
	time_t RecursivelyLatestDate( const Project&      project,
	                              const std::string&  includePath,
	                              const std::string&  pathname )
	{
		const std::vector< std::string >& includes = GetIncludes( pathname ).user;
		
		time_t modDate = ModifiedDate( pathname );
		
		// For each included file
		std::vector< std::string >::const_iterator it, end = includes.end();
		
		for ( it = includes.begin();  it != end;  ++it )
		{
			time_t incDate = RecursivelyLatestDate( project, *it );
			
			modDate = std::max( modDate, incDate );
		}
		
		gDates[ includePath ] = modDate;
		
		return modDate;
	}
	
}

