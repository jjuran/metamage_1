/*	================
 *	ProjectCommon.hh
 *	================
 *	
 *	Implemented by ProjectCommon.cc
 */

#pragma once

// Standard C++
#include <string>
#include <vector>

// Standard C
#include <time.h>

// Nitrogen
#include "Nitrogen/Files.h"

// A-line
#include "A-line/Project.hh"


namespace ALine
{
	
	namespace N = Nitrogen;
	
	Project& GetProject( const ProjName& projName );
	
	void AddIncludeDir( const ProjName& projName, const N::FSDirSpec& dir );
	
	std::string IncludeLocation( const IncludePath& includePath );
	
	FSSpec RezLocation( const FileName& filename );
	
	bool FindInclude( const IncludePath& includePath );
	void AddInclude ( const IncludePath& includePath, const FSSpec& file );
	
	void AddRezFile( const FileName& filename, const FSSpec& file );
	
	void SetCurrentSourceDir( const N::FSDirSpec& dir );
	void SetCurrentSourceDir( int zero );
	
	time_t RecursivelyLatestDate( const IncludePath& includePath );
	time_t RecursivelyLatestDate( const IncludePath& includePath, const FSSpec& file );
	
}

