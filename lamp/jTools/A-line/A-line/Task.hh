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
			std::vector< TaskPtr >  itsDependents;
			time_t                  itsInputStamp;
		
		public:
			Task() : itsInputStamp()
			{
			}
			
			virtual ~Task()
			{
			}
			
			bool MoreRecent( time_t output_stamp ) const  { return output_stamp > itsInputStamp; }
			
			void UpdateInputStamp( time_t stamp );
			
			void AddDependent( const TaskPtr& task )  { itsDependents.push_back( task ); }
			
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
			std::string itsOutputPathname;
		
		public:
			FileTask( const std::string& output ) : itsOutputPathname( output )
			{
			}
			
			const std::string& OutputPathname() const  { return itsOutputPathname; }
			
			time_t OutputStamp() const;
			
			bool UpToDate() const;
			
			virtual void Make() = 0;
			
			void Main();
			
	};
	
	void AddReadyTask( const TaskPtr& task );
	
	bool RunNextTask();
	
}

#endif

