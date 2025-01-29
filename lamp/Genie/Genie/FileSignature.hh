/*	======================
 *	Genie/FileSignature.hh
 *	======================
 */

#ifndef GENIE_FILESIGNATURE_HH
#define GENIE_FILESIGNATURE_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSSIGNATURE_HH
#include "Mac/Files/Types/FSSignature.hh"
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
	
	inline Mac::FSCreator TextFileCreator()
	{
		return Mac::FSCreator( gTextFileCreator );
	}
	
	FileSignature PickFileSignatureForName( const char*  name,
	                                        unsigned     length );
	
}

#endif
