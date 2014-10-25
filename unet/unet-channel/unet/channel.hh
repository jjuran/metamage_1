/*
	channel.hh
	----------
*/

#ifndef UNET_CHANNEL_HH
#define UNET_CHANNEL_HH

// poseven
#include "poseven/extras/coprocess_box.hh"


namespace unet
{
	
	poseven::coprocess_box enchannel( int in, int out, const char* argv[] );
	
}

#endif
