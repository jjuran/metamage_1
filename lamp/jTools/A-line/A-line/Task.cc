/*	=======
 *	Task.cc
 *	=======
 */

#include "A-line/Task.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <queue>

// plus
#include "plus/pointer_to_function.hh"

// poseven
#include "poseven/Pathnames.hh"
#include "poseven/functions/stat.hh"

// A-line
#include "A-line/A-line.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	using namespace io::path_descent_operators;
	
	
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
	
	Task::~Task()
	{
		Complete();
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
		Start();
		
		Success();
	}
	
	void Task::Complete()
	{
		std::for_each( its_dependents.begin(),
		               its_dependents.end(),
		               std::bind2nd( plus::ptr_fun( UpdateTaskInputStamp ),
		                             its_input_stamp ) );
		
		std::for_each( its_dependents.begin(),
		               its_dependents.end(),
		               std::ptr_fun( CheckIfTaskIsReady ) );
		
		its_dependents.clear();
	}
	
	void Task::Failure()
	{
		// Save the task for later reporting and to tie up its dependents
		gFailedTasks.push_back( shared_from_this() );
	}
	
	
	time_t FileTask::OutputStamp() const
	{
		struct stat output_stat;
		
		const bool output_exists = p7::stat( its_output_path, output_stat );
		
		if ( output_exists )
		{
			return output_stat.st_mtime;
		}
		
		return 0;
	}
	
	bool FileTask::UpToDate()
	{
		return MoreRecent( OutputStamp() );
	}
	
	void FileTask::Start()
	{
		// If the output file exists and it's up to date, we can skip this.
		
		if ( UpToDate() )
		{
			return;
		}
		
		Make();
	}
	
	FileTask::~FileTask()
	{
	}
	
	void FileTask::Success()
	{
		UpdateInputStamp( p7::stat( its_output_path ).st_mtime );
		
		Return( true );
	}
	
	void FileTask::Failure()
	{
		Task::Failure();
		
		// FIXME: Just bump the date back
		(void) ::unlink( its_output_path.c_str() );
		
		Return( false );
	}
	
	
	static const char* c_str( const std::string& s )
	{
		return s.c_str();
	}
	
	
	CommandTask::CommandTask( const Command&      command,
			                  const std::string&  output,
			                  const std::string&  diagnostics,
			                  const std::string  *input_begin,
			                  const std::string  *input_end )
	: FileTask( output ),
	  its_command( command ),
	  its_diagnostics_file_path( diagnostics ),
	  its_input_file_paths( input_begin, input_end )
	{
		its_command.reserve( command.size() + 2 + input_end - input_begin );
		
		its_command.push_back( OutputPath().c_str() );
		
		std::transform( its_input_file_paths.begin(),
		                its_input_file_paths.end(),
		                std::back_inserter( its_command ),
		                std::ptr_fun( c_str ));
		
		its_command.push_back( NULL );
	}
	
	void CommandTask::Return( bool succeeded )
	{
		check_diagnostics( succeeded, its_diagnostics_file_path.c_str() );
	}
	
	
	std::size_t CountFailures()
	{
		return gFailedTasks.size();
	}
	
	void AddReadyTask( const TaskPtr& task )
	{
		gReadyTasks.push( task );
	}
	
	bool StartNextTask()
	{
		if ( gReadyTasks.empty() )
		{
			return false;
		}
		
		TaskPtr task = gReadyTasks.front();
		
		gReadyTasks.pop();
		
		task->Start();
		
		return true;
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

