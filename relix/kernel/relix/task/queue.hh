/*
	queue.hh
	--------
*/

#ifndef RELIX_TASK_QUEUE_HH
#define RELIX_TASK_QUEUE_HH


namespace relix
{
	
	class queue;
	
	struct queue_element
	{
		queue_element*  next;
		queue*          from;
		
		queue_element() : from()
		{
		}
	};
	
	class queue
	{
		private:
			queue_element* its_head;
			queue_element* its_tail;
			
			// non-copyable
			queue           ( const queue& );
			queue& operator=( const queue& );
		
		public:
			queue() : its_head(), its_tail()
			{
			}
			
			queue_element* get() const  { return its_head; }
			
			bool empty() const  { return ! get(); }
			
			void prepend( queue_element* that );
			void append ( queue_element* that );
			
			void remove( queue_element* that );
	};
	
}

#endif
