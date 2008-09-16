/*	============
 *	iota/argv.hh
 *	============
 */

#ifndef IOTA_ARGV_HH
#define IOTA_ARGV_HH


namespace iota
{
	
#if defined( __GNUC__ ) || defined( __MACH__ )
	
	typedef char       *arg_t;
	
#else
	
	typedef char const *arg_t;
	
#endif
	
	typedef arg_t const *argp_t;
	typedef arg_t const  argv_t[];
	
	typedef char const *const const_argv_t[];
}

#endif

