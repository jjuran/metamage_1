// Nitrogen/OSStatus.cp
// --------------------

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif


namespace Nitrogen
{
	
	void ThrowOSStatus_Internal( OSStatus                          error,
	                             const Nucleus::DebuggingContext&  debugging )
	{
		Nucleus::ThrowErrorCode< OSStatus >( error, debugging );
	}
	
}

