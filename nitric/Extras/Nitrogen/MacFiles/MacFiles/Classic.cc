/*
	MacFiles/Classic.cc
	-------------------
*/

#include "MacFiles/Classic.hh"

// mac-sys-utils
#include "mac_sys/open_data_fork.hh"


namespace io
{

using Mac::FSFileRefNum;

nucleus::owned< FSFileRefNum >
open_data_fork( const FSSpec& file, Mac::FSIOPerm perm )
{
	short result = mac::sys::open_data_fork( file, perm );
	
	if ( result < 0 )
	{
		Mac::ThrowOSStatus( result );
	}
	
	return nucleus::owned< FSFileRefNum >::seize( FSFileRefNum( result ) );
}

}
