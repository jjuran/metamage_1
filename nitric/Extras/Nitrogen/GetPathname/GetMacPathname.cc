/*
	GetMacPathname.cc
	-----------------
*/

#include "GetMacPathname.hh"

// plus
#include "plus/var_string.hh"

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif

// Io: MacFiles
#include "MacFiles/Classic.hh"


static plus::string GetMacPathname_Internal( const FSSpec& file )
{
	const plus::string filename( file.name );
	
	if ( file.parID == fsRtParID )
	{
		return filename;
	}
	
	plus::var_string pathname = GetMacPathname( io::get_preceding_directory( file ) ).move();
	
	pathname += filename;
	
	return move( pathname );
}

plus::string GetMacPathname( const Nitrogen::FSDirSpec& dir )
{
	plus::var_string pathname = GetMacPathname_Internal( Nitrogen::FSMakeFSSpec( dir ) ).move();
	
	pathname += ':';
	
	return move( pathname );
}

plus::string GetMacPathname( const FSSpec& file )
{
	plus::string pathname = GetMacPathname_Internal( file );
	
	bool needsTrailingColon = file.parID == fsRtParID;
	
	if ( needsTrailingColon )
	{
		plus::var_string volume_name = move( pathname );
		
		volume_name += ':';
		
		pathname = move( volume_name );
	}
	
	return pathname;
}
