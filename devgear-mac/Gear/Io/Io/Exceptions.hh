/*	=============
 *	Exceptions.hh
 *	=============
 */

#ifndef IO_EXCEPTIONS_HH
#define IO_EXCEPTIONS_HH

#ifndef OTUNIXERRORS
#define OTUNIXERRORS 1
#endif

// Io
#include "io/io.hh"

// Nitrogen
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OpenTransport.h"


namespace Io
{
	
	namespace N = Nitrogen;
	
	typedef io::end_of_input EndOfInput;
	
	struct NoDataPending : public io::no_input_pending
	{
		operator N::OSStatus() const  { return N::OSStatus( kEWOULDBLOCKErr ); }
		
		operator N::OTUnixErr() const  { return N::OTUnixErr( EWOULDBLOCK ); }
	};
	
}

#endif

