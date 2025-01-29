/*	======================
 *	Genie/FileSignature.hh
 *	======================
 */

#ifndef GENIE_FILESIGNATURE_HH
#define GENIE_FILESIGNATURE_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


namespace Genie
{
	
	enum
	{
		kUnknownFSType = ::kUnknownType,  // '????'
		
		kSymLinkFileType  = 'slnk',
	};
	
	enum
	{
		kUnknownFSCreator = ::kUnknownType,  // '????'
		
		kSymLinkCreator  = 'rhap',
	};
	
	struct FileSignature
	{
		OSType  type;
		OSType  creator;
		
		FileSignature() : type(), creator()
		{
		}
		
		FileSignature( OSType  t,
		               OSType  c ) : type   ( t ),
		                             creator( c )
		{
		}
	};
	
	extern ::OSType gTextFileCreator;
	
	inline
	OSType TextFileCreator()
	{
		return gTextFileCreator;
	}
	
	FileSignature PickFileSignatureForName( const char*  name,
	                                        unsigned     length );
	
}

#endif
