/*
	queues.hh
	---------
*/

#ifndef QUEUES_HH
#define QUEUES_HH


struct queue_node
{
	queue_node*  next;
	int          size;  // Any non-zero value indicates validity
	
	bool valid() const  { return size != 0; }
	
	void invalidate()  { size = 0; }
};

struct rt_queue;

typedef bool (*node_predicate)( const queue_node* node, const void* data );

queue_node* queue_alloc_node( unsigned minimum_node_size );

void queue_append( rt_queue& queue, queue_node* node );

queue_node* find( rt_queue& queue, node_predicate test, const void* data );

bool void_until( rt_queue& queue, node_predicate test, const void* data );

queue_node* queue_get_next( rt_queue& queue );

void queue_advance( rt_queue& queue );

void queue_cull_used( rt_queue& queue );

extern rt_queue admin_queue;
extern rt_queue sound_queue;

#endif
