/*	=========
 *	FSTree.cc
 *	=========
 */

#include "Genie/FileSystem/FSTree.hh"

// POSIX
#include "fcntl.h"

// Nucleus
//#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	
	void FSTree::ChangeMode( mode_t mode ) const
	{
		P7::ThrowErrno( EPERM );
	}
	
	void FSTree::Delete() const
	{
		P7::ThrowErrno( EPERM );
	}
	
	
	boost::shared_ptr< IOHandle > FSTree::Open( OpenFlags flags, mode_t mode ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		if ( creating && excluding )
		{
			P7::ThrowErrno( EPERM );
		}
		
		return Open( flags );
	}
	
	boost::shared_ptr< IOHandle > FSTree::Open( OpenFlags flags ) const
	{
		P7::ThrowErrno( ENOENT );  // Assume read attempt if no mode
		
		// Not reached
		return boost::shared_ptr< IOHandle >();
	}
	
	void FSTree::Exec( const char* const argv[], const char* const envp[] ) const
	{
		P7::ThrowErrno( ENOEXEC );
	}
	
	
	void FSTree::CreateDirectory( mode_t mode ) const
	{
		P7::ThrowErrno( EPERM );
	}
	
	FSTreePtr FSTree::Lookup( const std::string& name ) const
	{
		P7::ThrowErrno( ENOENT );
		
		// Not reached
		return FSTreePtr();
	}
	
	FSIteratorPtr FSTree::Iterate() const
	{
		P7::ThrowErrno( ENOENT );
		
		// Not reached
		return FSIteratorPtr();
	}
	
}

