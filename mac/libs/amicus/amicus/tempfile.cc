/*
	tempfile.cc
	-----------
*/

#include "amicus/tempfile.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"


namespace amicus
{

const OSType folderType = kChewableItemsFolderType;

const char* tempfile_location()
{
	OSStatus err;
	
	FSRef ref;
	err = FSFindFolder( kUserDomain, folderType, kCreateFolder, &ref );
	
	if ( err )
	{
		return NULL;
	}
	
	static plus::var_string path;
	
	UInt32 maxPathSize = 64;
	
	do
	{
		maxPathSize *= 2;
		
		char* p = path.reset( maxPathSize );
		
		err = FSRefMakePath( &ref, (UInt8*) p, maxPathSize );
	}
	while ( err == pathTooLongErr  ||  err == buffersTooSmall );
	
	if ( err )
	{
		return NULL;
	}
	
	path.resize( path.find( '\0' ) );
	
	path += "/amicus-";
	path += gear::inscribe_unsigned_decimal( getpid() );
	path += ".skif";
	
	return path.c_str();
}

}
