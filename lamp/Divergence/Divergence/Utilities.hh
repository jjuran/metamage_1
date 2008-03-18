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
#else
	#include "fsspec_from_path.hh"
	#include "SystemCalls.hh"
#endif


namespace Divergence
{
	
	inline FSSpec ResolvePathToFSSpec( const char* path )
	{
	#if TARGET_RT_MAC_MACHO
		
		return Nucleus::Convert< FSSpec >( Nitrogen::FSPathMakeRef( path ).ref );
		
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

