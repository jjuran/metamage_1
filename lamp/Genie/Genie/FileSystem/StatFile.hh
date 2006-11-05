/*	===========
 *	StatFile.hh
 *	===========
 */

// POSIX
#include <sys/types.h>

// <Files.h>
struct FSSpec;

// <sys/stat.h>
struct stat;


namespace Genie
{
	
	void StatFile( const FSSpec& file, struct ::stat* sb, bool wantRsrcFork = false );
	
	void StatGeneric( struct ::stat* sb );
	
	void ChangeFileMode( const FSSpec& file, mode_t new_mode );
	
}

