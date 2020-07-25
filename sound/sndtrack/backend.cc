/*
	backend.cc
	----------
*/

#include "backend.hh"

// sndtrack
#include "portaudio.hh"


namespace backend
{

bool start()
{
	return portaudio::start();
}

bool stop()
{
	return portaudio::stop();
}

}
