/*	=====
 *	mv.cc
 *	=====
 */

// MoreFiles
#include "MoreFilesExtras.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Str.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"
#include "SystemCalls.hh"


namespace N = Nitrogen;
namespace O = Orion;


static bool SameFile( const FSSpec& a, const FSSpec& b )
{
	return a.vRefNum == b.vRefNum
	    && a.parID   == b.parID
	    && ::EqualString( a.name, b.name, false, true );
}

static int TryHFSMoveRename( const FSSpec& source, const FSSpec& dest );
	
int O::Main( int argc, const char *const argv[] )
{
	// Check for sufficient number of args
	if (argc < 3)
	{
		Io::Err << "mv: missing " << ((argc == 1) ? "file arguments" : "destination file") << "\n";
		return 1;
	}
	
	int fail = 0;
	
	if ( argc > 3 )
	{
		// Move multiple files.
		
		N::FSDirSpec dir;
		try
		{
			// Conversion to FSDirSpec will throw if item is not a dir
			dir = N::Convert< N::FSDirSpec >( Path2FSS( argv[ argc - 1 ] ) );
		}
		catch ( ... )
		{
			Io::Err << "mv: moving multiple files, but last argument (" << argv[argc - 1] << ") is not a directory.\n";
			return 1;
		}
		
		for ( int index = 1;  index < argc - 1;  ++index )
		{
			try
			{
				FSSpec source = Path2FSS( argv[ index ] );
				fail += TryHFSMoveRename( source, dir & source.name );
			}
			catch ( ... )
			{
				fail++;
			}
		}
	}
	else
	{
		// Move single file or directory.
		FSSpec source;
		try
		{
			source = Path2FSS( argv[ 1 ] );
		}
		catch ( N::OSStatus err )
		{
			Io::Err << "mv: cannot stat `" << argv[ 1 ] << "': No such file or directory " << err << "\n";
			return 1;
		}
		
		try
		{
			FSSpec dest = Path2FSS( argv[ 2 ] );
			
			if ( N::FSpTestDirectoryExists( dest ) )
			{
				// Move this -> that/this
				fail = TryHFSMoveRename(source, N::Convert< N::FSDirSpec >( dest ) & source.name);
			}
			else
			{
				// Move this -> that
				fail = TryHFSMoveRename(source, dest);
			}
		}
		catch ( ... )
		{
			
		}
	}
	
	return fail != 0;
}

/* This code may not be complete */
/*
	srcVol, srcDir, and srcName specify a source item.
	destDir and destName specify a destination directory on the same volume.
	Our mission:
		If no item named destName exists in destDir, move/rename srcName to destDir/destName.
		Otherwise, if srcName in srcDir and destName in destDir are both files,
			then delete destName and move/rename srcName, as above.
		Otherwise, report an error and return.
*/

int TryHFSMoveRename(
	const FSSpec& source, 
	const FSSpec& dest)
{
	if (source.vRefNum != dest.vRefNum) {
		Io::Err << "mv: can't move across partitions\n";
		return 1;
	}
	
	OSErr err = noErr;
	
	if ( N::FSpTestItemExists( dest ) )
	{
		// Dest file exists
		if ( SameFile( source, dest ) )
		{
			// Source and destination are the same file!
			Io::Err << "mv: `" << source.name << "' and `" << dest.name << "' are the same file\n";
			return 1;
		}
		else
		{
			// Distinct dest item exists.  Delete it if both src and dest are files.
			if ( N::FSpTestDirectoryExists( dest ) )
			{
				Io::Err << "mv: " << dest.name << ": Can't replace a directory"  "\n";
				return 1;
			}
			else if ( N::FSpTestDirectoryExists( source ) )
			{
				Io::Err << "mv: " << dest.name << ": Can't replace a file with a directory"  "\n";
				return 1;
			}
			
			err = FSpDelete(&dest);
			if (err != noErr)
			{
				Io::Err << "mv: " << dest.name << ": FSpDelete error " << err << "\n";
			}
		}
	}
	
	bool renaming = N::Convert< std::string >( source.name )
	             != N::Convert< std::string >( dest.name   );
	
	FSSpec dir = N::Convert< FSSpec >( N::FSpGetParent( dest ) );
	err = FSpMoveRenameCompat( &source, &dir, renaming ? dest.name : NULL );
	
	if (err != noErr)
	{
		Io::Err << "mv: " << source.name << ": FSpMoveRenameCompat error " << err << "\n";
		return 1;
	}
	
	return 0;
}
