/*	=========
 *	FSSpec.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSSPEC_HH
#define GENIE_FILESYSTEM_FSSPEC_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSDIRSPEC_HH
#include "Mac/Files/Types/FSDirSpec.hh"
#endif

// Genie
#include "Genie/FS/FSTreePtr.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	Mac::FSDirSpec Dir_From_CInfo( const CInfoPBRec& cInfo );
	
	Mac::FSDirSpec Dir_From_FSSpec( const FSSpec& dir );
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item );
	
	FSTreePtr FSTreeFromFSDirSpec( const Mac::FSDirSpec& dir );
	
}

#endif

