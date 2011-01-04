/*
	Mac/Toolbox/Utilities/ThrowOSStatus.cc
	--------------------------------------
*/

#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// nucleus
#ifndef NUCLEUS_ERRORCODE_HH
#include "nucleus/error_code.hh"
#endif

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"


namespace Mac
{
	
	void ThrowOSStatus_Internal( ::OSStatus error )
	{
		nucleus::throw_error_code< OSStatus >( error );
	}
	
}

