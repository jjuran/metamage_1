/*
	restack.h
	---------
*/

#ifndef LAMP_RESTACK_H
#define LAMP_RESTACK_H

#ifdef __cplusplus
extern "C" {
#endif

struct _lamp_stack_footer
{
	void* frame_pointer;
	
#ifdef __POWERPC__
	
	unsigned _save_CR;
	
#endif
	
	void* return_address;
	
#ifdef __POWERPC__
	
	void* _reserved_for_compiler;
	void* _reserved_for_binder;
	void* _save_RTOC;
	
	unsigned _save_GPRs[8];
	
#endif
	
	void* stack_limit;
	
	struct _lamp_stack_footer* previous;
};

/*
	Declare _lamp_restack as returning void so 68K callers who need the return
	value are forced to cast it to the correct function type, as opposed to
	merely casting the result itself, which wouldn't work if _lamp_restack()
	were declared as returning int but f returned a pointer, since the actual
	result would be in A0 but the calling code would cast the int in D0.
*/

void _lamp_restack( unsigned param_length, const void* f, ... );

#ifdef __cplusplus
}
#endif

#endif

