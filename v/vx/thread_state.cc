/*
	thread_state.cc
	---------------
*/

#include "thread_state.hh"

// Standard C
#include <string.h>

// Standard C++
#include <vector>

// poseven
#include "poseven/types/errno_t.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/function-utils.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/lambda.hh"
#include "vlib/types/string.hh"

// vx
#include "exception.hh"


namespace vlib
{
	
	namespace p7 = poseven;
	
	
	static std::vector< thread_state* > joinable_threads;
	
	static p7::mutex joinable_threads_mutex;
	
	static
	void add_joinable_thread( thread_state* t )
	{
		p7::lock k( joinable_threads_mutex );
		
		joinable_threads.push_back( t );
	}
	
	static
	void del_joinable_thread( thread_state* t )
	{
		p7::lock k( joinable_threads_mutex );
		
		typedef std::vector< thread_state* >::iterator Iter;
		
		const Iter begin = joinable_threads.begin();
		
		Iter it = joinable_threads.end();
		
		while ( it > begin )
		{
			--it;
			
			if ( *it == t )
			{
				joinable_threads.erase( it );
				
				break;
			}
		}
	}
	
	static inline
	thread_state* last_joinable_thread()
	{
		p7::lock k( joinable_threads_mutex );
		
		return joinable_threads.empty() ? NULL : joinable_threads.back();
	}
	
	void join_all_threads()
	{
		while ( thread_state* t = last_joinable_thread() )
		{
			t->join();
		}
	}
	
	static
	Value mapping( const plus::string& key, const Value& v )
	{
		return Value( String( key ), Op_mapping, v );
	}
	
	static
	void thread_error( int err )
	{
		Value error = mapping( "errno", Integer(           err   ) );
		Value desc  = mapping( "desc",  String ( strerror( err ) ) );
		
		Value exception( error, desc );
		
		throw_exception_object( exception );
	}
	
	void* thread_state::pthread_start( void* param )
	{
		thread_parameter_block& pb = *(thread_parameter_block*) param;
		
		try
		{
			pb.result = call_function( pb.f, empty_list );
			pb.status = Thread_returned;
			
			return param;
		}
		catch ( const user_exception& e )
		{
			pb.result = e.object;
		}
		catch ( ... )
		{
		}
		
		pb.status = Thread_threw_exception;
		
		return NULL;
	}
	
	thread_state::thread_state( const Value& f )
	{
		its_pb.f      = f;
		its_pb.status = Thread_running;
		
		if ( is_block( f ) )
		{
			its_pb.f = Lambda( f );  // Allow `return` in thread blocks.
		}
		
		try
		{
			its_thread.create( &pthread_start, &its_pb );
		}
		catch ( const p7::errno_t& err )
		{
			thread_error( err );
		}
		
		add_joinable_thread( this );
	}
	
	void thread_state::join()
	{
		if ( its_thread.exists() )
		{
			try
			{
				its_thread.join();
			}
			catch ( const p7::errno_t& err )
			{
				thread_error( err );
			}
			
			del_joinable_thread( this );
		}
	}
	
	const Value& thread_state::operator*()
	{
		join();
		
		if ( its_pb.status == Thread_threw_exception )
		{
			throw user_exception( its_pb.result, source_spec() );
		}
		
		return its_pb.result;
	}
	
}
