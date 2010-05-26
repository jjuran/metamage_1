/*
	GetMacPathname.cc
	-----------------
	
	Joshua Juran
*/

#include "GetPathname.hh"

// plus
#include "plus/make_string.hh"

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif

// Io: MacFiles
#include "MacFiles/Classic.hh"


static plus::string GetMacPathname_Internal( const FSSpec& file )
{
	plus::string filename = plus::make_string( file.name );
	
	if ( file.parID == fsRtParID )
	{
		return filename;
	}
	
	return GetMacPathname( io::get_preceding_directory( file ) ) + filename;
}

plus::string GetMacPathname( const Nitrogen::FSDirSpec& dir )
{
	return GetMacPathname_Internal( Nitrogen::FSMakeFSSpec( dir ) ) + ":";
}

plus::string GetMacPathname( const FSSpec& file )
{
	bool needsTrailingColon = file.parID == fsRtParID;
	
	return GetMacPathname_Internal( file ) + ( needsTrailingColon ? ":" : "" );
}

