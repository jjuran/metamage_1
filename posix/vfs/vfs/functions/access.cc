/*
	access.cc
	---------
*/

#include "vfs/functions/access.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void access( const node& that, int amode, int flags )
	{
		const mode_t mode = that.filemode();
		
		// If the item doesn't exist, access fails.
		
		if ( mode == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		const uid_t user = that.user();
		
		// The root user always has access.
		
		if ( user == 0 )
		{
			return;  // TODO:  Always check X_OK for non-directories.
		}
		
		int antimode = ~mode;  // withheld permissions
		
		// Check the sought permissions against the withheld permissions.
		
		if ( amode & antimode )
		{
			// World-level access not provided.
			// TODO:  Check user and group access.
			
			p7::throw_errno( EACCES );
		}
	}
	
}
