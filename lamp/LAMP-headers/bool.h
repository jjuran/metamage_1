/*	======
 *	bool.h
 *	======
 */

#ifndef KEROSENE_BOOL_H
#define KEROSENE_BOOL_H

#ifdef __cplusplus
	
	typedef bool _Bool;
	
#else
	
	enum _Bool { _False = 0, _True = 1 };
	
#endif

#endif

