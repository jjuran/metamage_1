/*
	gestalt.hh
	----------
*/

#ifndef MACSYS_GESTALT_HH
#define MACSYS_GESTALT_HH


namespace mac {
namespace sys {
	
	struct gestalt_record
	{
		unsigned long  selector;
		unsigned long  zero;
	};
	
	typedef gestalt_record const* const* gestalt_handle;
	
	long gestalt( unsigned long selector, long default_value = 0 );
	
	bool gestalt_defined( unsigned long selector );
	
}
}

#endif
