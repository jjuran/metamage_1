/*
	api.cc
	------
*/

#include "mwdebug/api.hh"

// MacFixup
#include "UPP.h"

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mwdebug
{
	
	const int kMetroNubUserAPIVersion = 2;
	
	typedef pascal Boolean (*IsDebuggerRunningProcPtr)();
	typedef pascal Boolean (*AmIBeingDebuggedProcPtr )();
	
	enum
	{
		uppIsDebuggerRunningProcInfo = 0x00000010,
		uppAmIBeingDebuggedProcInfo  = 0x00000010,
	};
	
	DEFINE_UPP_0( IsDebuggerRunning, Boolean )
	DEFINE_UPP_0( AmIBeingDebugged,  Boolean )
	
	struct MetroNubUserEntryBlock
	{
		long   blockLength;
		short  apiLowVersion;
		short  apiHiVersion;
		Str31  nubVersion;
		
		IsDebuggerRunningUPP  isDebuggerRunning;
		AmIBeingDebuggedUPP   amIBeingDebugged;
	};
	
	static inline const MetroNubUserEntryBlock* GetMetroNubUserEntryBlock_raw()
	{
		return (const MetroNubUserEntryBlock*) mac::sys::gestalt( 'MnUI' );
	}
	
	static const MetroNubUserEntryBlock* GetMetroNubUserEntryBlock( int version )
	{
		if ( const MetroNubUserEntryBlock* nub = GetMetroNubUserEntryBlock_raw() )
		{
			if ( nub->apiLowVersion <= version  &&  version <= nub->apiHiVersion )
			{
				return nub;
			}
		}
		
		return 0;  // NULL
	}
	
	static const MetroNubUserEntryBlock* GetMetroNubUserEntryBlock()
	{
		const int v = kMetroNubUserAPIVersion;
		
		static const MetroNubUserEntryBlock* nub = GetMetroNubUserEntryBlock( v );
		
		return nub;
	}
	
	const unsigned char* MetroNubVersion()
	{
		if ( const MetroNubUserEntryBlock* nub = GetMetroNubUserEntryBlock_raw() )
		{
			return nub->nubVersion;
		}
		
		return 0;  // NULL
	}
	
	bool IsMetroNubInstalled()
	{
		return GetMetroNubUserEntryBlock();
	}
	
	bool IsDebuggerRunning()
	{
		if ( const MetroNubUserEntryBlock* nub = GetMetroNubUserEntryBlock() )
		{
			return InvokeIsDebuggerRunningUPP( nub->isDebuggerRunning );
		}
		
		return false;
	}
	
	bool AmIBeingDebugged()
	{
		if ( const MetroNubUserEntryBlock* nub = GetMetroNubUserEntryBlock() )
		{
			return InvokeAmIBeingDebuggedUPP( nub->amIBeingDebugged );
		}
		
		return false;
	}
	
}
