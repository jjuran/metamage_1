/*
	exceptions.cc
	-------------
*/

#include "exceptions.hh"

// Standard C
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


#define PROGRAM "sndtrack"

#define LENGTH( array )  (sizeof (array) / sizeof (array)[0])

enum disposition
{
	none,
	debug,
	warn,
	error,
	fatal,
	
	alloc = fatal,
	start = error,
	stop  = warn,
	ioerr = fatal,
	proto = fatal,
	under = fatal,
	frame = warn,
	inval = error,
	empty = warn,
	badid = warn,
	audio = warn,
};

static const char* responses[] =
{
	"?",
	"DEBUG",
	"WARNING",
	"ERROR",
	"FATAL ERROR",
};

struct exception_spec
{
	disposition  response;
	const char*  summary;
};

static const exception_spec exceptions[] =
{
	{ none,  "???" },
	{ alloc, "OUT OF MEMORY" },
	{ start, "error starting audio stream" },
	{ stop,  "error stopping audio stream" },
	{ ioerr, "error reading input stream" },
	{ debug, "EOF reading header" },  // client crashed?
	{ ioerr, "EOF after short read of header" },
	{ proto, "non-zero reserved field in header" },
	{ proto, "undefined domain value" },
	{ proto, "undefined opcode value" },
	{ under, "message length too short for type" },
	{ frame, "message length too long for type" },
	{ frame, "message length invalid for type" },
	{ ioerr, "EOF reading message" },
	{ empty, "sound contains no data" },
	{ frame, "sound is unterminated" },
	{ inval, "invalid zero playback rate" },
	{ inval, "invalid duration value > 255" },
	{ empty, "sound duration is zero" },
	{ inval, "invalid zero wavelength for tone" },
	{ empty, "tone duration is zero" },
	{ badid, "no such ID for four-tone update" },
	{ audio, "gap in audio playback detected" },
	{ ioerr, "error writing output stream" },
	{ ioerr, "short write to output stream" },
};

bool take_exception_internal( exception_type type )
{
	assert( type > 0  &&  type < LENGTH( exceptions ) );
	
	const exception_spec& e = exceptions[ type ];
	
	if ( e.response >= warn )
	{
		const char* category = responses[ e.response ];
		
		fprintf( stderr, PROGRAM ": %s:  ** %s **\n", category, e.summary );
	}
	
	return e.response == fatal;
}

void take_exception( exception_type type )
{
	if ( take_exception_internal( type ) )
	{
		exit( 1 );
	}
}
