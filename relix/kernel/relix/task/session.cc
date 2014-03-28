/*
	session.cc
	----------
*/

#include "relix/task/session.hh"

// vfs
#include "vfs/filehandle.hh"


namespace relix
{
	
	session::session( int id ) : its_id( id )
	{
	}
	
	session::~session()
	{
	}
	
	void session::set_ctty( vfs::filehandle& terminal )
	{
		its_terminal = &terminal;
	}
	
}

