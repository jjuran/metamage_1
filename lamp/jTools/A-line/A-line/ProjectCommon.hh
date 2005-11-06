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

// Nitrogen
#include "Nitrogen/Files.h"

// A-line
#include "A-line/Project.hh"


namespace ALine
{
	
	namespace N = Nitrogen;
	
	typedef UInt32 MacDate;
	
	Project& GetProject( const ProjName& projName );
	
	void AddIncludeDir( const ProjName& projName, const N::FSDirSpec& dir, bool isSystem );
	bool GetIncludeDir( const ProjName& projName, N::FSDirSpec& dir, bool isSystem );
	
	FSSpec IncludeLocation( const IncludePath& includePath );
	
	FSSpec SourceLocation( const FileName& filename );
	FSSpec RezLocation   ( const FileName& filename );
	
	bool FindInclude( const IncludePath& includePath );
	void AddInclude ( const IncludePath& includePath, const FSSpec& file );
	
	void AddSourceFile( const FileName& filename, const FSSpec& file );
	void AddRezFile   ( const FileName& filename, const FSSpec& file );
	
	void SetCurrentSourceDir( const N::FSDirSpec& dir );
	void SetCurrentSourceDir( int zero );
	
	MacDate RecursivelyLatestDate( const IncludePath& includePath );
	MacDate RecursivelyLatestDate( const IncludePath& includePath, const FSSpec& file );
	
}

