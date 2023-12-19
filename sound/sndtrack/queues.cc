/*
	queues.cc
	---------
*/

#include "queues.hh"

// Boost
#ifndef __RELIX__
#include <boost/atomic.hpp>
#endif

// sndtrack
#include "buffers.hh"


#ifndef NULL
#define NULL  0
#endif

#ifdef __RELIX__
typedef                queue_node*   queue_rtptr;
#else
typedef boost::atomic< queue_node* > queue_rtptr;
#endif

struct rt_queue
{
	queue_node*  head;
	queue_rtptr  next;
	queue_rtptr  tail;
	
	rt_queue();
};

rt_queue::rt_queue()
{
	queue_node* dummy = (queue_node*) alloc_buffer( sizeof (queue_node) );
	
	dummy->invalidate();
	
	head =
	next =
	tail = dummy;
}

queue_node* queue_alloc_node( unsigned minimum_node_size )
{
	queue_node* node = (queue_node*) alloc_buffer( minimum_node_size );
	
	if ( node != NULL )
	{
		node->next = NULL;
		node->invalidate();
	}
	
	return node;
}

void queue_append( rt_queue& queue, queue_node* node )
{
	queue_node* tail = queue.tail;
	
	tail->next = node;
	queue.tail = node;
}

queue_node* find( rt_queue& queue, node_predicate test, const void* data )
{
	queue_node* node = queue.next;
	queue_node* tail = queue.tail;
	
	while ( ! test( node, data ) )
	{
		if ( node == tail )
		{
			return NULL;
		}
		
		node = node->next;
	}
	
	return node;
}

bool void_through( rt_queue& queue, node_predicate test, const void* data )
{
	queue_node* node = queue.next;
	queue_node* tail = queue.tail;
	
	while ( ! test( node, data ) )
	{
		node->invalidate();
		
		if ( node == tail )
		{
			return false;
		}
		
		node = node->next;
	}
	
	/*
		This is the sentinel terminating the full stop.
		
		Now that it's passed the test, invalidate it
		so it will fail for any subsequent full stop.
		
		(If we don't do this, the first sentinel will limit
		the range of every queued full stop, making the later ones
		no-ops and leaving the next sound playing without end
		(as well as leaving subsequent sound requests pending).)
	*/
	
	node->invalidate();
	
	return true;
}

queue_node* queue_get_next( rt_queue& queue )
{
	queue_node* node = queue.next;
	queue_node* tail = queue.tail;
	
	if ( ! node->valid() )
	{
		while ( ! node->valid()  &&  node != tail )
		{
			node = node->next;
		}
		
		queue.next = node;
	}
	
	return node;
}

void queue_advance( rt_queue& queue )
{
	queue_node* node = queue.next;
	
	node->invalidate();
	
	if ( node != queue.tail )
	{
		queue.next = node->next;
	}
}

void queue_cull_used( rt_queue& queue )
{
	queue_node* head = queue.head;
	queue_node* next = queue.next;
	
	while ( head != next )
	{
		queue_node* garbage = head;
		
		head = head->next;
		
		free_buffer( garbage );
	}
	
	queue.head = head;
}

rt_queue admin_queue;
rt_queue sound_queue;
