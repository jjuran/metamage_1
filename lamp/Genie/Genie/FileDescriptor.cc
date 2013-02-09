/*	=================
 *	FileDescriptor.cc
 *	=================
 */

#include "Genie/FileDescriptor.hh"

// iota
#include "iota/swap.hh"

// vfs
#include "vfs/filehandle.hh"


namespace Genie
{
	
	FileDescriptor::FileDescriptor()
	:
		close_on_exec( false )
	{
	}
	
	FileDescriptor::FileDescriptor( const filehandle_ptr& handle )
	:
		handle       ( handle ),
		close_on_exec( false  )
	{
	}
	
	FileDescriptor::FileDescriptor( const FileDescriptor& other )
	:
		handle       ( other.handle      ),
		close_on_exec( other.close_on_exec )
	{
	}
	
	FileDescriptor::~FileDescriptor()
	{
	}
	
	FileDescriptor& FileDescriptor::operator=( const FileDescriptor& other )
	{
		handle        = other.handle;
		close_on_exec = other.close_on_exec;
		
		return *this;
	}
	
	void FileDescriptor::swap( FileDescriptor& other )
	{
		using iota::swap;
		
		swap( handle,        other.handle        );
		swap( close_on_exec, other.close_on_exec );
	}
	
}

