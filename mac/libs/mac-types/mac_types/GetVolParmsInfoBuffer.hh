/*
	GetVolParmsInfoBuffer.hh
	------------------------
*/

#ifndef MACTYPES_GETVOLPARMSINFOBUFFER_HH
#define MACTYPES_GETVOLPARMSINFOBUFFER_HH

// Mac OS
//#include <Files.h>
struct GetVolParmsInfoBuffer;


namespace mac   {
namespace types {
	
#pragma options align=mac68k
	
	struct GetVolParmsInfoBuffer
	{
		typedef ::GetVolParmsInfoBuffer BaseType;
		
		typedef char** Handle;
		
		short   vMVersion;
		long    vMAttrib;
		Handle  vMLocalHand;
		long    vMServerAdr;
		
		// end version 1
		
		long    vMVolumeGrade;
		short   vMForeignPrivID;
		
		// end version 2
		
		long    vMExtendedAttributes;
		
		// end version 3
		
		void*   vMDeviceID;
		
		// end version 4
		
		operator BaseType const&() const  { return *(BaseType const*) this; }
		operator BaseType      &()        { return *(BaseType      *) this; }
	};
	
#pragma options align=reset
	
}
}

#endif
