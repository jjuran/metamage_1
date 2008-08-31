/*	=======
 *	Task.hh
 *	=======
 */

#ifndef ALINE_TASK_HH
#define ALINE_TASK_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Standard C++
#include <string>
#include <vector>

// Standard C
#include <time.h>

// Boost
#include <boost/shared_ptr.hpp>


namespace ALine
{
	
	class Task;
	
	typedef boost::shared_ptr< Task > TaskPtr;
	
	class Task
	{
		private:
			std::vector< TaskPtr >  its_dependents;
			time_t                  its_input_stamp;
		
		public:
			Task() : its_input_stamp()
			{
			}
			
			virtual ~Task()
			{
			}
			
			bool MoreRecent( time_t output_stamp ) const  { return output_stamp > its_input_stamp; }
			
			void UpdateInputStamp( time_t stamp );
			
			void AddDependent( const TaskPtr& task )  { its_dependents.push_back( task ); }
			
			virtual void Main() = 0;
			
			void Run();
			
			void Complete( time_t stamp );
	};
	
	class NullTask : public Task
	{
		public:
			void Main()  {}
	};
	
	class FileTask : public Task
	{
		private:
			std::string its_output_path;
		
		public:
			FileTask( const std::string& output ) : its_output_path( output )
			{
			}
			
			const std::string& OutputPath() const  { return its_output_path; }
			
			time_t OutputStamp() const;
			
			virtual bool UpToDate();
			
			virtual void Make() = 0;
			
			void Main();
			
	};
	
	void AddReadyTask( const TaskPtr& task );
	
	bool RunNextTask();
	
}

#endif

