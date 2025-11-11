/*
	test-FSRef-to-FSSpec-name.cc
	----------------------------
	
	The results below are incomplete.  If you have on hand
	a version of Mac OS X for which test results are missing
	(especially OS X 10.10 or 10.11), you can help:
	
		git clone $REPOSITORY_ADDRESS
		
		cd metamage_1
		
		./build.pl -i th
		./build.pl -i -N test-FSRef-to-FSSpec-name
		
		var/bin/th var/bin/test-FSRef-to-FSSpec-name
	
	See <https://www.jjuran.org/> for contact.  (Email and IRC preferred.)
	
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdio.h>

// chars
#include "conv/mac_utf8.hh"

// mac-relix-utils
#include "mac_relix/FSRef_from_path.hh"


#pragma exceptions off


#define PREFIX  "test-FSRef-to-FSSpec-name: "

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ERROR( e, msg )  (wrote( 2, STR_LEN( PREFIX msg "\n" ) ) ? e : 13)


/*
	The underscore in the filename will be replaced
	with each non-ASCII MacRoman character in turn.
*/

static char hfs_filename[] = "xxx_xxx (Delete me)";

/*
	What we're testing is this:  Does the single non-ASCII
	character in the MacRoman filename survive round-trip
	conversion to UTF-8 (by way of Metamage library code)
	and back (via FSRef APIs)?  Mostly, it does. ...
	
	... Mostly.
	
	Of the 128 non-ASCII characters in MacRoman, for 127 of
	them, the answer is an unqualified yes.  There's just one
	exception -- the em dash.  In MacRoman, the em dash codes
	as the byte $"d1".  But in certain versions of Mac OS X,
	the conversion from FSRef to FSSpec yields two bytes for
	the em dash: $"81 5c", which is just plain silly.
	
	(For reference, the em dash is $"e2 80 94" in UTF-8.)
	
	This test exists to determine which versions those are.
	
	Legend:
		XX	Positive result observed (bug is present)
		==	Negative result observed (bug is absent)
		-	No observation, negative result implied by adjacent results
		+	No observation, positive result implied by adjacent results
		?	No observation, result is unknown
	
	                PPC	x86
	Mac OS X 10.0:	-
	Mac OS X 10.1:	-
	Mac OS X 10.2:	==
	Mac OS X 10.3:	XX
	Mac OS X 10.4:	+	+
	Mac OS X 10.5:	XX	+
	Mac OS X 10.6:		XX
	Mac OS X 10.7:		XX
	    OS X 10.8:		+
	    OS X 10.9:		XX
	    OS X 10.10:		?
	    OS X 10.11:		?
	 macOS   10.12:		==
	 macOS   10.13:		==
	 macOS   10.14:		-
	
	The results in Mac OS X 10.2 and 10.3 are consistent between
	CFM and Mach-O.  Running in Classic yields negative results.
	
	As the functionality under test involves an FSSpec API,
	it only exists in 32-bit platforms.  Thus, testing ends
	after macOS 10.14 (and Apple Silicon is right out).
*/

static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

static
bool run_test( Byte c )
{
	const int hfs_filename_len = sizeof hfs_filename - 1;
	
	hfs_filename[ 3 ] = c;
	
	char utf8_filename[ sizeof hfs_filename + 2 ];
	
	size_t n = conv::utf8_from_mac( utf8_filename,
	                                sizeof utf8_filename,
	                                hfs_filename,
	                                hfs_filename_len );
	
	utf8_filename[ n ] = '\0';
	
	int fd = open( utf8_filename, O_RDWR | O_CREAT, 0666 );
	
	if ( fd < 0 )
	{
		_exit( ERROR( 30, "can't open test file" ) );
	}
	
	close( fd );
	
	FSRef  ref;
	FSSpec spec;
	
	int err = mac::relix::FSRef_from_path( utf8_filename, ref );
	
	if ( err )
	{
		_exit( ERROR( 30, "FSRef_from_path failed" ) );
	}
	
	err = FSGetCatalogInfo( &ref, 0, 0, 0, &spec, 0 );
	
	if ( err )
	{
		_exit( ERROR( 30, "FSGetCatalogInfo failed" ) );
	}
	
	unlink( utf8_filename );
	
	/*
		If the failure mode occurs, the em dash will be encoded
		as two bytes instead of one, making the string longer.
	*/
	
	return spec.name[ 0 ] == hfs_filename_len;
}

int main( int argc, char** argv )
{
	puts( "1..128" );
	
	for ( int i = 0;  i < 128;  ++i )
	{
		bool ok = run_test( i + 128 );
		
		const char* not_ = ok ? "" : "not ";
		
		printf( "%s" "ok %d\n", not_, i + 1 );
	}
	
	return 0;
}
