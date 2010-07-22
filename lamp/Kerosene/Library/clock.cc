/*
	clock.cc
	--------
*/

// Standard C
#include "time.h"

// Lamp
#include "lamp/parameter_block.h"
#include "tool-runtime/parameter_block.h"


#pragma exceptions off


clock_t clock( void )
{
	return global_system_params->microsecond_timer();
}

