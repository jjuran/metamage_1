/*
	cthread-system.cc
	-----------------
*/

#include "cthread-system.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __THREADS__
#include <Threads.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// mac-sys-utils
#include "mac_sys/current_thread_stack_space.hh"
#include "mac_sys/gestalt.hh"
#include "mac_sys/init_thread.hh"

// cthread
#include "cthread/parameter_block.hh"
#include "cthread/types.hh"

// recall
#include "recall/stack_frame.hh"


#ifdef __APPLE__
#ifndef MAC_OS_X_VERSION_10_3
/*
	In Mac OS X 10.2, GetDefaultThreadStackSize() emits console log noise and
	returns an error, so just don't call it at all.
*/
#define GetDefaultThreadStackSize( style, size_out )  0
#endif
#endif

namespace cthread {
namespace system  {
	
	static
	void* measure_stack_limit()
	{
	#ifdef __MACOS__
		
		using recall::get_frame_pointer;
		using mac::sys::current_thread_stack_space;
		
		return (char*) get_frame_pointer() - current_thread_stack_space();
		
	#endif
		
		return NULL;
	}
	
	
	static
	pascal void* ThreadEntry( void* param )
	{
		parameter_block& pb = *(parameter_block*) param;
		
		pb.stack_bottom = mac::sys::init_thread();
		pb.stack_limit  = measure_stack_limit();
		
		try
		{
			return pb.start( pb.param, pb.stack_bottom, pb.stack_limit );
		}
		catch ( ... )
		{
		}
		
		return NULL;
	}
	
	static
	pascal void ThreadSwitchIn( ThreadID thread, void* param )
	{
		const parameter_block& pb = *(parameter_block*) param;
		
		if ( pb.switch_in )
		{
			pb.switch_in( pb.param );
		}
	}
	
	static
	pascal void ThreadSwitchOut( ThreadID thread, void* param )
	{
		const parameter_block& pb = *(parameter_block*) param;
		
		if ( pb.switch_out )
		{
			pb.switch_out( pb.param );
		}
	}
	
	thread_id create_thread( parameter_block& pb, unsigned stack_size )
	{
		DEFINE_TPP( ThreadEntry, ThreadEntry )
		
		DEFINE_TPP( ThreadSwitch, ThreadSwitchIn  )
		DEFINE_TPP( ThreadSwitch, ThreadSwitchOut )
		
		::Size size = 0;
		
		OSStatus err = GetDefaultThreadStackSize( kCooperativeThread, &size );
		
		if ( size > stack_size )
		{
			stack_size = size;
		}
		
		thread_id thread;
		
		err = ::NewThread( kCooperativeThread,
		                   TPP_ARG( ThreadEntry ),
		                   &pb,
		                   stack_size,
		                   0,
		                   NULL,
		                   &thread );
		
		Mac::ThrowOSStatus( err );
		
		enum { Out, In };
		
		if ( pb.switch_in )
		{
			::SetThreadSwitcher( thread, TPP_ARG( ThreadSwitchIn ), &pb, In );
		}
		
		if ( pb.switch_out )
		{
			::SetThreadSwitcher( thread, TPP_ARG( ThreadSwitchOut ), &pb, Out );
		}
		
		return thread;
	}
	
	void destroy_thread( thread_id id )
	{
		::DisposeThread( id, NULL, false );
	}
	
	thread_id current_thread()
	{
		::ThreadID thread;
		OSErr err = ::GetCurrentThread( &thread );
		
		return err != noErr ? kNoThreadID : thread;
	}
	
	unsigned long current_thread_stack_space()
	{
		return mac::sys::current_thread_stack_space();
	}
	
	bool is_thread_stopped( thread_id id )
	{
		::ThreadState state;
		
		OSErr err = ::GetThreadState( id, &state );
		
		return err == noErr  &&  state == kStoppedThreadState;
	}
	
	short stop_thread_nothrow( thread_id id )
	{
		return ::SetThreadState( id, kStoppedThreadState, kNoThreadID );
	}
	
	bool woken_thread( thread_id id )
	{
		return ::SetThreadState( id, kReadyThreadState, id ) == noErr;
	}
	
	void yield_to_any_thread()
	{
		// Ignore errors so we don't throw in critical sections
		(void) ::YieldToAnyThread();
	}
	
	void yield_to_thread( thread_id id )
	{
		// Ignore errors so we don't throw in critical sections
		(void) ::YieldToThread( id );
	}
	
}
}
