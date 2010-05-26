/*
	GetPOSIXPathname.cc
	-------------------
	
	Joshua Juran
*/

#include "GetPathname.hh"

// plus
#include "plus/var_string.hh"

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif
#ifndef NITROGEN_FOLDERS_HH
#include "Nitrogen/Folders.hh"
#endif

// Io: MacFiles
#include "MacFiles/Classic.hh"


static plus::string GetPOSIXPathname_Internal( const FSSpec& file )
{
	ConstStr255Param name = file.name;
	
	plus::var_string filename;
	
	filename.resize( name[0] );
	
	std::replace_copy( &name[ 1           ],
					   &name[ 1 + name[0] ],
					   filename.begin(),
					   '/',
					   ':' );
	
	if ( file.parID == fsRtParID )
	{
		if ( file.vRefNum == io::system_root< Nitrogen::FSVolumeRefNum >() )
		{
			return "";
		}
		else
		{
			return "/Volumes/" + filename;
		}
	}
	
	return GetPOSIXPathname( io::get_preceding_directory( file ) ) + filename;
}

plus::string GetPOSIXPathname( const Nitrogen::FSDirSpec& dir )
{
	return GetPOSIXPathname_Internal( Nitrogen::FSMakeFSSpec( dir ) ) + "/";
}

plus::string GetPOSIXPathname( const FSSpec& file )
{
	bool needsTrailingSlash = file.parID == fsRtParID;
	
	return GetPOSIXPathname_Internal( file ) + ( needsTrailingSlash ? "/" : "" );
}

