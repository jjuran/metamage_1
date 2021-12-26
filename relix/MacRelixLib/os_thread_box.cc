/*
	os_thread_box.cc
	----------------
*/

#include "relix/api/os_thread_box.hh"

// cthread
#include "cthread/parameter_block.hh"

// cthread-either
#include "cthread-either.hh"

// vxo
#include "vxo/ref_count.hh"


namespace relix
{
	
	using namespace cthread::either;
	
	using cthread::parameter_block;
	
	
	class os_thread : public vxo::ref_count< os_thread >
	{
		private:
			parameter_block its_param;
			
			os_thread_id its_id;
			
			os_thread           ( const os_thread& );
			os_thread& operator=( const os_thread& );
		
		public:
			os_thread( cthread::start_proc   start,
			           void*                 param,
			           int                   stack_size,
			           cthread::switch_proc  switch_in,
			           cthread::switch_proc  switch_out );
			
			~os_thread();
			
			os_thread_id id() const  { return its_id; }
			
			void set_param( void* param )  { its_param.param = param; }
	};
	
	void set_os_thread_param( os_thread& thread, void* param )
	{
		thread.set_param( param );
	}
	
	
	os_thread::os_thread( cthread::start_proc   start,
	                      void*                 param,
	                      int                   stack_size,
	                      cthread::switch_proc  switch_in,
	                      cthread::switch_proc  switch_out )
	{
		its_param.start = start;
		its_param.param = param;
		
		its_param.switch_in  = switch_in;
		its_param.switch_out = switch_out;
		
		its_id = create_thread( its_param, stack_size );
	}
	
	os_thread::~os_thread()
	{
		destroy_thread( id() );
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
			os_thread_id thread = current_thread();
			
			if ( thread == its_thread->id() )
			{
				::operator delete( its_thread );
				
				destroy_thread( thread );
				
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
	
	os_thread_box new_os_thread( cthread::start_proc   start,
	                             void*                 param,
	                             int                   stack_size,
	                             cthread::switch_proc  switch_in,
	                             cthread::switch_proc  switch_out )
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
