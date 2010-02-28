// Nitrogen/OSStatus.cc
// --------------------

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	void ThrowOSStatus_Internal( OSStatus error )
	{
		Nucleus::ThrowErrorCode< OSStatus >( error );
	}
	
}

