/*
	process.cc
	----------
*/

#include "relix/task/process.hh"

// relix
#include "relix/task/process_group.hh"


namespace relix
{
	
	process::process( int id, int ppid, process_group& pg )
	:
		its_id           ( id   ),
		its_ppid         ( ppid ),
		its_process_group( &pg  )
	{
	}
	
	process::~process()
	{
	}
	
	process_group& process::get_process_group() const
	{
		return *its_process_group;
	}
	
	void process::set_process_group( process_group& pg )
	{
		its_process_group = &pg;
	}
	
}

