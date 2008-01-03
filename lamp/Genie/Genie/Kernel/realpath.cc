/*	===========
 *	realpath.cc
 *	===========
 */

// POSIX
#include <unistd.h>

// MoreFiles
#include "FileCopy.h"

// Nitrogen
#include "Nitrogen/Files.h"
#include "Nitrogen/OSStatus.h"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_realpath )
	DEFINE_MODULE_INIT(  Kernel_realpath )
	
	namespace N = Nitrogen;
	
	using namespace io::path_descent_operators;
	
	
	static ssize_t realpath_k( const char* pathname, char* resolved_path, size_t size )
	{
		SystemCallFrame frame( "realpath_k" );
		
		try
		{
			FSTreePtr cwd = frame.Caller().GetCWD();
			
			FSTreePtr file = ResolvePathname( pathname, cwd );
			
			ResolveLinks_InPlace( file );
			
			std::string resolved = file->Pathname();
			
			size = std::min( size, resolved.size() );
			
			std::copy( resolved.begin(), resolved.begin() + size, resolved_path );
			
			return resolved.size();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( realpath_k );
	
}

