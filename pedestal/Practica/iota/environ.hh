/*	===============
 *	iota/environ.hh
 *	===============
 */

#ifndef IOTA_ENVIRON_HH
#define IOTA_ENVIRON_HH

#ifdef __MC68K__
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#endif


namespace iota
{
	
#if defined( __GNUC__ ) || defined( __MACH__ )
	
	typedef char       *env_t;
	
#else
	
	typedef char const *env_t;
	
#endif
	
	typedef env_t const *envp_t;
	typedef env_t const  environ_t[];
	
}

#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	// ToolScratch:  addr = 0x09CE, len = 8 bytes
	
# ifdef __cplusplus
	
	static iota::environ_t& environ = *reinterpret_cast< iota::environ_t* >(LMGetToolScratch() + 4);
	
# else
	
	#define                 environ  (*                 (iota::environ_t*) (LMGetToolScratch() + 4) )
	
# endif
	
#else
	
	extern iota::environ_t environ;
	
#endif

#endif

