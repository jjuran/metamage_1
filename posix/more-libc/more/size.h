/*
	more/size.h
	-----------
*/

#ifndef MORE_SIZE_H
#define MORE_SIZE_H

#ifdef __MWERKS__
	
	#include <size_t.h>
	
	#ifdef __cplusplus
		
		using std::size_t;
		
	#endif
	
#else
	
	#include <stddef.h>
	
	#ifdef __cplusplus
		
		#include <cstddef>
		
	#endif
	
#endif

#endif

