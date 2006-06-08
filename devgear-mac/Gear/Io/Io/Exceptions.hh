/*	=============
 *	Exceptions.hh
 *	=============
 */

#ifndef IO_EXCEPTIONS_HH
#define IO_EXCEPTIONS_HH

#ifndef OTUNIXERRORS
#define OTUNIXERRORS 1
#endif

// Nitrogen
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OpenTransport.h"


namespace Io
{
	
	namespace N = Nitrogen;
	
	struct EndOfInput
	{
		operator N::OSStatus() const  { return eofErr; }
		
		// EOF is indicated in Unix by a zero return from read().
	};
	
	struct NoDataPending
	{
		operator N::OSStatus() const  { return kEWOULDBLOCKErr; }
		
		operator N::OTUnixErr() const  { return EWOULDBLOCK; }
	};
	
}

#endif

