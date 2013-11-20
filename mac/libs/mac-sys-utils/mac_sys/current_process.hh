/*
	current_process.hh
	------------------
*/

#ifndef MACSYS_CURRENTPROCESS_HH
#define MACSYS_CURRENTPROCESS_HH


struct ProcessSerialNumber;


namespace mac {
namespace sys {
	
	const ::ProcessSerialNumber& current_process();
	
}
}

#endif

