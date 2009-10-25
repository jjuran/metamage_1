/*
	return_address.hh
	
	Copyright 2007-2009, Joshua Juran
*/

#ifndef RECALL_RETURNADDRESS_HH
#define RECALL_RETURNADDRESS_HH


namespace recall
{
	
	// The 'native' code type, whatever that is.
	// Most platforms have just one, but Mac OS is special:
	// It's either classic 68K or Mach-O -- never CFM.
	typedef const struct opaque_code_native*  return_address_native;
	
#if defined( __MACOS__ )
	
	typedef const struct opaque_code_cfm*  return_address_cfm;
	
#endif
	
	
#if defined( __MC68K__ )  ||  defined( __MACOS__ )
	
	// 68K is considered native for traditional Mac OS
	// Even Carbon CFM binaries may run on OS 9 and can intermingle with 68K
	typedef return_address_native return_address_68k;
	
#else
	
	typedef const struct opaque_code_alien_68k* return_address_68k;
	
#endif
	
	
#if defined( __MACOS__ )
	
	typedef return_address_cfm return_address_ppc;
	
#elif defined( __POWERPC__ )
	
	typedef return_address_native return_address_ppc;
	
#else
	
	typedef const struct opaque_code_alien_ppc* return_address_ppc;
	
#endif
	
	
#ifdef __i386__
	
	typedef return_address_native return_address_x86;
	
#else
	
	typedef const struct opaque_code_alien_x86* return_address_x86;
	
#endif
	
	
#ifdef __MACH__
	
	typedef return_address_native return_address_mach_o;
	
#else
	
	typedef const struct opaque_code_alien_mach_o* return_address_mach_o;
	
#endif


#if defined( __POWERPC__ ) && !defined( __MACH__ )  ||  defined( __MACOS__ )
	
	typedef return_address_ppc return_address_traceback;
	
#else
	
	typedef const struct opaque_code_alien_traceback* return_address_traceback;
	
#endif
	
}

#endif

