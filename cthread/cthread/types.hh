/*
	types.hh
	--------
*/

#ifndef CTHREAD_TYPES_HH
#define CTHREAD_TYPES_HH


namespace cthread
{
	
	typedef void* (*start_proc)( void*, const void*, const void* );
	
	typedef void (*switch_proc)( void* );
	
}

#endif
