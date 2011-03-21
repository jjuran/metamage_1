/*
	iota/size_t.h
	-------------
*/

#ifndef IOTA_SIZE_T_H
#define IOTA_SIZE_T_H

#ifdef __MWERKS__
	
	#include <size_t.h>
	
	#ifdef __cplusplus
		
		using std::size_t;
		
	#endif
	
#else
	
	#include <stddef.h>
	
#endif

#endif

