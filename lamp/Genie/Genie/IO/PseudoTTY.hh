/*	============
 *	PseudoTTY.hh
 *	============
 */

#ifndef GENIE_IO_PSEUDOTTY_HH
#define GENIE_IO_PSEUDOTTY_HH

// vfs
#include "vfs/filehandle_ptr.hh"


namespace Genie
{
	
	void GetNewPseudoTTYPair( vfs::filehandle_ptr&  master,
	                          vfs::filehandle_ptr&  slave );
	
}

#endif
