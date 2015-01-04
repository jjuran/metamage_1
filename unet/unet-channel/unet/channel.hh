/*
	channel.hh
	----------
*/

#ifndef UNET_CHANNEL_HH
#define UNET_CHANNEL_HH

// poseven
#ifndef POSEVEN_EXTRAS_COPROCESSBOX_HH
#include "poseven/extras/coprocess_box.hh"
#endif


namespace unet
{
	
	poseven::coprocess_box enchannel( int in, int out, const char* argv[] );
	
}

#endif
