/*
	Genie/FS/HFS/MoveRename.hh
	--------------------------
*/

#ifndef GENIE_FS_HFS_MOVERENAME_HH
#define GENIE_FS_HFS_MOVERENAME_HH

//#include <Files.h>
struct FSSpec;


namespace Genie
{
	
	short MoveRename( const FSSpec&         src,
	                  const long            dst_dir,
	                  const unsigned char*  new_name );
	
}

#endif
