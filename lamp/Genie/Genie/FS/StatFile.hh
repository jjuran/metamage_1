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
	               Mac::FSVolumeRefNum   vRefNum,
	               Mac::FSDirID          dirID,
	               const unsigned char*  name = NULL,
	               bool                  is_rsrc_fork = false );
	
	inline void StatFile( bool async, const FSSpec& file, struct stat* sb, bool is_rsrc_fork = false )
	{
		Stat_HFS( async,
		          sb,
		          Mac::FSVolumeRefNum( file.vRefNum ),
		          Mac::FSDirID       ( file.parID   ),
		          file.name,
		          is_rsrc_fork );
	}
	
	void StatGeneric( struct ::stat* sb );
	
	void ChangeFileMode( const FSSpec& file, mode_t new_mode );
	
}

