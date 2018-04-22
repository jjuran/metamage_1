/*
	ResInfo.hh
	----------
*/

#ifndef MACTYPES_RESINFO_HH
#define MACTYPES_RESINFO_HH


namespace mac   {
namespace types {
	
	// ResInfo is not an actual Mac OS type and alignment doesn't matter.
	
#if ! __LP64__
	
	typedef unsigned long ResType;
	
#else
	
	typedef unsigned int  ResType;
	
#endif
	
	struct ResInfo
	{
		short          id;
		ResType        type;
		unsigned char  name[ 255 ];
	};
	
}
}

#endif
