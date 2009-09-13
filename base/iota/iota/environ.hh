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
	
	typedef char **envp_t;
	
#else
	
	typedef char const *const *envp_t;
	
#endif
	
	typedef envp_t environ_t;
	
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
	
	extern iota::environ_t environ;
	
#else
	
	extern environ_t environ;
	
#endif


#endif

