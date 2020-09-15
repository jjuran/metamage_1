/*
	is_front_process.hh
	-------------------
*/

#ifndef MACSYS_ISFRONTPROCESS_HH
#define MACSYS_ISFRONTPROCESS_HH


struct ProcessSerialNumber;


namespace mac {
namespace sys {
	
	bool is_front_process( const ProcessSerialNumber& psn );
	
}
}

#endif
