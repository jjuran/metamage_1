/*
	GetMachineName.cc
	-----------------
	
	Copyright 2010, Joshua Juran
*/

#include "Genie/Utilities/GetMachineName.hh"

#ifndef __GESTALT__
#include <Gestalt.h>
#endif


namespace Genie
{
	
	const unsigned char* GetMachineName()
	{
		SInt32 mnam;
		
		ConstStr255Param& result = *(ConstStr255Param*) &mnam;
		
		if ( OSErr err = ::Gestalt( gestaltUserVisibleMachineName, &mnam ) )
		{
			result = "\p" "?";
		}
		
		return result;
	}
	
}

