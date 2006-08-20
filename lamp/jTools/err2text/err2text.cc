/*	===========
 *	err2text.cc
 *	===========
 */

// Universal Interfaces
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif
#if UNIVERSAL_INTERFACES_VERSION < 0x0400
#include <FileTypesAndCreators.h>  // for sigMPWShell
#else
enum { sigMPWShell = 'MPS ' };
#endif

// C++ Standard Library
#include <algorithm>
#include <functional>
#include <string>
#include <vector>

// Nitrogen Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen Extras / Templates
#include "Templates/FunctionalExtensions.h"

// Io
#include "Io/Files.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// err2text
#include "FSLocator.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace O = Orion;

namespace ext = N::STLExtensions;

using std::string;

using FSLocator::FSLocatorChainedT;
using FSLocator::FSLocatorAppBySignature;
using FSLocator::FSLinkNewName;


struct UnrecognizedSysErrsDotErrFormat  {};
struct CorruptedSysErrsDotErrFile  {};

typedef FSLocatorChainedT< FSLocatorAppBySignature,
                           FSLinkNewName >           NextToApp;

class SysErrsDotErrLocator : public NextToApp
{
	public:
		SysErrsDotErrLocator() : NextToApp( FSLocatorAppBySignature( sigMPWShell ),
		                                    FSLinkNewName( "SysErrs.err" ) )
		{}
};

struct TOCEntry
{
	static OSErr GetError( TOCEntry entry )  { return entry.errNum; }
	
	SInt16 errNum;
	UInt16 offset;
};

class SysErrsDotErrTOC
{
	private:
		UInt16 myCount;
		std::vector< TOCEntry > myTOCEntries;
	
	public:
		SysErrsDotErrTOC( const FSSpec& errFile );
		UInt16 CountRecords() const  { return myCount; }
		UInt32 Length() const  { return 2 + 2 + myCount * 4; }
		UInt16 OffsetOf( OSErr err );
};

template < class DataType, class InputStream >
void ReadData( InputStream& stream, DataType& outData )
{
	int bytes = Io::Read( stream, reinterpret_cast< char* >( &outData ), sizeof outData );
	
	if ( bytes != sizeof outData )
	{
		throw CorruptedSysErrsDotErrFile();
	}
}

template < class DataType, class InputStream >
void ReadDataArray( InputStream& stream, DataType* outData, unsigned int count )
{
	int bytes = Io::Read( stream, reinterpret_cast< char* >( outData ), sizeof outData * count );
	
	if ( bytes != sizeof outData * count )
	{
		throw CorruptedSysErrsDotErrFile();
	}
}

SysErrsDotErrTOC::SysErrsDotErrTOC( const FSSpec& errFile )
:
	myCount( 0 )
{
	NN::Owned< N::FSFileRefNum > fileH( N::FSpOpenDF( errFile, fsRdPerm ) );
	
	ReadData( fileH, myCount );
	
	UInt16 reserved;
	ReadData( fileH, reserved );
	
	// FIXME:  These should probably return errors or throw exceptions or something.
	switch (reserved)
	{
		case 0x0000:
		case 0xFFFF:
			break;
		
		default:
			// Unrecognized SysErrs.err format (unlikely, given that MPW is not undergoing active development)
			throw UnrecognizedSysErrsDotErrFormat();
	}
	
	myTOCEntries.resize( myCount );
	ReadDataArray( fileH, &myTOCEntries.front(), myCount );
}

UInt16 SysErrsDotErrTOC::OffsetOf( OSErr err )
{
	typedef std::vector< TOCEntry >::const_iterator const_iterator;
	
	const_iterator it = std::find_if( myTOCEntries.begin(),
	                                  myTOCEntries.end(),
	                                  ext::compose1( std::bind1st( std::equal_to< SInt16 >(),
	                                                               err ),
	                                                 std::ptr_fun( TOCEntry::GetError ) ) );
	bool found = it != myTOCEntries.end();
	
	return found ? it->offset : 0;
}

class SysErrsDotErrText
{
	private:
		FSSpec myErrFile;
	
	public:
		SysErrsDotErrText( const FSSpec& errFile ) : myErrFile( errFile )  {}
		string GetStringAt( UInt16 offset );
};

string SysErrsDotErrText::GetStringAt( UInt16 offset )
{
	NN::Owned< N::FSFileRefNum > fileH( N::FSpOpenDF( myErrFile, fsRdPerm ) );
	
	N::SetFPos( fileH, fsFromStart, offset );
	
	enum { bufSize = 512 };
	char buf[ bufSize ];
	buf[ bufSize - 1 ] = '\0';
	Io::Read( fileH, buf, bufSize - 1 );
	ASSERT(buf[bufSize - 1] == '\0');
	
	return string( buf );
}

class Errortable
{
	private:
		SysErrsDotErrLocator myLocator;
		FSSpec myFile;
		SysErrsDotErrTOC myTOC;
		SysErrsDotErrText myText;
	
	public:
		Errortable()
		:
			myFile( myLocator.Locate() ),
			myTOC ( myFile             ),
			myText( myFile             )
		{}
		
		string Lookup( OSErr err );
};

string Errortable::Lookup( OSErr err )
{
	UInt16 offset = myTOC.OffsetOf( err );
	
	if ( offset == 0 )
	{
		return "";
	}
	
	if ( offset < myTOC.Length() )
	{
		throw CorruptedSysErrsDotErrFile();
	}
	
	return myText.GetStringAt( offset );
}


int O::Main( int argc, const char *const argv[] )
{
	Errortable table;
	
	for ( int i = 1;  i < argc;  i++ )
	{
		int errNum = std::atoi( argv[ i ] );
		// look up and print
		Io::Out << table.Lookup( errNum ) << "\n";
	}
	
	return 0;
}

