/*	=======
 *	Dummy.c
 *	=======
 */

// Universal Interfaces
#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif

extern pascal OSErr InitializeFragment( const CFragInitBlock* initBlock );
extern pascal void TerminateFragment();

#pragma export on
	
	pascal OSErr InitializeFragment( const CFragInitBlock* initBlock )
	{
		return noErr;
	}
	
	pascal void TerminateFragment()
	{
		// do nothing
	}
	
#pragma export reset

