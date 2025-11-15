/*	=======
 *	Task.hh
 *	=======
 */

#ifndef ALINE_TASK_HH
#define ALINE_TASK_HH

// Standard C++
#include <vector>

// Standard C
#include <time.h>

// Debug
#include "debug/boost_assert.hh"

// vxo
#include "vxo/ref_count.hh"

// plus
#include "plus/string.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// A-line
#include "A-line/StrConVector.hh"
#include "A-line/StringVector.hh"


namespace tool
{
	
	class Task;
	
	typedef boost::intrusive_ptr< Task > TaskPtr;
	
	class Task : public vxo::ref_count< Task >
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
			
			virtual void Success()  {}
			
			virtual void Failure();
			
			void Run();
			
			void Complete();
	};
	
	class NullTask : public Task
	{
		public:
			void Start()  {}
	};
	
	class FileTask : public Task
	{
		private:
			plus::string its_output_path;
		
		public:
			FileTask( const plus::string& output ) : its_output_path( output )
			{
			}
			
			~FileTask();
			
			const plus::string& OutputPath() const  { return its_output_path; }
			
			time_t OutputStamp() const;
			
			virtual bool UpToDate();
			
			virtual void Make() = 0;
			
			void Start();
			
			void Success();
			void Failure();
			
			virtual void Return( bool succeeded )  {}
	};
	
	
	typedef StrConVector Command;
	
	class CommandTask : public FileTask
	{
		private:
			Command       its_command;
			plus::string  its_diagnostics_file_path;
			StringVector  its_input_file_paths;
		
		public:
			CommandTask( const Command&       command,
			             const plus::string&  output,
			             const plus::string&  diagnostics,
			             const plus::string  *input_begin,
			             const plus::string  *input_end );
			
			const Command& get_command() const  { return its_command; }
			
			const plus::string& get_diagnostics_file_path() const  { return its_diagnostics_file_path; }
			
			void Return( bool succeeded );
	};
	
	std::size_t CountFailures();
	
	void AddReadyTask( const TaskPtr& task );
	
	bool StartNextTask();
	
	bool RunNextTask();
	
}

#endif
