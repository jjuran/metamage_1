/*
	MacFiles/Classic.cc
	-------------------
*/

#include "MacFiles/Classic.hh"

// mac-file-utils
#include "mac_file/open_data_fork.hh"


#if ! __LP64__

namespace io
{

using Mac::FSFileRefNum;

nucleus::owned< FSFileRefNum >
open_data_fork( const FSSpec& file, Mac::FSIOPerm perm )
{
	short result = mac::file::open_data_fork( file, perm );
	
	if ( result < 0 )
	{
		Mac::ThrowOSStatus( result );
	}
	
	return nucleus::owned< FSFileRefNum >::seize( FSFileRefNum( result ) );
}

}

#else

int dummy;

#endif
