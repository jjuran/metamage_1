/*
	Divergence/Utilities.cc
	-----------------------
	
	Joshua Juran
*/

#include "Divergence/Utilities.hh"

// poseven
#include "poseven/functions/stat.hh"
#include "poseven/types/errno_t.hh"

#if TARGET_RT_MAC_MACHO
	#include "poseven/functions/basename.hh"
	#include "Nitrogen/Files.hh"
	#include "MacFiles/Classic.hh"
	#include "pfiles/common.hh"
#else
	#include "mac_relix/FSSpec_from_stat.hh"
#endif


namespace Divergence
{
	
	namespace p7 = poseven;
	
	
#if ! __LP64__
	
	FSSpec ResolvePathToFSSpec( const char* path )
	{
	#if TARGET_RT_MAC_MACHO
		
		try
		{
			return Nitrogen::FSMakeFSSpec( Nitrogen::FSPathMakeRef( path ).ref );
		}
		catch ( ... )
		{
		}
		
		using namespace io::path_descent_operators;
		
		const plus::string parent_path = io::get_preceding_directory( path );
		
		FSRef parent_ref = Nitrogen::FSPathMakeRef( parent_path ).ref;
		
		FSSpec parent_spec = Nitrogen::FSMakeFSSpec( parent_ref );
		
		return parent_spec / p7::basename( path );
		
	#else
		
		using mac::relix::FSSpec_from_stat;
		
		struct stat stat_buffer;
		
		const bool exists = p7::stat( path, stat_buffer );
		
		FSSpec spec;	
		
		p7::throw_posix_result( FSSpec_from_stat( stat_buffer, spec ) );
		
		return spec;
		
	#endif
	}
	
#endif  // #if ! __LP64__
	
}
