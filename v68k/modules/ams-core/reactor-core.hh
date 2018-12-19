/*
	reactor-core.hh
	---------------
*/

#ifndef REACTORCORE_HH
#define REACTORCORE_HH


struct timeval;
struct reactor_core_parameter_block;

extern reactor_core_parameter_block the_reactor_core;

bool reactor_wait( timeval* timeout );

#endif
