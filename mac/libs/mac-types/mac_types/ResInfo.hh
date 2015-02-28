/*
	ResInfo.hh
	----------
*/

#ifndef MACTYPES_RESINFO_HH
#define MACTYPES_RESINFO_HH


namespace mac   {
namespace types {
	
	// ResInfo is not an actual Mac OS type and alignment doesn't matter.
	
	struct ResInfo
	{
		short          id;
		unsigned long  type;
		unsigned char  name[ 255 ];
	};
	
}
}

#endif
