/*	=======================
 *	Divergence/Utilities.hh
 *	=======================
 */

#ifndef DIVERGENCE_UTILITIES_HH
#define DIVERGENCE_UTILITIES_HH

#if !TARGET_OS_MAC

#error Divergence is Mac-only

#endif

// Universal Interfaces
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

// Standard C++
#include <string>

#if TARGET_RT_MAC_MACHO
	#include "Nucleus/Convert.h"
	#include "Nitrogen/Files.h"
	#include "MacFiles.hh"
	#include "poseven/Pathnames.hh"
#else
	#include "fsspec_from_path.hh"
	#include "SystemCalls.hh"
#endif


namespace Divergence
{
	
	inline FSSpec ResolvePathToFSSpec( const char* path )
	{
	#if TARGET_RT_MAC_MACHO
		
		try
		{
			return Nucleus::Convert< FSSpec >( Nitrogen::FSPathMakeRef( path ).ref );
		}
		catch ( ... )
		{
		}
		
		using namespace io::path_descent_operators;
		
		const std::string parent_path = io::get_preceding_directory( path );
		
		FSRef parent_ref = Nitrogen::FSPathMakeRef( parent_path ).ref;
		
		FSSpec parent_spec = Nucleus::Convert< FSSpec >( parent_ref );
		
		return parent_spec / io::get_filename( path );
		
	#else
		
		return make_fsspec_from_path( path );
		
	#endif
	}
	
	inline FSSpec ResolvePathToFSSpec( const std::string& path )
	{
		return ResolvePathToFSSpec( path.c_str() );
	}
	
#if TARGET_RT_MAC_MACHO
	
	inline OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply )
	{
		return ::AESend( appleEvent,
		                 reply,
		                 kAEWaitReply | kAECanInteract,
		                 kAENormalPriority,
		                 kNoTimeOut,
		                 NULL,
		                 NULL );
	}
	
#else
	
	using ::AESendBlocking;
	
#endif
	
}

#endif

