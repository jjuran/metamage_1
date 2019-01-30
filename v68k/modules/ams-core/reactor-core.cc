/*
	reactor-core.cc
	---------------
*/

#include "reactor-core.hh"

// POSIX
#include <sys/select.h>

// ams-common
#include "interrupts.hh"
#include "reactor.hh"
#include "time.hh"


static reactor_node* reactor_chain;
static timer_node*   timer_chain;

static fd_set active_readfds;

static int max_fd = 0;

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
	
	const uint64_t wake = node->wakeup;
	
	timer_node** slot = &timer_chain;
	
	timer_node* next;
	
	while ( (next = *slot)  &&  wake > next->wakeup )
	{
		slot = &next->next;
	}
	
	node->next = *slot;
	
	*slot = node;
	
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
uint64_t check_timers()
{
	if ( timer_chain == NULL )
	{
		return 0;
	}
	
	const uint64_t now_time = time_microseconds();
	
	while ( timer_chain != NULL  &&  timer_chain->wakeup <= now_time )
	{
		timer_node* next = timer_chain;
		
		timer_chain = timer_chain->next;
		
		next->ready( next );
	}
	
	if ( timer_chain == NULL )
	{
		return 0;
	}
	
	return timer_chain->wakeup - now_time;
}

bool reactor_wait( timeval* timeout )
{
	const short saved_SR = disable_interrupts();
	
	if ( uint64_t dt = check_timers() )
	{
		if ( dt < microseconds( *timeout ) )
		{
			timeout->tv_sec  = dt / 1000000;
			timeout->tv_usec = dt % 1000000;
		}
	}
	
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
