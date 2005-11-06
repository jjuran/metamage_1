/*	============
 *	Locations.hh
 *	============
 */

#pragma once

// Nitrogen
#include "Nitrogen/Files.h"


namespace ALine
{
	
	namespace N = Nitrogen;
	
	using std::string;
	
	N::FSDirSpec UserProjectsFolder();
	
	bool ProjectHasSystemIncludes( const N::FSDirSpec& folder );
	
	N::FSDirSpec ProjectSourcesFolder ( const N::FSDirSpec& folder );
	N::FSDirSpec ProjectIncludesFolder( const N::FSDirSpec& folder );
	
	FSSpec ProjectDotConfFile( const string& name );
	FSSpec SourceDotListFile ( const N::FSDirSpec& folder );
	
	N::FSDirSpec ProjectDiagnosticsFolder( const string& proj, const string& target );
	N::FSDirSpec ProjectPrecompiledFolder( const string& proj, const string& target );
	N::FSDirSpec ProjectObjectsFolder    ( const string& proj, const string& target );
	N::FSDirSpec ProjectLibrariesFolder  ( const string& proj, const string& target );
	
}

