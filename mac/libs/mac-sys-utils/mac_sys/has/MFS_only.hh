/*
	MFS_only.hh
	-----------
*/

#ifndef MACSYS_HAS_MFSONLY_HH
#define MACSYS_HAS_MFSONLY_HH


namespace mac {
namespace sys {
	
#ifdef __MC68K__
	
	short FSFCBLen : 0x03F6;
	
	inline bool has_MFS_only()  { return FSFCBLen < 0; }
	
#else
	
	inline bool has_MFS_only()  { return false; }
	
#endif
	
}
}

#endif
