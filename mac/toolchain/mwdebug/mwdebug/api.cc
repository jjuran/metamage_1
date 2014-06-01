/*
	api.cc
	------
*/

#include "mwdebug/api.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"


typedef unsigned char Str31[ 32 ];


namespace mwdebug
{
	
	const int kMetroNubUserAPIVersion = 2;
	
	struct MetroNubUserEntryBlock
	{
		long   blockLength;
		short  apiLowVersion;
		short  apiHiVersion;
		Str31  nubVersion;
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
	
	bool IsMetroNubInstalled()
	{
		return GetMetroNubUserEntryBlock();
	}
	
}
