/*	==========
 *	testing.cc
 *	==========
 */

// Universal Interfaces
#ifndef __MACH__
#include <Events.h>
#include <Folders.h>
#include <MacErrors.h>
#endif

// Standard C
#include "errno.h"

// Standard C/C++
#include <cstdio>

// Standard C++
#include <functional>
#include <map>
#include <string>
#include <vector>

// POSIX
#include "fcntl.h"
#include "sys/stat.h"
#include "unistd.h"

// MoreFiles
#include "MoreFilesExtras.h"

// Nucleus
#include "Nucleus/Convert.h"
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/CodeFragments.h"
#include "Nitrogen/DateTimeUtils.h"
#include "Nitrogen/MacMemory.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/OSUtils.h"
//#include "Nitrogen/Resources.h"
#include "Nitrogen/Sound.h"
#include "Nitrogen/Str.h"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Nitrogen Extras / ClassicToolbox
#if !TARGET_API_MAC_CARBON
#include "ClassicToolbox/AppleTalk.h"
#include "ClassicToolbox/Serial.h"
#endif

// Nitrogen Extras / Iteration
#include "Iteration/AEDescListItems.h"
#include "Iteration/AEDescListItemDatas.h"
#include "Iteration/FSContents.h"

// Nitrogen Extras / Templates
#include "Templates/DataPointer.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"
#include "Utilities/Processes.h"
#include "Utilities/Threads.h"

// Scaffold
#include "Tests.hh"

// BitsAndBytes
#include "DecimalStrings.hh"
#include "HexStrings.hh"

// Misc
#include "CRC32.hh"
#include "MD5.hh"

// Vectoria
#include "Vectoria/Matrix.hh"
#include "Vectoria/Vector.hh"
#include "Vectoria/CrossProduct3D.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace P7 = POSeven;
namespace FS = FileSystem;
namespace NX = NitrogenExtras;
namespace O = Orion;

using BitsAndBytes::EncodeAsHex;
using BitsAndBytes::EncodeDecimal2;


static int TestUnit( int argc, const char *const argv[] )
{
	Test::SampleTester tester;
	
	tester.RunAll();
	
	const Test::FullResults& results = tester.Results();
	
	Test::Status status = Test::AggregateStatus( results );
	
	return (status + 2) % 3;  // 0 => passed, 1 => failed, 2 => missed
}

static int TestAssert( int argc, const char *const argv[] )
{
	ASSERT( "This" == "a test" );
	//TEST_ASSERT( "This" == "a test" );
	
	return 0;
}

static int TestMap( int argc, const char *const argv[] )
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
			
			std::string string = NN::Convert< std::string >( value );
			
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

static int TestVectoria( int argc, const char *const argv[] )
{
	namespace V = Vectoria;
	
	std::printf( "%s\n%s\n", PrintableValue( V::IdentityMatrix::Make< int, 1 >() ).c_str(),
	                         PrintableValue( V::IdentityMatrix::Make< int, 2 >() ).c_str() );
	
	return 0;
}


static int TestSerial( int argc, const char *const argv[] )
{
#if TARGET_API_MAC_CARBON
	
	std::fprintf( stderr, "%s\n", "No serial I/O in Carbon." );
	
	return 1;
	
#else
	
	NN::Owned< N::SerialDeviceRef > port( N::Open_SerialDevice( (argc > 2) ? argv[ 2 ]
	                                                                       : "A"       ) );
	
	using N::kSERDHandshake;
	using N::baud19200;
	using N::data8;
	using N::noParity;
	using N::stop10;
	
	N::Control< kSERDHandshake >( port.Get()->output, NN::Make< N::SerShk >() );
	
	N::SerReset( port, baud19200 | data8 | noParity | stop10 );
	
	const char* str = "AT\015";
	
	//Io::S( port ) << str;
	N::Write( port, str, sizeof str - 1 );
	
	N::Delay( 8 );
	
	std::size_t count = N::SerGetBuf( port );
	
	if (count == 0)
	{
		Io::Err << "No data available"  "\n";
		return 1;
	}
	
	std::vector< char > block( count );
	int read = N::Read( port, &block.front(), count );
	
	if (read < count)
	{
		Io::Err << "Read failed"  "\n";
		return 1;
	}
	
	char* p = &block.front();
	int remaining = read;
	int len = std::strlen(str);
	
	if (std::memcmp(str, &block.front(), len) == 0)
	{
		Io::Out << "The modem echoed the command."  "\n";
		p += len;
		remaining -= len;
	}
	
	while (remaining > 0  &&  (*p == '\015' || *p == '\012'))
	{
		p++;
		remaining--;
	}
	
	if (remaining)
	{
		const char* ok = "OK\015\012";
		if (std::memcmp(ok, p, std::strlen(ok)) == 0)
		{
			Io::Out << "The modem returned OK."  "\n";
			
		}
	}
	
	return 0;
	
#endif
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

static int TestAFP(int argc, const char *const argv[])
{
	const char* server = argv[ 2 ];
	
#if !TARGET_API_MAC_CARBON
	
	bool isRunningOSX = TestAFPServer( server );
	
	const char* supports = isRunningOSX ? "does not support"
	                                    : "supports";
	
	Io::Out << "Server '"
	        << server
	        << "' "
	        << supports
	        << " AppleTalk.\n";
	
#endif
	
	return 0;
}


static int TestDate(int argc, const char *const argv[])
{
	Io::Out << "DateTime == " << N::GetDateTime() << "\n";
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


static Boolean FSpExists(FSSpec *file)
{
	FInfo	fndrInfo;
	
	return FSpGetFInfo(file, &fndrInfo) == noErr; 
}

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

static int TestCRC16(int argc, const char *const argv[])
{
	if (argc < 3)  return 1;
	
	const char* text = argv[ 2 ];
	
	unsigned int mb3CRC = CalculateCRC( (const unsigned char*)text, std::strlen( text ) );
	
	unsigned int lcsCRC = CalcCRC     ( (const unsigned char*)text, std::strlen( text ) );
	
	Io::Out << "MB3: " << EncodeAsHex( mb3CRC ) << "\n";
	Io::Out << "LCS: " << EncodeAsHex( lcsCRC ) << "\n";
	
	return 0;
}

static int TestCRC32(int argc, const char *const argv[])
{
	if (argc < 3)  return 1;
	
	const char* text = argv[2];
	unsigned int crc = CRC32::Checksum(text, std::strlen(text));
	
	Io::Out << EncodeAsHex(crc) << "\n";
	
	return 0;
}

static MD5::Result MD5String( const char* text )
{
	return MD5::Digest( text, std::strlen( text ) * 8 );
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

static int TestMD5(int argc, const char *const argv[])
{
	if (argc < 3)  return 1;
	
	const char* text = argv[ 2 ];
	
	Io::Out << MD5Hex( text ) << "\n";
	
	return 0;
}

/*
static int TestOSX(int argc, const char *const argv[])
{
	//if (argc < 3)  return 1;
	
	bool classic = V::IsRunningInClassic();
	
	const char* maybe = classic ? "" : "not ";
	
	Io::Out << "We are " << maybe << "running in Classic.\n";
	
	return 0;
}
*/

static int TestOADC(int argc, const char *const argv[])
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
		
		Io::Out << "OpenADefaultComponent returned " << err << ".\n";
	}
	
	return 0;
}

class HasSignature
{
	private:
		OSType signature;
		ProcessInfoRec processInfo;
	
	public:
		HasSignature( OSType signature ) : signature( signature )
		{
			processInfo.processInfoLength = sizeof processInfo;
			processInfo.processName = NULL;
			processInfo.processAppSpec = NULL;
		}
		
		bool operator()( const ProcessSerialNumber& process )
		{
			N::GetProcessInformation( process, processInfo );
			return processInfo.processSignature == signature;
		}
};

static int TestProcesses( int argc, char const *const argv[] )
{
	//if (argc < 3)  return 1;
	
	ProcessSerialNumber psn = N::NoProcess();
	ProcessInfoRec info;
	Str255 name;
	
	info.processInfoLength = sizeof info;
	info.processName = name;
	info.processAppSpec = NULL;
	
	typedef N::Process_Container::const_iterator P_ci;
	
	for ( P_ci it = N::Processes().begin();  it != N::Processes().end();  ++it )
	{
		N::GetProcessInformation( *it, info );
		
		Io::Out << "'"
		        << NN::Convert< std::string >( N::FourCharCode( info.processSignature ) )
		        << "' "
		        << NN::Convert< std::string, const unsigned char* >( name )
		        << "\n";
	}
	
	N::Process_Container::const_iterator finder = std::find_if( N::Processes().begin(),
	                                                            N::Processes().end(),
	                                                            HasSignature( 'MACS' ) );
	
	if ( finder != N::Processes().end() )
	{
		Io::Out << finder->highLongOfPSN << "-" << finder->lowLongOfPSN << "\n";
	}
	
	return 0;
}

static int TestSoundInput( int argc, char const *const argv[] )
{
	//if (argc < 3)  return 1;
	
	typedef N::SoundInputDevice_Container::const_iterator SID_ci;
	
	//SID_ci it = N::SoundInputDevices().begin();
	SID_ci it ( N::SoundInputDevices().begin() );
	SID_ci end( N::SoundInputDevices().end()   );
	
	for ( ;  it != end;  ++it )
	{
		Io::Out << *it
		        << "\n";
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

static void PrintString( const std::string& s )
{
	Io::Out << s << "\n";
}

static int TestAE( int argc, char const *const argv[] )
{
	//if (argc < 3)  return 1;
	NN::Owned< AEDescList > list = N::AECreateList< false >();
	
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
	Io::Out << "Param is " << param << "\n";
	
	
}

static int TestThread( int argc, char const *const argv[] )
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
			Io::Out << "State is " << state << "\n";
		}
		catch ( N::OSStatus err )
		{
			Io::Err << "OSStatus " << err << "\n";
		}
		
		++count;
		
		if ( count >= 5 )  break;
	}
	
	Io::Out << "Looped " << count << " times.\n";
	
	return 0;
}

static std::string ReadFileData( const FSSpec& file )
{
	NN::Owned< N::FSFileRefNum > fileH( N::FSpOpenDF( file, N::fsRdPerm ) );
	
	unsigned fileSize = N::GetEOF( fileH );
	
	std::string data;
	data.resize( fileSize );
	
	int bytes = FS::Read( fileH, &data[ 0 ], fileSize );
	
	ASSERT( bytes == fileSize );
	
	return data;
}

static void DoSomethingWithServiceFile( const FSSpec& file )
{
	// Find Info.plist
	FSSpec infoPListFile = NN::Convert< N::FSDirSpec >( file ) << "Contents" & "Info.plist";
	// Read the entire file contents
	std::string infoPList = ReadFileData( NN::Convert< FSSpec >( infoPListFile ) );
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

static int TestServices( int argc, char const *const argv[] )
{
	//if (argc < 3)  return 1;
	
	N::FSDirSpec systemLibraryServices = N::RootDirectory( N::BootVolume() ) << "System"
	                                                                         << "Library"
	                                                                         << "Services";
	
	std::for_each( N::FSContents( systemLibraryServices ).begin(),
	               N::FSContents( systemLibraryServices ).end(),
	               std::ptr_fun( DoSomethingWithServiceFile ) );
	
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

static int TestNull( int argc, char const *const argv[] )
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
	
	static NX::DataPtr< FragmentImage > ReadFragmentImageFromPluginFile( const char* pathname )
	{
		NN::Owned< P7::FileDescriptor > filehandle = P7::Open( pathname, O_RDONLY );
		
		struct ::stat stat_buffer;
		
		int statted = stat( pathname, &stat_buffer );
		
		std::size_t size = stat_buffer.st_size;
		
		std::auto_ptr< FragmentImage > result;
		
		result.reset( static_cast< FragmentImage* >( ::operator new( size ) ) );
		
		int bytes = read( filehandle, reinterpret_cast< char* >( result.get() ), size );
		
		return NX::DataPtr< FragmentImage >( result, size );
	}
	
static int TestGMFShared( int argc, char const *const argv[] )
{
#if TARGET_RT_MAC_CFM
	
	if ( argc < 3 )  return 1;
	
	const char* pathname = argv[2];
	
	NX::DataPtr< FragmentImage > fragment = ReadFragmentImageFromPluginFile( pathname );
	
	int len = fragment.Len();
	
	std::printf( "Fragment length: %d bytes\n", len );
	
	NN::Owned< CFragConnectionID > one = N::GetMemFragment< N::kPrivateCFragCopy >( fragment.Get(), fragment.Len() );
	
	int* scratch;
	
	N::FindSymbol( one, "\p" "gScratch", &scratch );
	
	*scratch = 42;
	
	NN::Owned< CFragConnectionID > two = N::GetMemFragment< N::kPrivateCFragCopy >( fragment.Get(), fragment.Len() );
	
	N::FindSymbol( two, "\p" "gScratch", &scratch );
	
	std::printf( "Second connection scratch value: %d\n", *scratch );
	
#endif
	
	return 0;
}

static int TestStrError( int argc, char const *const argv[] )
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

static int TestThrow( int argc, char const *const argv[] )
{
	if ( argc < 3 )  return 1;
	
	int errnum = std::atoi( argv[2] );
	
	if ( errnum < 0 )
	{
		N::ThrowOSStatus( errnum );
	}
	
	return 0;
}

static int TestReadLoc( int argc, char const *const argv[] )
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


typedef int (*MainProcPtr)(int argc, const char *const argv[]);

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
	
	{ "serial",    TestSerial     },
	{ "afp",       TestAFP        },
	
#endif
	
	{ "date",      TestDate       },
	{ "crc16",     TestCRC16      },
	{ "crc32",     TestCRC32      },
	{ "md5",       TestMD5        },
//	{ "X",         TestOSX        },
	{ "OADC",      TestOADC       },
	{ "proc",      TestProcesses  },
	{ "si",        TestSoundInput },
	{ "ae",        TestAE         },
	{ "svcs",      TestServices   },
	{ "thread",    TestThread     },
	{ "null",      TestNull       },
	
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

int O::Main(int argc, const char *const argv[])
{
	//Assert_(argc > 0);
	
	MakeMap();
	
	if (argc <= 1)
	{
		//Io::Err << "testing: I'd like to have an argument, please.\n";
		//return 1;
		
		typedef std::map< std::string, const SubMain* >::const_iterator const_iterator;
		
		for ( const_iterator it = gMapping.begin();  it != gMapping.end();  ++it )
		{
			const char* name = it->second->name;
			
			Io::Out << name << "\n";
		}
		
		return 0;
	}
	
	std::string arg1 = argv[1];
	
	const SubMain* sub = gMapping[ arg1 ];
	
	if ( sub == NULL )
	{
		Io::Err << "No such test '" << arg1 << "'.\n";
		
		return 1;
	}
	
	return sub->proc( argc, argv );
	
	if (false)
	{
	}
	else if (arg1 == "unit")
	{
		return TestUnit(argc, argv);
	}
	else if (arg1 == "assert")
	{
		return TestAssert(argc, argv);
	}
	else if (arg1 == "map")
	{
		return TestMap(argc, argv);
	}
	else if (arg1 == "serial")
	{
		return TestSerial(argc, argv);
	}
	else if (arg1 == "afp")
	{
		return TestAFP(argc, argv);
	}
	else if (arg1 == "date")
	{
		return TestDate(argc, argv);
	}
	else if (arg1 == "crc16")
	{
		return TestCRC16(argc, argv);
	}
	else if (arg1 == "crc32")
	{
		return TestCRC32(argc, argv);
	}
	else if (arg1 == "md5")
	{
		return TestMD5(argc, argv);
	}
	/*
	else if (arg1 == "X")
	{
		return TestOSX(argc, argv);
	}
	*/
	else if (arg1 == "OADC")
	{
		return TestOADC(argc, argv);
	}
	else if ( arg1 == "proc" )
	{
		return TestProcesses( argc, argv );
	}
	else if ( arg1 == "si" )
	{
		return TestSoundInput( argc, argv );
	}
	else if ( arg1 == "ae" )
	{
		return TestAE( argc, argv );
	}
	else if ( arg1 == "svcs" )
	{
		return TestServices( argc, argv );
	}
	else if ( arg1 == "thread" )
	{
		return TestThread( argc, argv );
	}
	else if ( arg1 == "null" )
	{
		return TestNull( argc, argv );
	}
	else if ( arg1 == "gmf" )
	{
		return TestGMFShared( argc, argv );
	}
	else if ( arg1 == "strerror" )
	{
		return TestStrError( argc, argv );
	}
	else if ( arg1 == "throw" )
	{
		return TestThrow( argc, argv );
	}
	else if ( arg1 == "loc" )
	{
		return TestReadLoc( argc, argv );
	}
	else
	{
		return 0;
	}
	
	return 0;
}

