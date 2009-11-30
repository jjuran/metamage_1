/*	==========
 *	testing.cc
 *	==========
 */

// Mac OS Universal Interfaces
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __FOLDERS__
#include <Folders.h>
#endif
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

// Standard C
#include "errno.h"
#include "signal.h"

// Standard C/C++
#include <cstdio>
#include <cstdlib>

// Standard C++
#include <functional>
#include <map>
#include <string>
#include <vector>

// POSIX
#include "fcntl.h"
#include "sys/ioctl.h"
#include "sys/stat.h"
#include "sys/wait.h"
#include "unistd.h"

// Iota
#include "iota/decimal.hh"

// Recall
#include "recall/demangle.hh"
#include "recall/mach_o.hh"
#include "recall/macsbug_symbols.hh"
#include "recall/stack_crawl.hh"
#include "recall/traceback_tables.hh"

// Debug
#include "debug/assert.hh"

// Divergence
#include "Divergence/Utilities.hh"

// MoreFiles
//#include "MoreFilesExtras.h"

// Nucleus
#include "Nucleus/Convert.h"
#include "Nucleus/Exception.h"
#include "Nucleus/TheExceptionBeingHandled.h"

// Io
#include "io/slurp.hh"

// Nitrogen
#include "Nitrogen/CodeFragments.h"
#include "Nitrogen/DateTimeUtils.h"
#include "Nitrogen/MacMemory.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/OSUtils.h"
//#include "Nitrogen/Resources.h"
#include "Nitrogen/Sound.h"
#include "Nitrogen/Str.h"
#include "Nitrogen/Threads.h"

// poseven
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/write.hh"

// Nitrogen Extras / ClassicToolbox
#if !TARGET_API_MAC_CARBON
#include "ClassicToolbox/AppleTalk.h"
#include "ClassicToolbox/Serial.h"
#endif

// Nitrogen Extras / Iteration
#include "Iteration/AEDescListItems.h"
#include "Iteration/AEDescListItemDatas.h"
#include "Iteration/FSContents.h"

// Scaffold
#include "Tests.hh"

// BitsAndBytes
#include "DecimalStrings.hh"
#include "HexStrings.hh"

// Arcana
#include "CRC32.hh"
#include "MD5.hh"

// Vectoria
#include "Vectoria/Matrix.hh"
#include "Vectoria/Vector.hh"
#include "Vectoria/CrossProduct3D.hh"

// Orion
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace n = nucleus;
namespace NN = Nucleus;
namespace p7 = poseven;
namespace Div = Divergence;

using BitsAndBytes::EncodeAsHex;
using BitsAndBytes::EncodeDecimal2;


static int TestUnit( int argc, iota::argv_t argv )
{
	Test::SampleTester tester;
	
	tester.RunAll();
	
	const Test::FullResults& results = tester.Results();
	
	Test::Status status = Test::AggregateStatus( results );
	
	return (status + 2) % 3;  // 0 => passed, 1 => failed, 2 => missed
}

static int TestAssert( int argc, iota::argv_t argv )
{
	ASSERT( "This" == "a test" );
	//TEST_ASSERT( "This" == "a test" );
	
	return 0;
}

static int TestMap( int argc, iota::argv_t argv )
{
	std::vector< std::string > foo;
	
	int count = (argc > 2) ? std::atoi( argv[ 2 ] ) : 10000;
	
	for ( int i = 0;  i < count;  ++i )
	{
		foo.push_back( std::string( "foo" ) );
	}
	
	for ( int i = 0;  i < count;  ++i )
	{
		foo[ i ] = std::string( "foo" );
	}
	
	foo.clear();
	
	return 0;
}


template < class Component, unsigned rows, unsigned cols >
std::string PrintableValue( const Vectoria::Matrix< Component, rows, cols >& matrix )
{
	std::string result;
	
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
			
			std::string string = iota::inscribe_decimal( value );
			
			result += string;
			
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

static int TestVectoria( int argc, iota::argv_t argv )
{
	namespace V = Vectoria;
	
	std::printf( "%s\n%s\n", PrintableValue( V::IdentityMatrix::Make< int, 1 >() ).c_str(),
	                         PrintableValue( V::IdentityMatrix::Make< int, 2 >() ).c_str() );
	
	return 0;
}


static std::string MinSec( unsigned int seconds )
{
	return EncodeDecimal2( seconds / 60 ) + ":" + 
	       EncodeDecimal2( seconds % 60 );
}


#if !TARGET_API_MAC_CARBON

namespace Test {
	
	struct LookupFailed  {};
	
	static AddrBlock LookupName(EntityName& entityName)
	{
		NBPparms pb;
		char buf[104];
		
		pb.ioCompletion = NULL;
		pb.csCode = lookupName;
		pb.interval = 7;
		pb.count = 5;
		pb.nbpPtrs.entityPtr = (char*)&entityName;
		pb.parm.Lookup.retBuffPtr = buf;
		pb.parm.Lookup.retBuffSize = 104;
		pb.parm.Lookup.maxToGet = 1;
		
		OSErr err = ::PLookupNameSync((MPPPBPtr)&pb);
		N::ThrowOSStatus(err);
		if (pb.parm.Lookup.numGotten != 1)  throw LookupFailed();
		
		AddrBlock addr;
		err = ::NBPExtract(buf, 1, 1, &entityName, &addr);
		N::ThrowOSStatus(err);
		
		return addr;
	}
	
	static AddrBlock LookupName(const EntityName& entityName)
	{
		EntityName result = entityName;
		return LookupName(result);
	}
	
	static const unsigned char* ASPGetStatus(AddrBlock addr)
	{
		short refNum;
		OSErr err = ::OpenDriver("\p.XPP", &refNum);
		N::ThrowOSStatus(err);
		
		ASPOpenPrm pbASP;
		XPPPrmBlk& pbXPP = (XPPPrmBlk&)pbASP;
		//char scb[scbMemSize];
		pbASP.ioCompletion = NULL;
		pbASP.ioRefNum = refNum;
		pbASP.csCode = getStatus;
		pbASP.aspTimeout = 7;
		pbASP.aspRetry = 5;
		pbASP.serverAddr = addr;
		
		static char status[512];
		pbXPP.rbSize = 512;
		pbXPP.rbPtr = status;
		
		err = ::ASPGetStatus((XPPParmBlkPtr)&pbASP, false);
		N::ThrowOSStatus(err);
		
		return reinterpret_cast<const unsigned char*>(status);
	}
	
}

static bool TestAFPServer( const unsigned char* serverName )
{
	const Byte* status = Test::ASPGetStatus( Test::LookupName( N::NBPSetEntity( serverName,
	                                                                            "\pAFPServer" ) ) );
	
	return status[ 9 ] == 0xFB;
	//if (status[9] == -5)  return true;
}

static bool TestAFPServer(const std::string& serverName)
{
	return TestAFPServer( N::Str255( serverName ) );
}

#endif  // #if !TARGET_API_MAC_CARBON

static int TestAFP( int argc, iota::argv_t argv )
{
	const char* server = argv[ 2 ];
	
#if !TARGET_API_MAC_CARBON
	
	bool isRunningOSX = TestAFPServer( server );
	
	const char* supports = isRunningOSX ? "does not support"
	                                    : "supports";
	
	std::string message;
	
	message += "Server '";
	message += server;
	message += "' ";
	message += supports;
	message += " AppleTalk.\n";
	
	p7::write( p7::stdout_fileno, message );
	
#endif
	
	return 0;
}


static int TestDate( int argc, iota::argv_t argv )
{
	std::printf( "DateTime == %.8x\n", N::GetDateTime() );
	
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

static int TestCRC16( int argc, iota::argv_t argv )
{
	if (argc < 3)  return 1;
	
	const char* text = argv[ 2 ];
	
	unsigned int mb3CRC = CalculateCRC( (const unsigned char*)text, std::strlen( text ) );
	
	unsigned int lcsCRC = CalcCRC     ( (const unsigned char*)text, std::strlen( text ) );
	
	std::printf( "MB3: %.8x\n"
	             "LCS: %.8x\n",
	                   mb3CRC,
	                   lcsCRC );
	
	return 0;
}

static int TestCRC32( int argc, iota::argv_t argv )
{
	if (argc < 3)  return 1;
	
	const char* text = argv[2];
	unsigned int crc = CRC32::Checksum(text, std::strlen(text));
	
	std::printf( "%.8x\n", crc );
	
	return 0;
}

static MD5::Result MD5String( const char* text )
{
	return MD5::Digest_Bytes( text, std::strlen( text ) );
}

static std::string MD5Hex( const char* text )
{
	return EncodeAsHex( MD5String( text ) );
}

TEST( MD5 )
{
	TEST_ASSERT( "d41d8cd98f00b204e9800998ecf8427e" == MD5Hex( "" ) );
	TEST_ASSERT( "0cc175b9c0f1b6a831c399e269772661" == MD5Hex( "a" ) );
	TEST_ASSERT( "900150983cd24fb0d6963f7d28e17f72" == MD5Hex( "abc" ) );
	TEST_ASSERT( "f96b697d7cb7938d525a2f31aaf161d0" == MD5Hex( "message digest" ) );
	TEST_ASSERT( "c3fcd3d76192e4007dfb496cca67e13b" == MD5Hex( "abcdefghijklmnop" "qrstuvwxyz" ) );
	TEST_ASSERT( "d174ab98d277d9f5a5611c2c9f419d9f" == MD5Hex( "ABCDEFGHIJKLMNOP" "QRSTUVWXYZabcdef" "ghijklmnopqrstuv" "wxyz0123456789" ) );
	TEST_ASSERT( "57edf4a22be3c955ac49da2e2107b67a" == MD5Hex( "1234567890123456" "7890123456789012" "3456789012345678" "9012345678901234"
	                                                           "5678901234567890" ) );
}

static int TestMD5( int argc, iota::argv_t argv )
{
	if (argc < 3)  return 1;
	
	const char* text = argv[ 2 ];
	
	std::string message = MD5Hex( text ) + "\n";
	
	p7::write( p7::stdout_fileno, message );
	
	return 0;
}

static int TestOADC( int argc, iota::argv_t argv )
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
		
		std::printf( "OpenADefaultComponent returned %d.\n", err );
	}
	
	return 0;
}


static int TestSoundInput( int argc, iota::argv_t argv )
{
	//if (argc < 3)  return 1;
	
	typedef N::SoundInputDevice_Container::const_iterator SID_ci;
	
	//SID_ci it = N::SoundInputDevices().begin();
	SID_ci it ( N::SoundInputDevices().begin() );
	SID_ci end( N::SoundInputDevices().end()   );
	
	for ( ;  it != end;  ++it )
	{
		ConstStr255Param deviceName = *it;
		
		std::string output = NN::Convert< std::string >( deviceName ) + "\n";
		
		p7::write( p7::stdout_fileno, output );
	}
	
	return 0;
}

static std::string Capitalize( std::string s )
{
	char& c = s[ 0 ];
	
	if ( std::islower( c ) )
	{
		c -= ' ';
	}
	
	return s;
}

static void PrintString( std::string s )
{
	s += "\n";
	
	p7::write( p7::stdout_fileno, s );
}

static int TestAE( int argc, iota::argv_t argv )
{
	//if (argc < 3)  return 1;
	NN::Owned< N::AEDescList_Data > list = N::AECreateList< N::AEDescList_Data >();
	
	N::AEPutPtr< N::typeChar >( list, 0, "foo" );
	N::AEPutPtr< N::typeChar >( list, 0, "bar" );
	N::AEPutPtr< N::typeChar >( list, 0, "baz" );
	
	/*
	N::AEDescListItemData_Container< typeChar > listData = N::AEDescListItemDatas< typeChar >( list );
	
	N::AEDescListItemData< typeChar > foo = *listData.begin();
	
	{
		NN::Scoped< N::AEDescListItemData< typeChar > > scoped( foo );
		
		foo = "zee";
		
		std::for_each( listData.begin(),
		               listData.end(),
		               std::ptr_fun( PrintString ) );
		
		std::transform( listData.begin(), 
		                listData.end(), 
		                listData.begin(), 
		                std::ptr_fun( Capitalize ) );
	}
	
	std::for_each( listData.begin(),
	               listData.end(),
	               std::ptr_fun( PrintString ) );
	*/
	
	return 0;
}

// Template non-type argument objects shall have external linkage
void MyThreadRoutine( const std::string& param  );

void MyThreadRoutine( const std::string& param  )
{
	std::string message = "Param is " + param + "\n";
	
	p7::write( p7::stdout_fileno, message );
}

static int TestThread( int argc, iota::argv_t argv )
{
	int done = 0;
	
	//Io::Out << "Testing thread is ID " << N::GetCurrentThread() << "\n";
	
	std::string param;
	
	if ( argc > 2 )
	{
		param = argv[ 2 ];
	}
	
	NN::Owned< N::ThreadID > thread = N::NewThread< const std::string&, MyThreadRoutine >( N::kCooperativeThread,
	                                                                                       param,
	                                                                                       0,
	                                                                                       N::ThreadOptions() );
	N::ThreadState state = N::GetThreadState( thread );
	
	int count = 0;
	
	while ( !done )
	{
		sleep( 0 );
		
		try
		{
			state = N::GetThreadState( thread );
			
			std::printf( "State is %d\n", state );
		}
		catch ( const N::OSStatus& err )
		{
			std::fprintf( stderr, "OSStatus %d\n", err.Get() );
		}
		
		++count;
		
		if ( count >= 5 )  break;
	}
	
	std::printf( "Looped %d times.\n", count );
	
	return 0;
}


static void DoSomethingWithServiceFile( const FSSpec& file )
{
	typedef NN::StringFlattener< std::string > Flattener;
	
	using namespace io::path_descent_operators;
	
	// Find Info.plist
	FSSpec infoPListFile = NN::Convert< N::FSDirSpec >( file ) / "Contents" / "Info.plist";
	
	// Read the entire file contents
	std::string infoPList = io::slurp_file< Flattener >( infoPListFile );
	
	// Search for a menu item
	std::size_t iNSMenuItem = infoPList.find( "<key>NSMenuItem</key>" );
	
	if ( iNSMenuItem == infoPList.npos )
	{
		// No menu items, we're done
		return;
	}
	
	// Keep track of where we left off
	std::size_t iLast = iNSMenuItem;
	
	while ( true )
	{
		std::size_t iDefault = infoPList.find( "<key>default</key>", iLast );
		
		if ( iDefault == infoPList.npos )
		{
			// No more entries, we're done
			break;
		}
		
		std::string stringElement = "<string>";
		// Find the <string> start tag
		std::size_t iString = infoPList.find( stringElement, iDefault );
		// Skip the tag
		std::size_t iValue = iString + stringElement.size();
		// Find the end tag
		std::size_t iEndString = infoPList.find( "</string>", iValue );
		// Grab the intervening text
		std::string value = infoPList.substr( iValue, iEndString - iValue );
		
		std::printf( "Service: %s\n", value.c_str() );
		
		iLast = iEndString;
	}
}

static int TestServices( int argc, iota::argv_t argv )
{
	//if (argc < 3)  return 1;
	
	/*
	N::FSDirSpec systemLibraryServices = N::RootDirectory( N::BootVolume() ) << "System"
	                                                                         << "Library"
	                                                                         << "Services";
	
	std::for_each( N::FSContents( systemLibraryServices ).begin(),
	               N::FSContents( systemLibraryServices ).end(),
	               std::ptr_fun( DoSomethingWithServiceFile ) );
	*/
	
	return 0;
}

static int ReadFromNull()
{
	int* null = NULL;
	
	return *null;
}

static void WriteToNull( int foo )
{
	int* null = NULL;
	
	*null = foo;
}

static int TestNull( int argc, iota::argv_t argv )
{
	int foo = ReadFromNull();
	
	WriteToNull( foo );
	
	return 0;
}

	struct FragmentImage {};
	
	/*
	static NX::DataPtr< FragmentImage > ReadFragmentImageFromPluginFile( const FSSpec& file )
	{
		NN::Owned< N::FSFileRefNum > filehandle = N::FSpOpenDF( file, N::fsRdPerm );
		
		std::size_t size = N::GetEOF( filehandle );
		
		std::auto_ptr< FragmentImage > result;
		
		result.reset( static_cast< FragmentImage* >( ::operator new( size ) ) );
		
		int bytes = N::FSRead( filehandle, size, reinterpret_cast< char* >( result.get() ) );
		
		return NX::DataPtr< FragmentImage >( result, size );
	}
	*/
	
	static NN::Owned< N::Ptr > ReadFragmentImageFromPluginFile( const char* pathname )
	{
		n::owned< p7::fd_t > filehandle = io::open_for_reading( pathname );
		
		struct ::stat stat_buffer;
		
		int statted = fstat( filehandle.get(), &stat_buffer );
		
		std::size_t size = stat_buffer.st_size;
		
		NN::Owned< N::Ptr > result = N::NewPtr( size );
		
		int bytes = read( filehandle, result.get(), size );
		
		return result;
	}
	
static int TestGMFShared( int argc, iota::argv_t argv )
{
#if TARGET_RT_MAC_CFM
	
	if ( argc < 3 )  return 1;
	
	const char* pathname = argv[2];
	
	NN::Owned< N::Ptr > fragment = ReadFragmentImageFromPluginFile( pathname );
	
	int len = N::GetPtrSize( fragment );
	
	std::printf( "Fragment length: %d bytes\n", len );
	
	NN::Owned< CFragConnectionID > one = N::GetMemFragment< N::kPrivateCFragCopy >( fragment.Get(), len );
	
	int* scratch;
	
	N::FindSymbol( one, "\p" "errno", &scratch );
	
	*scratch = 42;
	
	NN::Owned< CFragConnectionID > two = N::GetMemFragment< N::kPrivateCFragCopy >( fragment.Get(), len );
	
	N::FindSymbol( two, "\p" "errno", &scratch );
	
	std::printf( "Second connection scratch value: %d\n", *scratch );
	
#endif
	
	return 0;
}

static int TestStrError( int argc, iota::argv_t argv )
{
	errno = 0;
	
	for ( int errnum = 0;  errnum < 100;  ++errnum )
	{
		const char* str = strerror( errnum );
		
		if ( str == NULL || errno != 0 )  break;
		
		std::printf( "strerror(%d): %s\n", errnum, str );
	}
	
	return 0;
}

static int TestThrow( int argc, iota::argv_t argv )
{
	if ( argc < 3 )  return 1;
	
	int errnum = std::atoi( argv[2] );
	
	if ( errnum < 0 )
	{
		N::ThrowOSStatus( errnum );
	}
	
	return 0;
}

static int TestReadLoc( int argc, iota::argv_t argv )
{
	//if ( argc < 3 )  return 1;
	
	MachineLocation loc;
	
	N::ReadLocation( loc );
	
	double latitude  = loc.latitude;
	double longitude = loc.longitude;
	
	// Convert from Fract to normal -- move binary point
	latitude  /= (1 << 30);
	longitude /= (1 << 30);
	
	// Convert to degrees
	latitude  *= 90;
	longitude *= 90;
	
	long gmtDelta = loc.u.gmtDelta;
	
	bool dls = gmtDelta & 0x80000000;
	
	gmtDelta &= 0x00FFFFFF;
	
	bool signExtend = gmtDelta & 0x00800000;
	
	gmtDelta |= signExtend ? 0xFF000000 : 0x00000000;
	
	/*
	Io::Out << "Latitude:  " << latitude  << " degrees\n";
	Io::Out << "Longitude: " << longitude << " degrees\n";
	Io::Out << "Daylight Savings Time: " << (dls ? "on" : "off") << "\n";
	Io::Out << "GMT delta: " << gmtDelta / 3600 << " hours\n";
	*/
	
	/*
	std::printf( "Latitude: %d degrees\n",  int( latitude  ) );
	std::printf( "Longitude: %d degrees\n", int( longitude ) );
	
	std::printf( "Daylight Savings Time: %s\n", dls ? "on" : "off" );
	
	std::printf( "GMT delta: %d hours\n", int( gmtDelta / 3600 ) );
	*/
	
	#define ENDL "\n"
	//#define ENDL ",  "
	
	std::printf( "Latitude: %d degrees"      ENDL
	             "Longitude: %d degrees"     ENDL
	             "Daylight Savings Time: %s" ENDL
	             "GMT delta: %d hours"       "\n", int( latitude  ),
	                                               int( longitude ),
	                                               dls ? "on" : "off",
	                                               int( gmtDelta / 3600 ) );
	
	//std::fflush( stdout );
	
	return 0;
}

inline double get_scaled_linear_motion( double elapsed_time )
{
	return elapsed_time;
}

inline double get_scaled_parabolic_motion( double elapsed_time )
{
	return elapsed_time * elapsed_time;
}

inline double get_scaled_simple_harmonic_motion( double elapsed_time )
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

static int TestPath( int argc, iota::argv_t argv )
{
	if ( argc < 3 )
	{
		return 1;
	}
	
#ifdef __LAMP__
	
	const char* window_path = getenv( "WINDOW" );
	
	if ( !window_path )
	{
		return 1;
	}
	
	n::owned< p7::fd_t > window = p7::open( window_path,
	                                        p7::o_rdonly | p7::o_directory );
	
	int pix = std::atoi( argv[2] );
	
	Point location = { 0, 0 };
	
	int fd = 0;
	
	p7::read( p7::openat( window, "ref/pos", p7::o_rdonly | p7::o_binary ),
	          (char*) &location, sizeof location );
	
	int start_pos = location.h;
	
	int stop_pos = start_pos + pix;
	
	UInt64 time_length = 250000;  // quarter second
	
	n::owned< p7::fd_t > pos = p7::openat( window,
	                                       "ref/pos",
	                                       p7::o_wronly | p7::o_trunc | p7::o_binary );
	
	if ( ::GetCurrentKeyModifiers() & (shiftKey | rightShiftKey) )
	{
		time_length *= 4;
	}
	
	UInt64 start_time = N::Microseconds();
	
	//UInt64 stop_time = start_time + time_length;
	
	path_generator< UInt64 > path( time_length );
	
	UInt64 elapsed_time;
	
	while ( (elapsed_time = N::Microseconds() - start_time) < time_length )
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
	std::abort();
}

void TestMangling( Foo::Bar_i_ );

void TestMangling( Foo::Bar_i_ )
{
	Foo::Bar< int > bar;
	
	TestMangling( bar );
}

static int TestMangling( int argc, iota::argv_t argv )
{
	Foo::Bar_i_ bar_i_;
	
	TestMangling( bar_i_ );
	
	return 0;
}

typedef pascal void (*CallbackProcPtr)();

typedef DragGrayRgnUPP CallbackUPP;

static pascal void MyCallback()
{
	std::abort();
}

typedef pascal unsigned char (*InitMainProcPtr)( RGBColor* );

static int TestCallback( int argc, iota::argv_t argv )
{
	if ( argc <= 2 )
	{
		std::fputs( "Missing argument", stderr );
		
		return 1;
	}
	
	const char* pathname = argv[2];
	
	FSSpec file = Div::ResolvePathToFSSpec( pathname );
	
	NN::Owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, N::fsRdPerm );
	
	NN::Owned< N::Handle > init = N::DetachResource( N::Get1Resource( N::ResType( 'INIT' ), N::ResID( 0 ) ) );
	
	N::HLock( init );
	
	ColorComplementUPP initMain = (ColorComplementUPP) *init.Get();
	
	CallbackUPP callback = NewDragGrayRgnUPP( MyCallback );
	
	InvokeColorComplementUPP( (RGBColor*) callback, initMain );
	
	return 0;
}



static int TestUnmangle( int argc, iota::argv_t argv )
{
	if ( argc <= 2 )
	{
		std::puts( "Missing argument" );
		
		return 1;
	}
	
	const char* name = argv[2];
	
	std::string unmangled = name[0] == '.' ? recall::demangle_MWCPPC( name )
	                                       : recall::demangle_MWC68K( name );
	
	unmangled += "\n";
	
	p7::write( p7::stdout_fileno, unmangled );
	
	return 0;
}


static std::vector< recall::frame_data > gStackCrawl;

class ThingThatSavesStackCrawlDuringDestruction
{
	public:
		~ThingThatSavesStackCrawlDuringDestruction()
		{
			try
			{
				recall::make_stack_crawl( gStackCrawl );
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

static int TestUnwind( int argc, iota::argv_t argv )
{
	std::string report;
	
	try
	{
		throw ThingThatSavesStackCrawlDuringDestruction();
	}
	catch ( ... )
	{
	}
	
	report += recall::make_report_from_stack_crawl( gStackCrawl.begin(), gStackCrawl.end() );
	
	report += "\n";
	
	
	gStackCrawl.clear();
	
	try
	{
		ThingThatSavesStackCrawlDuringDestruction thing;
		
		Throw();
	}
	catch ( ... )
	{
	}
	
	report += recall::make_report_from_stack_crawl( gStackCrawl.begin(), gStackCrawl.end() );
	
	report += "\n";
	
	p7::write( p7::stdout_fileno, report.data(), report.size() );
	
	return 0;
}


static int TestForkAndStop( int argc, iota::argv_t argv )
{
	pid_t pid = vfork();
	
	if ( pid == -1 )
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

static int TestDefaultThreadStackSize( int argc, iota::argv_t argv )
{
	Size size = 0;
	
	OSErr err = GetDefaultThreadStackSize( kCooperativeThread, &size );
	
	if ( err != noErr )
	{
		std::fprintf( stderr, "GetDefaultThreadStackSize() returned OSErr %d\n", err );
		
		return 1;
	}
	
	std::fprintf( stdout, "Default thread stack size:  %d\n", size );
	
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

namespace Nucleus
{
	
	template <> struct Disposer< Object* > : public DisposeWithDelete {};
	
}

static void CheckObjects( int trial )
{
	if ( gObjectCount )
	{
		std::fprintf( stderr, "%d: Object count: %d\n", trial, gObjectCount );
		
		gObjectCount = 0;
	}
}

static int TestNucleusOwnedShared( int argc, iota::argv_t argv )
{
	{
		NN::Owned< Object* > foo = NN::Owned< Object* >::Seize( new Object );
	}
	CheckObjects( 1 );
	
	{
		NN::Owned< Object* > foo = NN::Owned< Object* >::Seize( new Object );
		
		NN::Owned< Object* > bar( foo );
	}
	CheckObjects( 2 );
	
	{
		NN::Owned< Object* > foo = NN::Owned< Object* >::Seize( new Object );
		NN::Owned< Object* > bar = NN::Owned< Object* >::Seize( new Object );
		
		bar = foo;
	}
	CheckObjects( 3 );
	
	{
		NN::Owned< Object* > foo = NN::Owned< Object* >::Seize( new Object );
		
		foo.Reset();
	}
	CheckObjects( 4 );
	
	{
		NN::Shared< Object* > foo = NN::Owned< Object* >::Seize( new Object );
	}
	CheckObjects( 5 );
	
	{
		NN::Shared< Object* > foo = NN::Owned< Object* >::Seize( new Object );
		
		NN::Shared< Object* > bar( foo );
	}
	CheckObjects( 6 );
	
	{
		NN::Shared< Object* > foo = NN::Owned< Object* >::Seize( new Object );
		NN::Shared< Object* > bar = NN::Owned< Object* >::Seize( new Object );
		
		bar = foo;
	}
	CheckObjects( 7 );
	
	{
		NN::Shared< Object* > foo = NN::Owned< Object* >::Seize( new Object );
		
		foo.Reset();
	}
	CheckObjects( 8 );
	
	{
		NN::Shared< Object* > foo = NN::Owned< Object* >::Seize( new Object );
		NN::Shared< Object* > bar = NN::Owned< Object* >::Seize( new Object );
		
		NN::Shared< Object* > baz( bar );
		
		bar = foo;
	}
	CheckObjects( 9 );
	
	return 0;
}

typedef int (*MainProcPtr)( int argc, iota::argv_t argv );

struct SubMain
{
	const char* name;
	MainProcPtr proc;
};

const SubMain gSubs[] =
{
	{ "unit",      TestUnit       },
	{ "assert",    TestAssert     },
	{ "map",       TestMap        },
	{ "vector",    TestVectoria   },
	
#if !TARGET_API_MAC_CARBON
	
	{ "afp",       TestAFP        },
	
#endif
	
	{ "date",      TestDate       },
	{ "crc16",     TestCRC16      },
	{ "crc32",     TestCRC32      },
	{ "md5",       TestMD5        },
	{ "OADC",      TestOADC       },
	{ "si",        TestSoundInput },
	{ "ae",        TestAE         },
//	{ "svcs",      TestServices   },
	{ "thread",    TestThread     },
	{ "null",      TestNull       },
	{ "path",      TestPath       },
	{ "unmangle",  TestUnmangle   },
	{ "unwind",    TestUnwind     },
	{ "mangling",  TestMangling   },
	{ "callback",  TestCallback   },
	{ "forkstop",  TestForkAndStop },
	{ "stack",     TestDefaultThreadStackSize },
	{ "owned",     TestNucleusOwnedShared },
	
#if TARGET_RT_MAC_CFM
	
	{ "gmf",       TestGMFShared  },
	
#endif
	
	{ "strerror",  TestStrError   },
	{ "throw",     TestThrow      },
	{ "loc",       TestReadLoc    }
};

std::map< std::string, const SubMain* > gMapping;

static void MakeMap()
{
	const SubMain* p = gSubs + sizeof gSubs / sizeof (SubMain);
	
	while ( --p >= gSubs )
	{
		gMapping[ p->name ] = p;
	}
}

namespace tool
{
	
	int Main( int argc, iota::argv_t argv )
	{
		NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
		
		//Assert_(argc > 0);
		
		MakeMap();
		
		std::string message;
		
		if (argc <= 1)
		{
			typedef std::map< std::string, const SubMain* >::const_iterator const_iterator;
			
			for ( const_iterator it = gMapping.begin();  it != gMapping.end();  ++it )
			{
				const char* name = it->second->name;
				
				message += name;
				message += "\n";
			}
			
			p7::write( p7::stdout_fileno, message );
			
			return 0;
		}
		
		std::string arg1 = argv[1];
		
		const SubMain* sub = gMapping[ arg1 ];
		
		if ( sub == NULL )
		{
			message = "No such test '" + arg1 + "'.\n";
			
			p7::write( p7::stderr_fileno, message );
			
			return 1;
		}
		
		return sub->proc( argc, argv );
	}

}

