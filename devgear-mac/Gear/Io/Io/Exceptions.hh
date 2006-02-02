/*	=============
 *	Exceptions.hh
 *	=============
 */

#ifndef IO_EXCEPTIONS_HH
#define IO_EXCEPTIONS_HH

#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OpenTransport.h"


namespace Io
{
	
	namespace N = Nitrogen;
	
	typedef Nitrogen::EOFErr EndOfInput;
	
	#if 0
	// If we do use this, we need to catch N::EOFErr from FSRead() et al and convert it.
	struct EndOfInput
	{
		operator N::OSStatus() const  { return eofErr; }
		
		// EOF is indicated in Unix by a zero return from read().
	};
	#endif
	
	struct NoDataPending
	{
		operator N::OSStatus() const  { return kEWOULDBLOCKErr; }
		
		operator N::OTUnixErr() const  { return EWOULDBLOCK; }
	};
	
}

#endif

