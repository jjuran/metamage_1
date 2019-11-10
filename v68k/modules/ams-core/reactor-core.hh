/*
	reactor-core.hh
	---------------
*/

#ifndef REACTORCORE_HH
#define REACTORCORE_HH


struct timer_node;
struct timeval;
struct reactor_core_parameter_block;

extern const timeval zero_timeout;

extern reactor_core_parameter_block the_reactor_core;

void schedule( timer_node* node );
void cancel  ( timer_node* node );

bool reactor_wait( timeval* timeout );

#endif
