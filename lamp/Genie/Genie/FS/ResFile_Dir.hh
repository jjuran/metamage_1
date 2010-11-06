/*	==============
 *	ResFile_Dir.hh
 *	==============
 */

#ifndef GENIE_FS_RESFILEDIR_HH
#define GENIE_FS_RESFILEDIR_HH

// Genie
#include "Genie/FS/FSTree.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	FSTreePtr Get_ResFileDir_FSTree( const FSTreePtr&     parent,
	                                 const plus::string&  name,
	                                 const FSSpec&        file );
	
}

#endif

