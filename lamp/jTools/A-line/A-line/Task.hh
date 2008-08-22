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
			
			void UpdateInputStamp( time_t stamp );
			
			void AddDependent( const TaskPtr& task )  { itsDependents.push_back( task ); }
			
			virtual void Main() = 0;
			
			void Complete( time_t stamp );
	};
	
	class NullTask : public Task
	{
		public:
			void Main()  {}
	};
	
}

#endif

