/*
	api.cc
	------
*/

#include "mwdebug/api.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mwdebug
#include "mwdebug/UPP.hh"


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
	
	typedef const MetroNubUserEntryBlock* MetroNubUserEntryBlockPtr;
	
	static inline
	MetroNubUserEntryBlockPtr GetMetroNubUserEntryBlock_raw()
	{
		return (MetroNubUserEntryBlockPtr) mac::sys::gestalt( 'MnUI' );
	}
	
	static
	MetroNubUserEntryBlockPtr GetMetroNubUserEntryBlock( int vers )
	{
		if ( MetroNubUserEntryBlockPtr nub = GetMetroNubUserEntryBlock_raw() )
		{
			if ( nub->apiLowVersion <= vers  &&  vers <= nub->apiHiVersion )
			{
				return nub;
			}
		}
		
		return 0;  // NULL
	}
	
	static
	MetroNubUserEntryBlockPtr GetMetroNubUserEntryBlock()
	{
		const int v = kMetroNubUserAPIVersion;
		
		static MetroNubUserEntryBlockPtr nub = GetMetroNubUserEntryBlock( v );
		
		return nub;
	}
	
	const unsigned char* MetroNubVersion()
	{
		if ( MetroNubUserEntryBlockPtr nub = GetMetroNubUserEntryBlock_raw() )
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
		if ( MetroNubUserEntryBlockPtr nub = GetMetroNubUserEntryBlock() )
		{
			return InvokeIsDebuggerRunningUPP( nub->isDebuggerRunning );
		}
		
		return false;
	}
	
	bool AmIBeingDebugged()
	{
		if ( MetroNubUserEntryBlockPtr nub = GetMetroNubUserEntryBlock() )
		{
			return InvokeAmIBeingDebuggedUPP( nub->amIBeingDebugged );
		}
		
		return false;
	}
	
}
