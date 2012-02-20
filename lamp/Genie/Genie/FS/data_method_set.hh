/*
	data_method_set.hh
	------------------
*/

#ifndef GENIE_FS_DATAMETHODSET_HH
#define GENIE_FS_DATAMETHODSET_HH

// POSIX
#include <sys/types.h>


namespace Genie
{
	
	class FSTree;
	
	typedef off_t (*geteof_method)( const FSTree* );
	typedef void  (*seteof_method)( const FSTree*, off_t );
	
	
	struct data_method_set
	{
		geteof_method  geteof;
		seteof_method  seteof;
	};
	
}

#endif

