/*
	freestore_free.hh
	-----------------
	
	Programs that don't use the free store, but do construct objects on the
	stack, by default get saddled with an unneeded exception-handling runtime
	when using the Metrowerks toolchain, but this can be avoided.
	
	Including this header overrides global operator new and operator delete.
	The new definition of operator new calls an undefined function, so if
	any calls to operator new exist in the program, it will fail to link
	(in which case you shouldn't be including freestore_free.hh anyway).
	
	Having established that a program doesn't call operator new, we assume
	that it also doesn't call operator delete /explicitly/.  Unfortunately,
	Metrowerks C++ generates runtime code for destructors that unconditionally
	includes a branch that calls operator delete, even if that branch can
	never be taken (e.g. since objects are only constructed on the stack).
	Since operator delete calls __unexpected() given a NULL argument (again,
	even if that argument can never be NULL, or in this case, even though it
	will never be called in the first place), the entire exception-handling
	subsystem gets linked into the code, even though none of it gets used.
*/

#ifndef IOTA_FREESTOREFREE_HH
#define IOTA_FREESTOREFREE_HH


#ifdef __MWERKS__

extern void* operator_new_is_undefined;

void* operator new( unsigned long )
{
	return operator_new_is_undefined;
}

void operator delete( void* )
{
}

#endif

#endif
