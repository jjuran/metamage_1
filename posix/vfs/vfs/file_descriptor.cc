/*
	file_descriptor.cc
	------------------
*/

#include "vfs/file_descriptor.hh"

// iota
#include "iota/swap.hh"

// vfs
#include "vfs/filehandle.hh"


namespace vfs
{
	
	file_descriptor::file_descriptor()
	:
		close_on_exec( false )
	{
	}
	
	file_descriptor::file_descriptor( const filehandle_ptr& handle )
	:
		handle       ( handle ),
		close_on_exec( false  )
	{
	}
	
	file_descriptor::file_descriptor( const file_descriptor& other )
	:
		handle       ( other.handle      ),
		close_on_exec( other.close_on_exec )
	{
	}
	
	file_descriptor::~file_descriptor()
	{
	}
	
	file_descriptor& file_descriptor::operator=( const file_descriptor& other )
	{
		handle        = other.handle;
		close_on_exec = other.close_on_exec;
		
		return *this;
	}
	
	void file_descriptor::swap( file_descriptor& other )
	{
		using iota::swap;
		
		swap( handle,        other.handle        );
		swap( close_on_exec, other.close_on_exec );
	}
	
}

