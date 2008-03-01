/*	===============
 *	iota/environ.hh
 *	===============
 */

#ifndef IOTA_ENVIRON_HH
#define IOTA_ENVIRON_HH


#ifdef __cplusplus
namespace iota
{
#endif
	
#if defined( __GNUC__ ) || defined( __MACH__ )
	
	typedef char       *env_t;
	
#else
	
	typedef char const *env_t;
	
#endif
	
	typedef env_t const *environ_t;
	typedef env_t const *envp_t;
	typedef env_t const  envv_t[];
	
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
	
	extern iota::environ_t environ;
	
#else
	
	extern environ_t environ;
	
#endif


#endif

