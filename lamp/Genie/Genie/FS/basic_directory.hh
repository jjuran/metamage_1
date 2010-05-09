/*
	basic_directory.hh
	------------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_FS_BASICDIRECTORY_HH
#define GENIE_FS_BASICDIRECTORY_HH

// Genie
#include "Genie/FS/FSTree_Directory.hh"


namespace Genie
{
	
	typedef FSTreePtr (*Lookup_Proc )( const FSTreePtr& parent, const plus::string& name );
	typedef void      (*Iterate_Proc)( FSTreeCache& cache );
	
	
	FSTreePtr new_basic_directory( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               Lookup_Proc          lookup,
	                               Iterate_Proc         iterate );
	
}

#endif

