/*
	FSIORefNum.hh
	-------------
*/

#ifndef MACFILE_FSIOREFNUM_HH
#define MACFILE_FSIOREFNUM_HH


namespace mac  {
namespace file {

#if __LP64__
	
	typedef int FSIORefNum;
	
#else
	
	typedef short FSIORefNum;
	
#endif
	
}
}

#endif
