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


static reactor_node* reactor_chain;

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

bool reactor_wait( timeval* timeout )
{
	const short saved_SR = disable_interrupts();
	
	fd_set readfds;
	FD_COPY( &active_readfds, &readfds );
	
	int selected = select( max_fd + 1, &readfds, NULL, NULL, timeout );
	
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
};
