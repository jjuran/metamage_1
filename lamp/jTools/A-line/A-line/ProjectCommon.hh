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

// A-line
#include "A-line/Project.hh"


namespace ALine
{
	
	Project& GetProject( const ProjName& projName );
	
	void AddIncludeDir( const ProjName& projName );
	
	std::string IncludeLocation( const IncludePath& includePath );
	
	std::string RezLocation( const FileName& filename );
	
	bool FindInclude( const IncludePath& includePath );
	void AddInclude ( const IncludePath& includePath, const std::string& file );
	
	void AddRezFile( const FSSpec& file );
	
	/*
	void SetCurrentSourceDir( const N::FSDirSpec& dir );
	void SetCurrentSourceDir( int zero );
	*/
	
	time_t RecursivelyLatestDate( const IncludePath& includePath );
	time_t RecursivelyLatestDate( const IncludePath& includePath, const std::string& pathname );
	
}

