/*	==============
 *	ResFile_Dir.hh
 *	==============
 */

#ifndef GENIE_FS_RESFILEDIR_HH
#define GENIE_FS_RESFILEDIR_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	FSTreePtr Get_ResFileDir_FSTree( const FSTreePtr&     parent,
	                                 const plus::string&  name,
	                                 const FSSpec&        file );
	
}

#endif

