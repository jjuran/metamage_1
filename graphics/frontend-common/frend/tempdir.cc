/*
	tempdir.cc
	----------
*/

#include "frend/tempdir.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Standard C
#include <stdlib.h>


#define COUNT( array )  (sizeof array / sizeof array[0])


namespace frend
{

const char* chewable_items_path()
{
	static char* path;
	
#ifdef __APPLE__
	
	const OSType folderType = kChewableItemsFolderType;
	
	OSStatus err;
	
	FSRef ref;
	err = FSFindFolder( kUserDomain, folderType, kCreateFolder, &ref );
	
	if ( err )
	{
		return NULL;
	}
	
	UInt32 maxPathSize = 64;
	
	do
	{
		maxPathSize *= 2;
		
		free( path );
		
		path = (char*) malloc( maxPathSize );
		
		err = path ? FSRefMakePath( &ref, (UInt8*) path, maxPathSize )
		           : memFullErr;
	}
	while ( err == pathTooLongErr  ||  err == buffersTooSmall );
	
	if ( err )
	{
		return NULL;
	}
	
#endif
	
	return path;
}

static const char* tmp_dir_vars[] =
{
	"TMP",
	"TMPDIR",
	"TEMP",
};

const char* tmp_dir_path()
{
	for ( int i = 0;  i < COUNT( tmp_dir_vars );  ++i )
	{
		if ( const char* path = getenv( tmp_dir_vars[ i ] ) )
		{
			return path;
		}
	}
	
	return "/tmp";
}

}
