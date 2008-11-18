/*	==========
 *	AsyncIO.hh
 *	==========
 */

#ifndef GENIE_ASYNCIO_HH
#define GENIE_ASYNCIO_HH

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/Process/AsyncYield.hh"


namespace Genie
{
	
	struct Async {};
	
	
	//using Nitrogen::ThrowEOF_Always;
	using Nitrogen::ThrowEOF_OnZero;
	using Nitrogen::ThrowEOF_Never;
	
	using Nitrogen::FNF_Throws;
	using Nitrogen::FNF_Returns;
	
	
	// Async read
	SInt32 FSRead( Nitrogen::FSFileRefNum  file,
	               SInt32                  requestCount,
	               void *                  buffer,
	               Async                   async,
	               ThrowEOF_OnZero         policy );
	
	SInt32 FSRead( Nitrogen::FSFileRefNum  file,
	               SInt32                  requestCount,
	               void *                  buffer,
	               Async                   async,
	               ThrowEOF_Never          policy );
	
	// Async write
	SInt32 FSWrite( Nitrogen::FSFileRefNum  file,
	                SInt32                  requestCount,
	                const void *            buffer,
	                Async                   async );
	
	
	// Asynchronous, throws FNFErr
	void FSpGetCatInfo( const FSSpec&  item,
	                    CInfoPBRec&    pb,
	                    Async          async,
	                    FNF_Throws     policy = FNF_Throws() );
	
	// Asynchronous, returns false on fnfErr
	bool FSpGetCatInfo( const FSSpec&  item,
	                    CInfoPBRec&    pb,
	                    Async          async,
	                    FNF_Returns    policy );
	
}

#endif

