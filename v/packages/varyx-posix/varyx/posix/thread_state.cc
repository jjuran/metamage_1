/*
	thread_state.cc
	---------------
*/

#include "varyx/posix/thread_state.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// debug
#include "debug/assert.hh"

// vxo
#include "vxo/ptrvec.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/function-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/tracker.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/lambda.hh"
#include "vlib/types/string.hh"
#include "vlib/types/term.hh"

// varyx-posix
#include "varyx/posix/exception.hh"


#define THREAD_SELF_DESTRUCTION  "\n" "vx:   SELF-DESTRUCT CODE ACCEPTED" "\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace varyx
{
namespace posix
{
	
	namespace p7 = poseven;
	
	using namespace vlib;
	
	
	static vxo::UPtrVec< thread_state > joinable_threads;
	
	static p7::mutex joinable_threads_mutex;
	
	static
	void add_joinable_thread( thread_state* t )
	{
		add_root( t->function() );
		
		p7::lock k( joinable_threads_mutex );
		
		joinable_threads.push_back( t );
	}
	
	static
	void del_joinable_thread( thread_state* t )
	{
		del_root( t->function() );
		
		p7::lock k( joinable_threads_mutex );
		
		typedef vxo::UPtrVec< thread_state >::iterator Iter;
		
		const Iter begin = joinable_threads.begin();
		
		Iter it = joinable_threads.end();
		
		while ( it > begin )
		{
			--it;
			
			if ( *it == t )
			{
				*it = joinable_threads.back();
				
				joinable_threads.pop_back();
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
	void thread_error( int err )
	{
		Value exception = error_desc( err );
		
		throw_exception_object( exception );
	}
	
	void* thread_state::pthread_start( void* param )
	{
		thread_parameter_block& pb = *(thread_parameter_block*) param;
		
		try
		{
			pb.result.sym()->deref_unsafe() = call_function( pb.f, empty_list );
			pb.status = Thread_returned;
			
			return param;
		}
		catch ( const p7::thread_cancelled& )
		{
			pb.status = Thread_cancelled;
			
			throw;
		}
		catch ( const user_exception& e )
		{
			pb.result.sym()->deref_unsafe() = e.object;
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
		its_pb.result = Constant( "__thread_result" );
		its_pb.status = Thread_running;
		
		track_symbol( its_pb.result );
		
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
		if ( its_thread.is_the_current_thread() )
		{
			write( STDERR_FILENO, STR_LEN( THREAD_SELF_DESTRUCTION "\n" ) );
			
			ASSERT( ! its_thread.is_the_current_thread() );
			
			abort();  // in case ASSERT() is a no-op
		}
		
		try
		{
			its_thread.wait();
		}
		catch ( const p7::errno_t& err )
		{
			thread_error( err );
		}
		
		del_joinable_thread( this );
	}
	
	const Value& thread_state::operator*()
	{
		join();
		
		if ( its_pb.status == Thread_threw_exception )
		{
			throw user_exception( its_pb.result.sym()->get(), source_spec() );
		}
		
		return its_pb.result.sym()->get();
	}
	
}
}
