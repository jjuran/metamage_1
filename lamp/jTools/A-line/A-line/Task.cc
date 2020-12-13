/*	=======
 *	Task.cc
 *	=======
 */

#include "A-line/Task.hh"

// Standard C++
#include <queue>

// poseven
#include "poseven/functions/stat.hh"

// pfiles
#include "pfiles/common.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/Commands.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	using namespace io::path_descent_operators;
	
	
	static std::queue< TaskPtr > gReadyTasks;
	static std::vector< TaskPtr > gFailedTasks;
	
	
	static inline void UpdateTaskInputStamp( const TaskPtr& task, time_t stamp )
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
		for ( size_t i = 0;  i < its_dependents.size();  ++i )
		{
			UpdateTaskInputStamp( its_dependents[ i ], its_input_stamp );
		}
		
		for ( size_t i = 0;  i < its_dependents.size();  ++i )
		{
			CheckIfTaskIsReady( its_dependents[ i ] );
		}
		
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
	
	
	CommandTask::CommandTask( const Command&       command,
			                  const plus::string&  output,
			                  const plus::string&  diagnostics,
			                  const plus::string  *input_begin,
			                  const plus::string  *input_end )
	: FileTask( output ),
	  its_command( command ),
	  its_diagnostics_file_path( diagnostics ),
	  its_input_file_paths()
	{
		its_input_file_paths.insert( its_input_file_paths.begin(), input_begin, input_end );
		
		its_command.push_back( OutputPath().c_str() );
		
		AugmentCommand( its_command, its_input_file_paths );
		
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
