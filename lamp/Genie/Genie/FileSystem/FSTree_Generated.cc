/*	===================
 *	FSTree_Generated.cc
 *	===================
 */

#include "Genie/FileSystem/FSTree_Generated.hh"

// POSIX
#include <fcntl.h>

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/IO/PropertyFile.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	boost::shared_ptr< IOHandle > FSTree_Generated::Open( OpenFlags flags ) const
	{
		if ( flags != O_RDONLY )
		{
			throw p7::errno_t( EINVAL );
		}
		
		if ( itsReadHook == NULL )
		{
			p7::throw_errno( EACCES );
		}
		
		std::string data = itsReadHook( itsKeyHook( this ) );
		
		return boost::shared_ptr< IOHandle >( new PropertyReaderFileHandle( shared_from_this(),
		                                                                    flags,
		                                                                    data ) );
	}
	
}

