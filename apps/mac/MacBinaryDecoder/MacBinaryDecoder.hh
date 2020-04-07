/*
	MacBinaryDecoder.hh
	-------------------
*/

#ifndef MACBINARYDECODER_HH
#define MACBINARYDECODER_HH

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPE_HH
#include "Mac/AppleEvents/Types/DescType.hh"
#endif

// MacFiles
#include "MacFiles/Classic.hh"
#include "MacFiles/Unicode.hh"


namespace MacBinaryDecoder
{
	
	struct FSSpec_Io_Details : public Nitrogen::FSSpec_Io_Details
	{
		static const Mac::DescType typeFileSpec = Mac::typeFSS;
	};
	
	struct FSRef_Io_Details : public Nitrogen::FSRef_Io_Details
	{
		static const Mac::DescType typeFileSpec = Mac::typeFSRef;
	};
	
#if TARGET_API_MAC_CARBON
	
	typedef FSRef_Io_Details Io_Details;
	
#else
	
	typedef FSSpec_Io_Details Io_Details;
	
#endif
	
}

#endif
