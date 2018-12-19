/*
	reactor.hh
	----------
*/

#ifndef REACTOR_HH
#define REACTOR_HH


typedef void (*reactor_callback)( struct reactor_node* node );

struct reactor_node
{
	reactor_node* next;
	
	int fd;
	
	reactor_callback ready;
};

typedef void (*reactor_call)( reactor_node* node );

struct reactor_core_parameter_block
{
	unsigned long version;
	unsigned long reserved;
	
	reactor_call insert;
	reactor_call remove;
};

#endif
