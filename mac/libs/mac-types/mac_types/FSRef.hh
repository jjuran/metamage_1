/*
	FSRef.hh
	--------
*/

#ifndef MACTYPES_FSREF_HH
#define MACTYPES_FSREF_HH

// Mac OS
//#include <Files.h>
struct FSRef;


namespace mac   {
namespace types {
	
	typedef unsigned char Str63[ 64 ];
	
	typedef short SInt16;
	
#ifdef __LP64__
	
	typedef unsigned int UInt32;
	
#else
	
	typedef unsigned long UInt32;
	
#endif
	
	enum
	{
		kFSRefNodeIDIsInvalid   = 0x0000,  // this is a guess
		kFSRefNodeIDIsFile      = 0x8000,
		kFSRefNodeIDIsDirectory = 0xC000,
	};
	
	struct FSRef
	{
		SInt16  vRefNum;    //  0
		SInt16  flags;      //  2
		UInt32  reserved1;  //  4  (zero for /, nodeID otherwise)
		UInt32  nodeID;     //  8
		UInt32  parID;      // 12
		Str63   name;       // 16
		
		operator ::FSRef const&() const  { return *(::FSRef const*) this; }
		operator ::FSRef      &()        { return *(::FSRef      *) this; }
	};
	
}
}

#endif
