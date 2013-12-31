/*
	FSSpec.hh
	---------
*/

#ifndef MACTYPES_FSSPEC_HH
#define MACTYPES_FSSPEC_HH

// Mac OS
//#include <Files.h>
struct FSSpec;


namespace mac   {
namespace types {
	
#pragma options align=mac68k
	
	struct FSSpec
	{
		short          vRefNum;
		long           parID;
		unsigned char  name[64];
		
		operator ::FSSpec const&() const  { return *(::FSSpec const*) this; }
		operator ::FSSpec      &()        { return *(::FSSpec      *) this; }
	};
	
#pragma options align=reset
	
}
}

#endif
