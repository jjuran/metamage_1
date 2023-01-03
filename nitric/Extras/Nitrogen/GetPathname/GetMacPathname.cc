/*
	GetMacPathname.cc
	-----------------
*/

#include "GetMacPathname.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// mac-file-utils
#include "mac_file/make_FSSpec.hh"
#include "mac_file/parent_directory.hh"

// plus
#include "plus/var_string.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


static inline
bool is_error( const FSSpec& file )
{
	return file.vRefNum == 0;
}

static inline
long error( const FSSpec& file )
{
	return file.parID;
}

static plus::string GetMacPathname_Internal( const FSSpec& file )
{
	// make_FSSpec() may return errors, so check here.
	
	if ( is_error( file ) )
	{
		Mac::ThrowOSStatus( error( file ) );
	}
	
	const plus::string filename( file.name );
	
	if ( file.parID == fsRtParID )
	{
		return filename;
	}
	
	plus::var_string pathname = GetMacPathname( mac::file::parent_directory( file ) ).move();
	
	pathname += filename;
	
	return move( pathname );
}

plus::string GetMacPathname( const mac::types::VRefNum_DirID& dir )
{
	plus::var_string pathname = GetMacPathname_Internal( mac::file::make_FSSpec( dir ) ).move();
	
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
