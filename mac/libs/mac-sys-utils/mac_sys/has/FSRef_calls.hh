/*
	FSRef_calls.hh
	--------------
*/

#ifndef MACSYS_HAS_FSREFCALLS_HH
#define MACSYS_HAS_FSREFCALLS_HH


#ifndef TARGET_API_MAC_CARBON
#define TARGET_API_MAC_CARBON  0
#endif

#define LOWMEM( addr, type )  (*(type*) (addr))

#define SysVersion  LOWMEM( 0x015A, short )


namespace mac {
namespace sys {
	
	/*
		FSRef calls generally are available in Carbon or Mac OS 9.
	*/
	
	inline
	bool has_FSRef_calls()
	{
		return TARGET_API_MAC_CARBON  ||  SysVersion >= 0x0900;
	}
	
}
}

#endif
