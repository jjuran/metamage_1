/*	===========
 *	StatFile.hh
 *	===========
 */

// POSIX
#include <sys/types.h>

// Nitrogen
#ifndef MAC_FILES_TYPES_FSDIRID_HH
#include "Mac/Files/Types/FSDirID.hh"
#endif
#ifndef MAC_FILES_TYPES_FSVOLUMEREFNUM_HH
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#endif

// <sys/stat.h>
struct stat;


namespace Genie
{
	
	mode_t GetItemMode( const HFileInfo& hFileInfo );
	
	void Stat_HFS( struct stat*          sb,
	               const CInfoPBRec&     cInfo,
	               const unsigned char*  name,
	               bool                  is_rsrc_fork = false );
	
	void StatGeneric( struct stat* sb );
	
	void ChangeFileMode( const FSSpec& file, mode_t new_mode );
	
}
