/*	======
 *	bool.h
 *	======
 */

#ifndef _BOOL_H_
#define _BOOL_H_

#ifdef __cplusplus
	
	typedef bool _Bool;
	
#else
	
	enum _Bool { _False = 0, _True = 1 };
	
#endif

#endif
