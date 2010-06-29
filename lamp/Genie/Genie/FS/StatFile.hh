/*	===========
 *	StatFile.hh
 *	===========
 */

// POSIX
#include <sys/types.h>

// Nitrogen
#include "Mac/Files/Types/FSDirID.hh"
#include "Mac/Files/Types/FSVolumeRefNum.hh"

// <sys/stat.h>
struct stat;


namespace Genie
{
	
	void Stat_HFS( bool                  async,
	               struct ::stat*        sb,
	               const CInfoPBRec&     cInfo,
	               const unsigned char*  name,
	               bool                  is_rsrc_fork = false );
	
	void StatGeneric( struct ::stat* sb );
	
	void ChangeFileMode( const FSSpec& file, mode_t new_mode );
	
}

