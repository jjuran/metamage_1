/*	========
 *	touch.cc
 *	========
 */

// Universal Interfaces
#include <Errors.h>

// MoreFiles
#include "MoreFilesExtras.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"
#include "SystemCalls.hh"


namespace N = Nitrogen;
namespace O = Orion;

using std::string;


static int TryHFSSoftTouch(const FSSpec& item);
static int TryHFSHeavyTouch(const FSSpec& item);

int O::Main(int argc, char const* const argv[])
{
	// Check for sufficient number of args
	if (argc < 2) {
		fprintf(stderr, "touch: missing arguments\n");
		return 1;
	}
	
	// Try to touch each item.  Return whether any errors occurred.
	int fail = 0;
	
	for ( int index = 1;  index < argc;  ++index )
	{
		FSSpec item;
		
		try
		{
			item = Path2FSS( argv[ index ] );
		}
		catch ( ... )
		{
			Io::Err << "touch: " << argv[ index ] << ": nonexistent directory in pathname\n";
			fail++;
		}
		
		if ( N::FSpTestItemExists( item ) )
		{
			fail += TryHFSSoftTouch( item );
		}
		else
		{
			fail += TryHFSHeavyTouch( item );
		}
	}
	
	return (fail == 0) ? 0 : 1;
}

int TryHFSSoftTouch(const FSSpec& item)
{
	OSErr err = FSpBumpDate(&item);
	if (err != noErr) {
		Io::Err << "touch: " << item.name << "FSpBumpDate error " << err << "\n";
		return 1;
	}
	return 0;
}

int TryHFSHeavyTouch(const FSSpec& item)
{
	OSErr err = FSpCreate(&item, 'R*ch', 'TEXT', smRoman);
	if (err != noErr) {
		Io::Err << "touch: " << item.name << "FSpCreate error " << err << "\n";
		return 1;
	}
	return 0;
}

