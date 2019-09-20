/*
	circular_queue.hh
	-----------------
*/

#ifndef CIRCULARQUEUE_HH
#define CIRCULARQUEUE_HH


namespace cthread {
namespace custom  {
	
	struct queue_element
	{
		queue_element* next;
	};
	
	class circular_queue
	{
		private:
			queue_element*  its_head;
			queue_element*  its_tail;
		
		public:
			circular_queue() : its_head(), its_tail()
			{
			}
			
			bool contains( queue_element* node ) const;
			
			void reset( queue_element* node );
			void prepend( queue_element* node );
			
			queue_element* behead();
			
			queue_element* head() const  { return its_head; }
			queue_element* tail() const  { return its_tail; }
			
			queue_element* next();
			
			void select( queue_element* node );
	};
	
}
}

#endif
