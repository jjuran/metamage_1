/*	=====
 *	cp.cc
 *	=====
 */

#include "errno.h"

// Standard C/C++
#include <cstring>

// MoreFiles
#include "FileCopy.h"

// Nitrogen Nucleus
#include "Nucleus/TheExceptionBeingHandled.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// POSeven
#include "POSeven/Errno.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"
#include "SystemCalls.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace P7 = POSeven;
namespace O = Orion;


namespace
{
	
	void FSpFileCopy( const FSSpec&         source,
	                  const FSSpec&         destDir,
	                  const unsigned char*  copyName       = NULL,
	                  void*                 copyBufferPtr  = NULL,
	                  long                  copyBufferSize = 0,
	                  bool                  preflight      = true )
	{
		(void) N::FileManagerErrorsRegistrationDependency();
		
		N::ThrowOSStatus( ::FSpFileCopy( &source,
		                                 &destDir,
		                                 copyName,
		                                 copyBufferPtr,
		                                 copyBufferSize,
		                                 preflight ) );
	}
	
}

static bool SameFile( const FSSpec& a, const FSSpec& b )
{
	return a.vRefNum == b.vRefNum
	    && a.parID   == b.parID
	    && ::EqualString( a.name, b.name, false, true );
}

static int TryHFSCopy(const FSSpec& source, const FSSpec& dest);
	
int O::Main(int argc, const char *const argv[])
{
	int fail = 0;
	
	// Check for sufficient number of args
	if (argc < 3)
	{
		Io::Err << "cp: missing " << ((argc == 1) ? "file arguments" : "destination file") << "\n";
		return 1;
	}
	
	if (argc > 3)
	{
		// Copy multiple files.
		
		// Last arg should be the destination directory.
		N::FSDirSpec dir;
		try
		{
			// Conversion to FSDirSpec will throw if item is not a dir
			dir = NN::Convert< N::FSDirSpec >( Path2FSS( argv[ argc - 1 ] ) );
		}
		catch ( ... )
		{
			Io::Err << "cp: copying multiple files, but last argument (" << argv[argc - 1] << ") is not a directory.\n";
			return 1;
		}
		
		// Try to copy each file.  Return whether any errors occurred.
		for ( int index = 1;  index < argc - 1;  ++index )
		{
			try
			{
				FSSpec source = Path2FSS(argv[index]);
				fail += TryHFSCopy(source, dir & source.name);
			}
			catch (...)
			{
				fail++;
			}
		}
	}
	else
	{
		// Copy single file.
		
		FSSpec source;
		try
		{
			source = Path2FSS(argv[1]);
		}
		catch ( N::OSStatus err )
		{
			Io::Err << "cp: cannot stat `" << argv[1] << "': No such file or directory " << err << "\n";
			return 1;
		}
		
		try
		{
			FSSpec dest = Path2FSS(argv[2]);
			if ( N::FSpTestDirectoryExists( dest ) )
			{
				// Copy this -> that/this
				fail = TryHFSCopy(source, NN::Convert< N::FSDirSpec >( dest ) & source.name);
			}
			else
			{
				// Copy this -> that
				fail = TryHFSCopy(source, dest);
			}
		}
		catch (...)
		{
			
		}
	}
	
	return fail;
}


static void RegisterMacToUnixErrorConversions()
{
	NN::RegisterExceptionConversion< P7::Errno, N::OSStatus >();
}

int TryHFSCopy(const FSSpec& source, const FSSpec& dest)
{
	RegisterMacToUnixErrorConversions();
	
	if ( N::FSpTestDirectoryExists( source ) )
	{
		// Source item is a directory.
		Io::Err << "cp: " << source.name << ": omitting directory\n";
		return 1;
	}
	
	if ( N::FSpTestItemExists( dest ) )
	{
		// Dest file exists
		if ( SameFile( source, dest ) )
		{
			// Source and destination are the same file!
			Io::Err << "cp: `" << source.name << "' and `" << dest.name << "' are the same file\n";
			return 1;
		}
		else if ( N::FSpTestDirectoryExists( dest ) )
		{
			Io::Err << "cp: " << dest.name << ": can't replace a directory " << "\n";
		}
		else
		{
			// Distinct dest file exists.  Delete it.
			try
			{
				N::FSpDelete( dest );
			}
			catch ( ... )
			{
				P7::Errno errnum = NN::Convert< P7::Errno >( NN::TheExceptionBeingHandled() );
				
				if ( errnum == -1 )
				{
					N::OSStatus err = NN::Convert< N::OSStatus >( NN::TheExceptionBeingHandled() );
					
					Io::Err << "cp: " << dest.name << ": " << err << "\n";
				}
				else
				{
					Io::Err << "cp: " << dest.name << ": " << std::strerror( errnum ) << "\n";
				}
			}
		}
	}
	
	try
	{
		FSpFileCopy( source, NN::Convert< FSSpec >( N::FSpGetParent( dest ) ), dest.name );
	}
	catch ( ... )
	{
		P7::Errno errnum = NN::Convert< P7::Errno >( NN::TheExceptionBeingHandled() );
		
		if ( errnum == -1 )
		{
			N::OSStatus err = NN::Convert< N::OSStatus >( NN::TheExceptionBeingHandled() );
			
			Io::Err << "cp: " << source.name << ": " << err << "\n";
		}
		else
		{
			Io::Err << "cp: " << source.name << ": " << std::strerror( errnum ) << "\n";
		}
		
		return 1;
	}
	return 0;
}

