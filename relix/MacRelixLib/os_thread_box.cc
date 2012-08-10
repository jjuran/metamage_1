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

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

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
		
		os_thread_switch_type  switch_in;
		os_thread_switch_type  switch_out;
		
		const void*   stack_bottom;
		const void*   stack_limit;
	};
	
	class os_thread : public plus::ref_count< os_thread >
	{
		private:
			typedef void* (*start_function_type)( void*, const void*, const void* );
			
			typedef void (*switch_function_type)( void* );
			
			os_thread_param its_param;
			
			os_thread_id its_id;
			
			os_thread           ( const os_thread& );
			os_thread& operator=( const os_thread& );
		
		public:
			os_thread( start_function_type   start,
			           void*                 param,
			           int                   stack_size,
			           switch_function_type  switch_in,
			           switch_function_type  switch_out );
			
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
	
	
	static pascal void ThreadSwitchIn( ThreadID thread, void* param_ )
	{
		const os_thread_param& param = *(os_thread_param*) param_;
		
		if ( param.switch_in )
		{
			param.switch_in( param.param );
		}
	}
	
	static pascal void ThreadSwitchOut( ThreadID thread, void* param_ )
	{
		const os_thread_param& param = *(os_thread_param*) param_;
		
		if ( param.switch_out )
		{
			param.switch_out( param.param );
		}
	}
	
	os_thread::os_thread( start_function_type   start,
	                      void*                 param,
	                      int                   stack_size,
	                      switch_function_type  switch_in,
	                      switch_function_type  switch_out )
	{
		its_param.start = start;
		its_param.param = param;
		
		its_param.switch_in  = switch_in;
		its_param.switch_out = switch_out;
		
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
		
		static ThreadSwitchUPP switchIn  = NewThreadSwitchUPP( ThreadSwitchIn  );
		static ThreadSwitchUPP switchOut = NewThreadSwitchUPP( ThreadSwitchOut );
		
		if ( switch_in  )  SetThreadSwitcher( its_id, switchIn,  &its_param, true  );
		if ( switch_out )  SetThreadSwitcher( its_id, switchOut, &its_param, false );
	}
	
	os_thread::~os_thread()
	{
		::DisposeThread( id(), NULL, false );
	}
	
	
	os_thread_box::os_thread_box( os_thread& thread )
	:
		its_thread( &thread )
	{
		intrusive_ptr_add_ref( its_thread );
	}
	
	os_thread_box::os_thread_box( const os_thread_box& that )
	:
		its_thread( that.its_thread )
	{
		if ( its_thread )
		{
			intrusive_ptr_add_ref( its_thread );
		}
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
		
		if ( that.its_thread )
		{
			intrusive_ptr_add_ref( that.its_thread );
		}
		
		if ( its_thread )
		{
			intrusive_ptr_release( its_thread );
		}
		
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
				::operator delete( its_thread );
				
				::DisposeThread( thread, NULL, false );
				
				// Not reached
			}
		}
		
		if ( its_thread )
		{
			intrusive_ptr_release( its_thread );
		}
	}
	
	void os_thread_box::swap( os_thread_box& that )
	{
		os_thread* temp = its_thread;
		
		its_thread = that.its_thread;
		
		that.its_thread = temp;
	}
	
	os_thread_box new_os_thread( os_thread_start_type   start,
	                             void*                  param,
	                             int                    stack_size,
	                             os_thread_switch_type  switch_in,
	                             os_thread_switch_type  switch_out )
	{
		return os_thread_box( *new os_thread( start,
		                                      param,
		                                      stack_size,
		                                      switch_in,
		                                      switch_out ) );
	}
	
	os_thread_id get_os_thread_id( const os_thread& thread )
	{
		return thread.id();
	}
	
}

