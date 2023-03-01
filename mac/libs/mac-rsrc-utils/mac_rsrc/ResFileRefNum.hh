/*
	ResFileRefNum.hh
	----------------
*/

#ifndef MACRSRC_RESFILEREFNUM_HH
#define MACRSRC_RESFILEREFNUM_HH


namespace mac  {
namespace rsrc {

#if __LP64__
	
	typedef int ResFileRefNum;
	
#else
	
	typedef short ResFileRefNum;
	
#endif
	
}
}

#endif
