/*
	load_segment.hh
	---------------
*/

#ifndef MACSYS_LOADSEGMENT_HH
#define MACSYS_LOADSEGMENT_HH


namespace mac {
namespace sys {
	
	enum { _LoadSeg = 0xA9F0 };
	
	static inline
	asm
	void
	load_segment( short num )
	{
	// #1
	start:
		BRA.S    load
		
	// #3
	loaded:
		BRA.S    exit
		
	not_reached:
		NOP
		
	// #2
	load:
		_LoadSeg
		
	// #4
	exit:
	}
	
}
}

#endif
