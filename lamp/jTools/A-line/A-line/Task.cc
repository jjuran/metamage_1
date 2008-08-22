/*	=======
 *	Task.cc
 *	=======
 */

#include "A-line/Task.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <queue>

// MoreFunctional
#include "PointerToFunction.hh"


namespace ALine
{
	
	static std::queue< TaskPtr > gReadyTasks;
	static std::vector< TaskPtr > gFailedTasks;
	
	
	inline void UpdateTaskInputStamp( const TaskPtr& task, time_t stamp )
	{
		task->UpdateInputStamp( stamp );
	}
	
	static void CheckIfTaskIsReady( const TaskPtr& task )
	{
		if ( task.unique() )
		{
			gReadyTasks.push( task );
		}
	}
	
	void Task::UpdateInputStamp( time_t stamp )
	{
		if ( stamp > itsInputStamp )
		{
			itsInputStamp = stamp;
		}
	}
	
	void Task::Run()
	{
		Main();
		
		Complete( itsInputStamp );
	}
	
	void Task::Complete( time_t stamp )
	{
		std::for_each( itsDependents.begin(),
		               itsDependents.end(),
		               std::bind2nd( more::ptr_fun( UpdateTaskInputStamp ),
		                             stamp ) );
		
		std::for_each( itsDependents.begin(),
		               itsDependents.end(),
		               std::ptr_fun( CheckIfTaskIsReady ) );
		
		itsDependents.clear();
	}
	
	void AddReadyTask( const TaskPtr& task )
	{
		gReadyTasks.push( task );
	}
	
	bool RunNextTask()
	{
		if ( gReadyTasks.empty() )
		{
			return false;
		}
		
		TaskPtr task = gReadyTasks.front();
		
		gReadyTasks.pop();
		
		task->Run();
		
		return true;
	}
	
}

