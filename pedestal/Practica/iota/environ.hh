/*	===============
 *	iota/environ.hh
 *	===============
 */

#ifndef IOTA_ENVIRON_HH
#define IOTA_ENVIRON_HH


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

extern iota::environ_t environ;

#endif

