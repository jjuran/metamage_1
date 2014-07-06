/*	============
 *	PseudoTTY.hh
 *	============
 */

#ifndef GENIE_IO_PSEUDOTTY_HH
#define GENIE_IO_PSEUDOTTY_HH

// vfs
#include "vfs/filehandle_ptr.hh"


namespace relix
{
	
	void new_pseudotty_pair( vfs::filehandle_ptr&  master,
	                         vfs::filehandle_ptr&  slave );
	
}

#endif
