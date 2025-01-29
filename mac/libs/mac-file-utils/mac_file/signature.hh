/*
	signature.hh
	------------
*/

#ifndef MACFILE_SIGNATURE_HH
#define MACFILE_SIGNATURE_HH

#if __LP64__
	
	typedef unsigned int OSType;
	
#else
	
	typedef unsigned long OSType;
	
#endif


namespace mac  {
namespace file {
	
	struct signature
	{
		OSType type;
		OSType creator;
	};
	
}
}

#endif
