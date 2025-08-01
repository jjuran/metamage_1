/*	==========
 *	testing.cc
 *	==========
 */

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// Math
#include <math.h>
#ifndef __FP__
#include <fp.h>
#endif

// Standard C
#include "errno.h"
#include "signal.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

// Standard C++
#include <algorithm>
#include <vector>

// POSIX
#include "fcntl.h"
#include "sys/wait.h"
#include "unistd.h"

// mac-sys-utils
#include "mac_sys/errno_from_mac_error.hh"
#include "mac_sys/microseconds.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// Recall
#include "recall/backtrace.hh"
#include "recall/stack_crawl.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// nucleus
#include "nucleus/shared.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// poseven
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"
#include "poseven/functions/perror.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/errno_t.hh"

// pfiles
#include "pfiles/common.hh"

// Scaffold
#include "Tests.hh"

// Vectoria
#include "Vectoria/Matrix.hh"
#include "Vectoria/Vector.hh"
#include "Vectoria/CrossProduct3D.hh"

// Orion
#include "Orion/Main.hh"


namespace n = nucleus;
namespace p7 = poseven;


static int TestUnit( int argc, char** argv )
{
	Test::SampleTester tester;
	
	tester.RunAll();
	
	const Test::FullResults& results = tester.Results();
	
	Test::Status status = Test::AggregateStatus( results );
	
	return (status + 2) % 3;  // 0 => passed, 1 => failed, 2 => missed
}

static int TestAssert( int argc, char** argv )
{
	ASSERT( "This" == "a test" );
	//TEST_ASSERT( "This" == "a test" );
	
	return 0;
}

static int TestMap( int argc, char** argv )
{
	std::vector< plus::string > foo;
	
	int count = (argc > 2) ? gear::parse_unsigned_decimal( argv[ 2 ] ) : 10000;
	
	for ( int i = 0;  i < count;  ++i )
	{
		foo.push_back( plus::string( "foo" ) );
	}
	
	for ( int i = 0;  i < count;  ++i )
	{
		foo[ i ] = plus::string( "foo" );
	}
	
	foo.clear();
	
	return 0;
}


template < class Component, unsigned rows, unsigned cols >
plus::string PrintableValue( const Vectoria::Matrix< Component, rows, cols >& matrix )
{
	plus::var_string result;
	
	// 0x0:  "[]"
	// 0x2:  "[]"
	// 2x0:  "[  ]"
	// 2x2:  "[ 1 0  0 1 ]"
	
	result += "[";
	
	for ( unsigned i = 0;  i < rows;  ++i )
	{
		result += "";
		
		for ( unsigned j = 0;  j < cols;  ++j )
		{
			result += " ";
			
			int value = matrix.Cell( i, j );
			
			result += gear::inscribe_decimal( value );
			
			result += "";
		}
		
		result += " ";
	}
	
	result += "]";
	
	return result;
}

/*
TEST( 1x1 Matrix )
{
	namespace V = Vectoria;
	
	typedef V::Matrix< int, 1, 1 > OneByOne;
	
	OneByOne a;
	
	*a.begin() = 2;
	
	TEST_ASSERT( PrintableValue( a ) == "[ 2 ]" );
	
	V::BasicMatrix< int, 1, 1 >::Assign( a, V::ZeroMatrix );
	
	TEST_ASSERT( PrintableValue( a ) == "[ 0 ]" );
	
	V::SquareMatrix< int, 1 >::Assign( a, V::IdentityMatrix );
	
	TEST_ASSERT( PrintableValue( a ) == "[ 1 ]" );
	
	a *= 4;
	
	TEST_ASSERT( PrintableValue( a ) == "[ 4 ]" );
	
	OneByOne b = a;
	
	TEST_ASSERT( PrintableValue( b ) == "[ 4 ]" );
	
	b /= 2;
	
	TEST_ASSERT( PrintableValue( b ) == "[ 2 ]" );
	
	a += b;
	
	TEST_ASSERT( PrintableValue( a ) == "[ 6 ]" );
	
	a += -b;
	
	TEST_ASSERT( PrintableValue( a ) == "[ 4 ]" );
	
	OneByOne c = a * b;
	
	TEST_ASSERT( PrintableValue( c ) == "[ 8 ]" );
	
	c = V::Transpose( a );
	
	TEST_ASSERT( PrintableValue( c ) == "[ 4 ]" );
}
*/

TEST( Matrix )
{
	namespace V = Vectoria;
	
	typedef V::Matrix< int, 1, 1 > OneByOne;  // dot
	typedef V::Matrix< int, 1, 2 > OneByTwo;  // dash
	typedef V::Matrix< int, 2, 1 > TwoByOne;  // pipe
	typedef V::Matrix< int, 2, 2 > TwoByTwo;  // square
	
	TwoByTwo square_1 = V::ZeroMatrix();
	
	int* p = square_1.begin();
	
	TEST_ASSERT( PrintableValue( square_1 ) == "[ 0 0  0 0 ]" );
	
	V::IdentityMatrix::Initialize( square_1 );
	
	TEST_ASSERT( PrintableValue( square_1 ) == "[ 1 0  0 1 ]" );
	
	p[1] = 2;
	p[2] = 3;
	p[3] = 4;
	
	TwoByTwo square_2 = square_1;
	
	int* q = square_2.begin();
	
	TEST_ASSERT( PrintableValue( square_2 ) == "[ 1 2  3 4 ]" );
	
	TEST_ASSERT( PrintableValue( V::Transpose( square_2 ) ) == "[ 1 3  2 4 ]" );
	
	TEST_ASSERT( PrintableValue( square_2 * 2 ) == "[ 2 4  6 8 ]" );
	
	TEST_ASSERT( PrintableValue( -square_2 ) == "[ -1 -2  -3 -4 ]" );
	
}

TEST( Vector )
{
	namespace V = Vectoria;
	
	typedef V::Matrix< int, 2, 1 > Vector_2;
	typedef V::Matrix< int, 3, 1 > Vector_3;
	
	Vector_2 v2;
	
	int* p = v2.begin();
	
	p[0] = 3;
	p[1] = 4;
	
	TEST_ASSERT( V::MagnitudeSquared( v2 ) == 25 );
}

static int TestVectoria( int argc, char** argv )
{
	namespace V = Vectoria;
	
	printf( "%s\n%s\n", PrintableValue( V::IdentityMatrix::Make< int, 1 >() ).c_str(),
	                    PrintableValue( V::IdentityMatrix::Make< int, 2 >() ).c_str() );
	
	return 0;
}


static unsigned short crctable[] = {
     0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
     0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
     0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
     0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
     0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
     0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
     0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
     0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
     0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
     0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
     0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
     0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
     0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
     0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
     0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
     0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
     0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
     0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
     0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
     0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
     0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
     0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
     0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
     0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
     0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
     0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
     0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
     0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
     0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
     0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
   	 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
     0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};


/* taken from the mcvert source code */
static short CalculateCRC(const unsigned char *p, long len, short seed = 0)
{
 	short hold;		/* crc computed so far */
 	long i;			/* index into data */
 
 	hold = seed;			       /* start with seed */
 	for (i = 0; i < len; i++, p++) {
 		hold ^= (*p << 8);
 		hold = (hold << 8) ^ crctable[(unsigned char) (hold >> 8)];
 	}
 
 	return (hold);
}				/* calc_crc() */


#define CCITT_CRC_GEN	0x1021

static short CalcCRC(register const unsigned char *dataBuf, register long size)
{
	register unsigned long crc = 0;
	register unsigned long dataByte;
	register long i;
	
	while (--size >= 0)
	{
		dataByte = *dataBuf++;
		dataByte <<= 8;
		i = 8;
		do {	
			register long bit = dataByte;
			dataByte += dataByte;
			bit ^= crc;
			crc += crc;
			if (bit &= 0x8000)
				crc ^= CCITT_CRC_GEN;
		} while (--i);
	}
	return crc;
}

static int TestCRC16( int argc, char** argv )
{
	if (argc < 3)  return 1;
	
	const char* text = argv[ 2 ];
	
	unsigned int mb3CRC = CalculateCRC( (const uint8_t*) text, strlen( text ) );
	
	unsigned int lcsCRC = CalcCRC     ( (const uint8_t*) text, strlen( text ) );
	
	printf( "MB3: %.8x\n"
	        "LCS: %.8x\n",
	              mb3CRC,
	              lcsCRC );
	
	return 0;
}

static int TestOADC( int argc, char** argv )
{
	//if (argc < 3)  return 1;
	
	if ( TARGET_CPU_PPC )
	{
		ComponentInstance ci;
		
		OSErr err = ::OpenADefaultComponent( 'Foo ', 'Bar ', &ci );
		
		if ( err == noErr )
		{
			::CloseComponent( ci );
		}
		
		printf( "OpenADefaultComponent returned %d.\n", err );
	}
	
	return 0;
}


static int TestStrError( int argc, char** argv )
{
	errno = 0;
	
	for ( int errnum = 0;  errnum < 100;  ++errnum )
	{
		const char* str = strerror( errnum );
		
		if ( str == NULL || errno != 0 )  break;
		
		printf( "strerror(%d): %s\n", errnum, str );
	}
	
	return 0;
}

static int TestThrow( int argc, char** argv )
{
	if ( argc < 3 )  return 1;
	
	int errnum = gear::parse_decimal( argv[2] );
	
	if ( errnum < 0 )
	{
		Mac::ThrowOSStatus( errnum );
	}
	
	return 0;
}

static inline double get_scaled_linear_motion( double elapsed_time )
{
	return elapsed_time;
}

static inline double get_scaled_parabolic_motion( double elapsed_time )
{
	return elapsed_time * elapsed_time;
}

static inline double get_scaled_simple_harmonic_motion( double elapsed_time )
{
	return (1.0 - cos( elapsed_time * pi )) / 2.0;
}

template < class TimeSpan >
class path_generator
{
	private:
		TimeSpan its_timespan;
	
	public:
		path_generator( TimeSpan timespan ) : its_timespan( timespan )  {}
		
		double sample( TimeSpan elapsed_time ) const
		{
			return get_scaled_simple_harmonic_motion( 1.0 * elapsed_time / its_timespan );
		}
};

static inline void MoveWindowTo( p7::fd_t pos_fd, Point point )
{
	p7::write( pos_fd, (const char*) &point, sizeof point );
}

static int TestPath( int argc, char** argv )
{
	if ( argc < 3 )
	{
		return 1;
	}
	
#ifdef __RELIX__
	
	const char* port_path = getenv( "PORT" );
	
	if ( !port_path )
	{
		return 1;
	}
	
	n::owned< p7::fd_t > port = p7::open( port_path,
	                                      p7::o_rdonly | p7::o_directory );
	
	int pix = gear::parse_decimal( argv[2] );
	
	Point location = { 0, 0 };
	
	int fd = 0;
	
	p7::read( p7::openat( port, "w/.~pos", p7::o_rdonly ),
	          (char*) &location, sizeof location );
	
	int start_pos = location.h;
	
	int stop_pos = start_pos + pix;
	
	UInt64 time_length = 250000;  // quarter second
	
	n::owned< p7::fd_t > pos = p7::openat( port,
	                                       "w/.~pos",
	                                       p7::o_wronly | p7::o_trunc );
	
	if ( ::GetCurrentKeyModifiers() & (shiftKey | rightShiftKey) )
	{
		time_length *= 4;
	}
	
	using mac::sys::microseconds;
	
	UInt64 start_time = microseconds();
	
	//UInt64 stop_time = start_time + time_length;
	
	path_generator< UInt64 > path( time_length );
	
	UInt64 elapsed_time;
	
	while ( (elapsed_time = microseconds() - start_time) < time_length )
	{
		location.h = start_pos + path.sample( elapsed_time ) * pix;
		
		MoveWindowTo( pos, location );
	}
	
	location.h = stop_pos;
	
	MoveWindowTo( pos, location );
	
#endif
	
	return 0;
}


namespace Foo
{
	template < class T > class Bar {};
	
	class Bar_i_ {};
}

void TestMangling( Foo::Bar< int > );

void TestMangling( Foo::Bar< int > )
{
	abort();
}

void TestMangling( Foo::Bar_i_ );

void TestMangling( Foo::Bar_i_ )
{
	Foo::Bar< int > bar;
	
	TestMangling( bar );
}

static int TestMangling( int argc, char** argv )
{
	Foo::Bar_i_ bar_i_;
	
	TestMangling( bar_i_ );
	
	return 0;
}


static recall::frame_data gStackCrawl[ 64 ];

static unsigned n_stack_frames;

class ThingThatSavesStackCrawlDuringDestruction
{
	public:
		~ThingThatSavesStackCrawlDuringDestruction()
		{
			try
			{
				n_stack_frames = recall::make_stack_crawl( gStackCrawl, sizeof gStackCrawl );
			}
			catch ( ... )
			{
			}
		}
};

static void Throw()
{
	p7::throw_errno( ENOENT );
}

static int TestUnwind( int argc, char** argv )
{
	plus::var_string report;
	
	try
	{
		throw ThingThatSavesStackCrawlDuringDestruction();
	}
	catch ( ... )
	{
	}
	
	recall::make_report_from_stack_crawl( report, gStackCrawl, gStackCrawl + n_stack_frames );
	
	report += "\n";
	
	
	n_stack_frames = 0;
	
	try
	{
		ThingThatSavesStackCrawlDuringDestruction thing;
		
		Throw();
	}
	catch ( ... )
	{
	}
	
	recall::make_report_from_stack_crawl( report, gStackCrawl, gStackCrawl + n_stack_frames );
	
	report += "\n";
	
	p7::write( p7::stdout_fileno, report.data(), report.size() );
	
	return 0;
}


static int TestForkAndStop( int argc, char** argv )
{
	pid_t pid = vfork();
	
	if ( pid < 0 )
	{
		return 1;
	}
	
	if ( pid == 0 )
	{
		raise( SIGSTOP );
		
		_exit( 0 );
	}
	
	int stat = -1;
	
	waitpid( pid, &stat, 0 );
	
	return 0;
}

static int gObjectCount = 0;

class Object
{
	public:
		Object()  { ++gObjectCount; }
		
		Object( const Object& )  { ++gObjectCount; }
		
		~Object()  { --gObjectCount; }
		
		//Object& operator=( const gObjectCount& )  {}
};

namespace nucleus
{
	
	template <> struct disposer< Object* > : public dispose_with_delete {};
	
}

static void CheckObjects( int trial )
{
	if ( gObjectCount )
	{
		fprintf( stderr, "%d: Object count: %d\n", trial, gObjectCount );
		
		gObjectCount = 0;
	}
}

static int TestNucleusOwnedShared( int argc, char** argv )
{
	{
		n::owned< Object* > foo = n::owned< Object* >::seize( new Object );
	}
	CheckObjects( 1 );
	
	{
		n::owned< Object* > foo = n::owned< Object* >::seize( new Object );
		
		n::owned< Object* > bar( foo );
	}
	CheckObjects( 2 );
	
	{
		n::owned< Object* > foo = n::owned< Object* >::seize( new Object );
		n::owned< Object* > bar = n::owned< Object* >::seize( new Object );
		
		bar = foo;
	}
	CheckObjects( 3 );
	
	{
		n::owned< Object* > foo = n::owned< Object* >::seize( new Object );
		
		foo.reset();
	}
	CheckObjects( 4 );
	
	{
		n::shared< Object* > foo = n::owned< Object* >::seize( new Object );
	}
	CheckObjects( 5 );
	
	{
		n::shared< Object* > foo = n::owned< Object* >::seize( new Object );
		
		n::shared< Object* > bar( foo );
	}
	CheckObjects( 6 );
	
	{
		n::shared< Object* > foo = n::owned< Object* >::seize( new Object );
		n::shared< Object* > bar = n::owned< Object* >::seize( new Object );
		
		bar = foo;
	}
	CheckObjects( 7 );
	
	{
		n::shared< Object* > foo = n::owned< Object* >::seize( new Object );
		
		foo.reset();
	}
	CheckObjects( 8 );
	
	{
		n::shared< Object* > foo = n::owned< Object* >::seize( new Object );
		n::shared< Object* > bar = n::owned< Object* >::seize( new Object );
		
		n::shared< Object* > baz( bar );
		
		bar = foo;
	}
	CheckObjects( 9 );
	
	return 0;
}

typedef int (*MainProcPtr)( int argc, char** argv );

struct command_t
{
	const char* name;
	MainProcPtr proc;
};

static const command_t global_commands[] =
{
	{ "OADC",      TestOADC       },
	{ "assert",    TestAssert     },
	{ "crc16",     TestCRC16      },
	{ "forkstop",  TestForkAndStop },
	{ "mangling",  TestMangling   },
	{ "map",       TestMap        },
	{ "owned",     TestNucleusOwnedShared },
	{ "path",      TestPath       },
	{ "strerror",  TestStrError   },
	{ "throw",     TestThrow      },
	{ "unit",      TestUnit       },
	{ "unwind",    TestUnwind     },
	{ "vector",    TestVectoria   },
};

static bool operator==( const command_t& command, const char* name )
{
	return strcmp( command.name, name ) == 0;
}

static bool operator<( const command_t& command, const char* name )
{
	return strcmp( command.name, name ) < 0;
}

static bool operator<( const char* name, const command_t& command )
{
	return strcmp( name, command.name ) < 0;
}

static const command_t* find_command( const char* name )
{
	const command_t* begin = global_commands;
	const command_t* end   = begin + sizeof global_commands / sizeof (command_t);
	
	const command_t* it = std::lower_bound( begin, end, name );
	
	if ( it != end  &&  *it == name )
	{
		return it;
	}
	
	return NULL;
}

namespace tool
{
	
	int Main( int argc, char** argv )
	{
		plus::var_string message;
		
		if (argc <= 1)
		{
			const command_t* begin = global_commands;
			const command_t* end   = begin + sizeof global_commands / sizeof (command_t);
			
			for ( const command_t* it = begin;  it != end;  ++it )
			{
				message += it->name;
				message += "\n";
			}
			
			p7::write( p7::stdout_fileno, message );
			
			return 0;
		}
		
		const command_t* sub = find_command( argv[1] );
		
		if ( sub == NULL )
		{
			p7::perror( "testing", argv[1], "no such subcommand" );
			
			return 1;
		}
		
		try
		{
			return sub->proc( argc, argv );
		}
		catch ( const Mac::OSStatus& err )
		{
			plus::var_string status = "OSStatus ";
			
			status += gear::inscribe_decimal( err );
			
			p7::perror( "testing", status, 0 );
			
			int result = mac::sys::errno_from_mac_error( err );
			
			if ( result > 0 )
			{
				p7::throw_errno( result );
			}
			
			throw;
		}
		
		// Not reached
		return 0;
	}

}
