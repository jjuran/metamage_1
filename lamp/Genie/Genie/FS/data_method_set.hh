/*
	data_method_set.hh
	------------------
*/

#ifndef GENIE_FS_DATAMETHODSET_HH
#define GENIE_FS_DATAMETHODSET_HH

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/FS/FSTree_fwd.hh"
#include "Genie/IO/IOPtr.hh"


namespace Genie
{
	
	typedef off_t (*geteof_method)( const FSTree* );
	typedef void  (*seteof_method)( const FSTree*, off_t );
	
	typedef IOPtr (*open_method)( const FSTree*, int flags, mode_t );
	
	
	struct data_method_set
	{
		open_method    open;
		geteof_method  geteof;
		seteof_method  seteof;
	};
	
}

#endif

