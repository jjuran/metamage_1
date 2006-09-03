/*	======================
 *	Genie/FileSignature.hh
 *	======================
 */

#ifndef GENIE_FILESIGNATURE_HH
#define GENIE_FILESIGNATURE_HH

// Standard C++
#include <string>

// Mac OS Universal Interfaces
#include <MacTypes.h>


namespace Genie
{
	
	struct FileSignature
	{
		::OSType creator;
		::OSType type;
		
		FileSignature() : creator(), type()  {}
		
		FileSignature( ::OSType creator,
		               ::OSType type ) : creator( creator ),
		                                 type   ( type    )  {}
	};
	
	::OSType TextFileCreator();
	
	FileSignature PickFileSignatureForName( const std::string& name );
	
}

#endif

