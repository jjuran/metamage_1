/*
	reactor-core.cc
	---------------
*/

#include "reactor-core.hh"

// POSIX
#include <sys/select.h>

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "interrupts.hh"
#include "reactor.hh"
#include "time.hh"


const timeval zero_timeout = { 0, 0 };

static reactor_node* reactor_chain;
static timer_node*   timer_chain;

static fd_set active_readfds;

static int max_fd = 0;

static inline
timeval operator -( const timeval& a, const timeval& b )
{
	return timeval_sub( a, b );
}

static
void insert( reactor_node* node )
{
	const int fd = node->fd;
	
	const short saved_SR = disable_interrupts();
	
	if ( fd > max_fd )
	{
		max_fd = fd;
	}
	
	FD_SET( fd, &active_readfds );
	
	node->next = reactor_chain;
	
	reactor_chain = node;
	
	reenable_interrupts( saved_SR );
}

static
void remove( reactor_node* node )
{
	const short saved_SR = disable_interrupts();
	
	FD_CLR( node->fd, &active_readfds );
	
	if ( reactor_chain == node )
	{
		reactor_chain = node->next;
	}
	else
	{
		reactor_node* prev = reactor_chain;
		reactor_node* it = prev->next;
		
		while ( it != NULL )
		{
			if ( it == node )
			{
				prev->next = it->next;
				break;
			}
			
			prev = it;
			it   = it->next;
		}
	}
	
	reenable_interrupts( saved_SR );
}

void schedule( timer_node* node )
{
	const short saved_SR = disable_interrupts();
	
	const timeval& wake = node->wakeup;
	
	timer_node** slot = &timer_chain;
	
	timer_node* next;
	
	while ( (next = *slot)  &&  next != node  &&  wake > next->wakeup )
	{
		slot = &next->next;
	}
	
	if ( next == node )
	{
		ERROR = "reactor schedule: node is already scheduled";
	}
	else
	{
		node->next = *slot;
		
		*slot = node;
	}
	
	reenable_interrupts( saved_SR );
}

void cancel( timer_node* node )
{
	const short saved_SR = disable_interrupts();
	
	if ( timer_chain == node )
	{
		timer_chain = node->next;
	}
	else
	{
		timer_node* prev = timer_chain;
		timer_node* it = prev->next;
		
		while ( it != NULL )
		{
			if ( it == node )
			{
				prev->next = it->next;
				break;
			}
			
			prev = it;
			it   = it->next;
		}
	}
	
	reenable_interrupts( saved_SR );
}

static
void check_timers( timeval* timeout = NULL )
{
	timeval now;
	time( &now );
	
	while ( timer_chain != NULL  &&  timer_chain->wakeup <= now )
	{
		timer_node* next = timer_chain;
		
		timer_chain = timer_chain->next;
		
		next->ready( next );
	}
	
	if ( timeout )
	{
		if ( timer_chain )
		{
			timeval dt = timer_chain->wakeup - now;
			
			if ( dt < *timeout )
			{
				*timeout = dt;
			}
		}
	}
}

bool reactor_wait( timeval* timeout )
{
	const short saved_SR = disable_interrupts();
	
	check_timers( timeout );
	
	fd_set readfds;
	FD_COPY( &active_readfds, &readfds );
	
	int selected = select( max_fd + 1, &readfds, NULL, NULL, timeout );
	
	check_timers();
	
	if ( selected <= 0 )
	{
		reenable_interrupts( saved_SR );
		
		return false;
	}
	
	reactor_node* it = reactor_chain;
	
	while ( selected > 0  &&  it != NULL )
	{
		reactor_node* tmp = it;
		
		it = it->next;
		
		if ( FD_ISSET( tmp->fd, &readfds ) )
		{
			--selected;
			
			tmp->ready( tmp );
		}
	}
	
	reenable_interrupts( saved_SR );
	
	return true;
}

reactor_core_parameter_block the_reactor_core =
{
	0,  // version
	0,  // reserved
	&insert,
	&remove,
	&schedule,
	&cancel,
};
