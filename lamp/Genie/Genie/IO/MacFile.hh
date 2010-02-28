/*	==========
 *	MacFile.hh
 *	==========
 */

#ifndef GENIE_IO_MACFILE_HH
#define GENIE_IO_MACFILE_HH

// Nucleus
#include "Nucleus/Shared.h"

// Nitrogen
#include "Nitrogen/Files.hh"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	boost::shared_ptr< IOHandle >
	//
	New_DataForkHandle( const Nucleus::Shared< Nitrogen::FSFileRefNum >&  refNum,
	                    OpenFlags                                         flags );
	
	boost::shared_ptr< IOHandle >
	//
	New_RsrcForkHandle( const Nucleus::Shared< Nitrogen::FSFileRefNum >&  refNum,
	                    OpenFlags                                         flags );
	
}

#endif

