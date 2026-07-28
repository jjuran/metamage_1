/*
	SlurpFile.cc
	------------
*/

#include "MacPlus/SlurpFile.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// mac-file-utils
#include "mac_file/open_data_fork.hh"
#include "mac_file/rw.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace MacPlus
{

plus::string SlurpFile( const FSSpec& file )
{
	using mac::file::FSIORefNum;
	using mac::file::open_data_fork;
	
	plus::string result;
	
	FSIORefNum input = open_data_fork( file, fsRdPerm );
	
	OSErr err = input;
	
	if ( input >= 0 )
	{
		Size size;
		char* p;
		
		(err = GetEOF( input, &size ))                                  ||
		(! (p = result.reset_nothrow( size ))  &&  (err = memFullErr))  ||
		(err = mac::file::read_all( input, p, size ));
		
		FSClose( input );
	}
	
	Mac::ThrowOSStatus( err );
	
	return result;
}

}
