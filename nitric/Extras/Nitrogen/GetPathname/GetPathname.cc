/*	==============
 *	GetPathname.cc
 *	==============
 */

#ifndef GETPATHNAME_HH
#include "GetPathname.hh"
#endif

// plus
#include "plus/make_string.hh"
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

