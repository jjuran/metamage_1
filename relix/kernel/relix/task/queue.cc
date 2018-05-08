/*
	queue.cc
	--------
*/

#include "relix/task/queue.hh"


#ifndef NULL
#define NULL  0L
#endif


namespace relix
{
	
	void queue::prepend( queue_element* that )
	{
		that->from = this;
		that->next = its_head;
		
		if ( its_head == NULL )
		{
			its_tail = that;
		}
		
		its_head = that;
	}
	
	void queue::append( queue_element* that )
	{
		that->from = this;
		that->next = NULL;
		
		if ( its_tail )
		{
			its_tail->next = that;
		}
		else
		{
			its_head = that;
		}
		
		its_tail = that;
	}
	
	void queue::remove( queue_element* that )
	{
		if ( its_head == that )
		{
			its_head = that->next;
			
			if ( that->next == NULL )
			{
				its_tail = NULL;
			}
			
			return;
		}
		
		queue_element* prev = its_head;
		
		while ( prev->next != that )
		{
			prev = prev->next;
			
			if ( prev == NULL )
			{
				return;
			}
		}
		
		prev->next = that->next;
		
		if ( its_tail == that )
		{
			its_tail = prev;
		}
	}
	
}
