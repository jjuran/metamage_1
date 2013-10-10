/*
	os_thread_box.cc
	----------------
*/

#include "relix/api/os_thread_box.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __THREADS__
#include <Threads.h>
#endif

// mac-sys-utils
#include "mac_sys/current_thread_stack_space.hh"
#include "mac_sys/init_thread.hh"

// plus
#include "plus/ref_count.hh"

// recall
#include "recall/stack_frame.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace relix
{
	
	struct os_thread_param
	{
		os_thread_start_type  start;
		void*                 param;
		
		const void*   stack_bottom;
		const void*   stack_limit;
	};
	
	class os_thread : public plus::ref_count< os_thread >
	{
		private:
			typedef void* (*start_function_type)( void*, const void*, const void* );
			
			os_thread_param its_param;
			
			os_thread_id its_id;
			
			os_thread           ( const os_thread& );
			os_thread& operator=( const os_thread& );
		
		public:
			os_thread( start_function_type start, void* param, int stack_size );
			
			~os_thread();
			
			os_thread_id id() const  { return its_id; }
	};
	
	
	static void* measure_stack_limit()
	{
	#ifdef __MACOS__
		
		return (char*) recall::get_frame_pointer() - mac::sys::current_thread_stack_space();
		
	#endif
		
		return NULL;
	}
	
	
	static pascal void* ThreadEntry( void* param_ )
	{
		os_thread_param& param = *(os_thread_param*) param_;
		
		param.stack_bottom = mac::sys::init_thread();
		param.stack_limit  = measure_stack_limit();
		
		try
		{
			return param.start( param.param, param.stack_bottom, param.stack_limit );
		}
		catch ( ... )
		{
		}
		
		return NULL;
	}
	
	static ThreadEntryTPP GetThreadEntryFunction()
	{
		static ThreadEntryTPP upp = NewThreadEntryUPP( &ThreadEntry );
		
		return upp;
	}
	
	
	os_thread::os_thread( start_function_type start, void* param, int stack_size )
	{
		its_param.start = start;
		its_param.param = param;
		
		its_param.stack_bottom = NULL;
		its_param.stack_limit  = NULL;
		
		::Size size = 0;
		
		// Jaguar returns paramErr
		OSStatus err = ::GetDefaultThreadStackSize( kCooperativeThread, &size );
		
		if ( size > stack_size )
		{
			stack_size = size;
		}
		
		err = ::NewThread( kCooperativeThread,
		                   GetThreadEntryFunction(),
		                   &its_param,
		                   stack_size,
		                   0,
		                   NULL,
		                   &its_id );
		
		Mac::ThrowOSStatus( err );
	}
	
	os_thread_box::os_thread_box( os_thread& thread )
	:
		its_thread( &thread )
	{
	}
	
	os_thread::~os_thread()
	{
		::DisposeThread( id(), NULL, false );
	}
	
	
	os_thread_box::os_thread_box()
	{
	}
	
	os_thread_box::os_thread_box( const os_thread_box& that )
	:
		its_thread( that.its_thread )
	{
	}
	
	os_thread_box& os_thread_box::operator=( const os_thread_box& that )
	{
		/*
			The temporary is necessary in case the value prior to assignment
			is the only reference to the current thread.  In this case, the
			assignment will complete successfully and then the thread will
			terminate during the destruction of temp.
		*/
		
		os_thread_box temp = *this;
		
		its_thread = that.its_thread;
		
		return *this;
	}
	
	os_thread_box::~os_thread_box()
	{
		if ( get()  &&  intrusive_ptr_ref_count( get() ) == 1 )
		{
			::ThreadID thread;
			
			OSErr err = ::GetCurrentThread( &thread );
			
			if ( err == noErr  &&  thread == its_thread->id() )
			{
				boost::intrusive_ptr< os_thread > temp;
				
				temp.swap( its_thread );
				
				::operator delete( temp.get() );
				
				::DisposeThread( thread, NULL, false );
				
				// Not reached
			}
		}
	}
	
	void os_thread_box::swap( os_thread_box& that )
	{
		its_thread.swap( that.its_thread );
	}
	
	os_thread_box new_os_thread( os_thread_start_type start, void* param, int stack_size )
	{
		typedef boost::intrusive_ptr< os_thread > intrusive_ptr;
		
		return os_thread_box( *new os_thread( start, param, stack_size ) );
	}
	
	os_thread_id get_os_thread_id( const os_thread& thread )
	{
		return thread.id();
	}
	
}

