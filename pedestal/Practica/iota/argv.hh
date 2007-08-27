/*	============
 *	iota/argv.hh
 *	============
 */

#ifndef IOTA_ARGV_HH
#define IOTA_ARGV_HH


namespace iota
{
	
#if defined( __GNUC__ ) || defined( __MACH__ )
	
	typedef char       *const argv_t[];
	
#else
	
	typedef char const *const argv_t[];
	
#endif
	
}

#endif

