/*	=========
 *	FSSpec.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSSPEC_HH
#define GENIE_FILESYSTEM_FSSPEC_HH

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FS/FSTree.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	Nitrogen::FSDirSpec Dir_From_CInfo( const CInfoPBRec& cInfo );
	
	Nitrogen::FSDirSpec Dir_From_FSSpec( const FSSpec& dir );
	
	FSSpec GetFSSpecFromFSTree( const FSTreePtr& file );
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item, bool onServer );
	
	FSTreePtr FSTreeFromFSDirSpec( const Nitrogen::FSDirSpec& dir, bool onServer );
	
}

#endif

