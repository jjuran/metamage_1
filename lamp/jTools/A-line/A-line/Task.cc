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

// A-line
#include "A-line/BuildCommon.hh"


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
		if ( stamp > its_input_stamp )
		{
			its_input_stamp = stamp;
		}
	}
	
	void Task::Run()
	{
		Main();
		
		Complete( its_input_stamp );
	}
	
	void Task::Complete( time_t stamp )
	{
		std::for_each( its_dependents.begin(),
		               its_dependents.end(),
		               std::bind2nd( more::ptr_fun( UpdateTaskInputStamp ),
		                             stamp ) );
		
		std::for_each( its_dependents.begin(),
		               its_dependents.end(),
		               std::ptr_fun( CheckIfTaskIsReady ) );
		
		its_dependents.clear();
	}
	
	
	time_t FileTask::OutputStamp() const
	{
		if ( io::file_exists( its_output_pathname ) )
		{
			return ModifiedDate( its_output_pathname );
		}
		
		return 0;
	}
	
	bool FileTask::UpToDate()
	{
		return MoreRecent( OutputStamp() );
	}
	
	void FileTask::Main()
	{
		// If the output file exists and it's up to date, we can skip this.
		
		if ( UpToDate() )
		{
			return;
		}
		
		Make();
		
		UpdateInputStamp( ModifiedDate( its_output_pathname ) );
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

