/*
	gear/compare.hh
	---------------
*/

#ifndef GEAR_COMPARE_HH
#define GEAR_COMPARE_HH


namespace gear
{
	
	typedef signed char    cmp_t;
	typedef unsigned char  byte_t;
	typedef unsigned long  size_t;
	
	cmp_t compare_bytes( byte_t const*  a,
	                     size_t         a_size,
	                     byte_t const*  b,
	                     size_t         b_size );
	
}

#endif
