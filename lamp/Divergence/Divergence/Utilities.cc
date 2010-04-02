/*
	Divergence/Utilities.cc
	-----------------------
	
	Joshua Juran
*/

#include "Divergence/Utilities.hh"

#if TARGET_RT_MAC_MACHO
	#include "Nitrogen/Files.hh"
	#include "MacFiles/Classic.hh"
	#include "poseven/Pathnames.hh"
#else
	#include "fsspec_from_path.hh"
#endif


namespace Divergence
{
	
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
		
		const std::string parent_path = io::get_preceding_directory( path );
		
		FSRef parent_ref = Nitrogen::FSPathMakeRef( parent_path ).ref;
		
		FSSpec parent_spec = Nitrogen::FSMakeFSSpec( parent_ref );
		
		return parent_spec / io::get_filename( path );
		
	#else
		
		return make_fsspec_from_path( path );
		
	#endif
	}
	
}

