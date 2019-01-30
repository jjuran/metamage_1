/*
	reactor.hh
	----------
*/

#ifndef REACTOR_HH
#define REACTOR_HH


typedef void (*reactor_callback)( struct reactor_node* node );
typedef void (*timer_callback  )( struct timer_node*   node );

struct reactor_node
{
	reactor_node* next;
	
	int fd;
	
	reactor_callback ready;
};

struct timer_node
{
	timer_node* next;
	
	unsigned long long wakeup;  // microseconds since Jan 1, 1970
	
	timer_callback ready;
};

typedef void (*reactor_call)( reactor_node* node );
typedef void (*timer_call  )( timer_node*   node );

struct reactor_core_parameter_block
{
	unsigned long version;
	unsigned long reserved;
	
	reactor_call insert;
	reactor_call remove;
	
	timer_call schedule;
	timer_call cancel;
};

#endif
