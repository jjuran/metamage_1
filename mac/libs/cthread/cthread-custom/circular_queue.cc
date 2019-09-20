/*
	circular_queue.cc
	-----------------
*/

#include "circular_queue.hh"

// debug
#include "debug/assert.hh"


#ifndef NULL
#define NULL  0
#endif


namespace cthread {
namespace custom  {
	
	void circular_queue::reset( queue_element* node )
	{
		node->next = node;
		
		its_head = node;
		its_tail = node;
	}
	
	void circular_queue::prepend( queue_element* node )
	{
		ASSERT( its_tail );
		
		node->next = its_head;
		its_tail->next = node;
		
		its_head = node;
	}
	
	queue_element* circular_queue::behead()
	{
		ASSERT( its_head != its_tail );
		
		queue_element* head = its_head;
		queue_element* next = its_head->next;
		
		its_tail->next = next;
		its_head       = next;
		
		return head;
	}
	
	queue_element* circular_queue::next()
	{
		ASSERT( its_head != NULL );
		
		its_tail = its_head;
		its_head = its_head->next;
		
		return its_tail;
	}
	
	bool circular_queue::contains( queue_element* node ) const
	{
		queue_element* it = its_head;
		
		while ( it != its_tail )
		{
			if ( it == node )
			{
				return true;
			}
			
			it = it->next;
		}
		
		return it == node;
	}
	
	void circular_queue::select( queue_element* node )
	{
		ASSERT( contains( node ) );
		
		its_head = node->next;
		its_tail = node;
	}
	
}
}
