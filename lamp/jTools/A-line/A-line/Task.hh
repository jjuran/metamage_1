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
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

// POSeven
#include "POSeven/types/wait_t.hh"


namespace tool
{
	
	class Task;
	
	typedef boost::shared_ptr< Task > TaskPtr;
	
	class Task : public boost::enable_shared_from_this< Task >
	{
		private:
			std::vector< TaskPtr >  its_dependents;
			time_t                  its_input_stamp;
		
		public:
			Task() : its_input_stamp()
			{
			}
			
			virtual ~Task();
			
			bool MoreRecent( time_t output_stamp ) const  { return output_stamp > its_input_stamp; }
			
			void UpdateInputStamp( time_t stamp );
			
			void AddDependent( const TaskPtr& task )  { its_dependents.push_back( task ); }
			
			virtual void Start() = 0;
			
			virtual void Finish()  { Complete(); }
			
			virtual void Return( poseven::wait_t wait_status )  { Finish(); }
			
			void Run();
			
			void Complete();
	};
	
	class NullTask : public Task
	{
		public:
			void Start()  { Complete(); }
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
			
			void Start();
			void Finish();
			
	};
	
	
	typedef std::vector< const char* > Command;
	
	class CommandTask : public FileTask
	{
		private:
			Command                     its_command;
			std::string                 its_diagnostics_file_path;
			std::vector< std::string >  its_input_file_paths;
		
		public:
			CommandTask( const Command&      command,
			             const std::string&  output,
			             const std::string&  diagnostics,
			             const std::string  *input_begin,
			             const std::string  *input_end );
			
			const Command& get_command() const  { return its_command; }
			
			const std::string& get_diagnostics_file_path() const  { return its_diagnostics_file_path; }
			
			void Return( poseven::wait_t wait_status );
	};
	
	void AddReadyTask( const TaskPtr& task );
	
	bool StartNextTask();
	
	bool RunNextTask();
	
}

#endif

