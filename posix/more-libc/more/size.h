/*
	more/size.h
	-----------
*/

#ifndef MORE_SIZE_H
#define MORE_SIZE_H

#ifdef __MWERKS__
	
	#include <size_t.h>
	
	#ifdef __cplusplus
		
		#if __MSL__ < 0x6000
			
			using namespace std;
			
		#else
			
			using std::size_t;
			
		#endif
		
	#endif
	
#else
	
	#include <stddef.h>
	
	#ifdef __cplusplus
		
		#include <cstddef>
		
	#endif
	
#endif

#endif

