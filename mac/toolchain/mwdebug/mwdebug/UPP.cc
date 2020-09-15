/*
	UPP.cc
	------
*/

#include "mwdebug/UPP.hh"

// Mac OS
#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mwdebug
#include "mwdebug/UPP.hh"


#if TARGET_API_MAC_CARBON

static
CFragConnectionID Get_InterfaceLib()
{
	CFragConnectionID result;
	
	OSErr err = GetSharedLibrary( "\p" "InterfaceLib",
	                              kPowerPCCFragArch,
	                              kFindCFrag,
	                              &result,
	                              NULL,
	                              NULL );
	
	if ( err != noErr )
	{
		return 0;
	}
	
	return result;
}

CallUniversalProc_f Get_CallUniversalProc()
{
	if ( CFragConnectionID InterfaceLib = Get_InterfaceLib() )
	{
		CallUniversalProc_f result;
		
		OSErr err = FindSymbol( InterfaceLib,
		                        "\p" "CallUniversalProc",
		                        (Ptr*) &result,
		                        NULL );
	
		if ( err == noErr )
		{
			return result;
		}
	}
	
	return 0;  // NULL
}

#endif
