/*
	callback.hh
	-----------
*/

#ifndef VXO_CALLBACK_HH
#define VXO_CALLBACK_HH


namespace vxo
{
	
	typedef void (*destructor)( void* );
	typedef bool (*duplicator)( void* );
	
	bool no_duplication( void* );
	bool POD_duplicator( void* );
	
}

#endif
