/*	==========
 *	MacFile.hh
 *	==========
 */

#ifndef GENIE_IO_MACFILE_HH
#define GENIE_IO_MACFILE_HH

// nucleus
#include "nucleus/shared.hh"

// Nitrogen
#include "Mac/Files/Types/FSFileRefNum.hh"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	boost::shared_ptr< IOHandle >
	//
	New_DataForkHandle( const nucleus::shared< Mac::FSFileRefNum >&  refNum,
	                    OpenFlags                                    flags );
	
	boost::shared_ptr< IOHandle >
	//
	New_RsrcForkHandle( const nucleus::shared< Mac::FSFileRefNum >&  refNum,
	                    OpenFlags                                    flags );
	
}

#endif

