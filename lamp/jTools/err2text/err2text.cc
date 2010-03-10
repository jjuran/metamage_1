/*	===========
 *	err2text.cc
 *	===========
 */

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif
#if UNIVERSAL_INTERFACES_VERSION < 0x0400
#ifndef __FILETYPESANDCREATORS__
#include <FileTypesAndCreators.h>  // for sigMPWShell
#endif
#else
enum { sigMPWShell = 'MPS ' };
#endif

// Standard C++
#include <algorithm>
#include <string>
#include <vector>

// Standard C
#include <string.h>

// iota
#include "iota/decimal.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/functions/write.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	struct UnrecognizedSysErrsDotErrFormat  {};
	struct CorruptedSysErrsDotErrFile  {};
	
	
	struct toc_header
	{
		UInt16 count;
		UInt16 magic;
	};
	
	struct toc_entry
	{
		SInt16 errnum;
		UInt16 offset;
	};
	
	static FSSpec global_SysErrs_dot_err;
	
	static std::vector< toc_entry > global_toc_entries;
	
	
	static FSSpec Find_SysErrsDotErr()
	{
		const N::OSType sigMPWShell = N::OSType( ::sigMPWShell );
		
		FSSpec fsspec = N::DTGetAPPL( sigMPWShell );
		
		const unsigned char* new_name = "\p" "SysErrs.err";
		
		memcpy( fsspec.name, new_name, 1 + new_name[0] );
		
		return fsspec;
	}
	
	static OSErr get_error_from_entry( toc_entry entry )
	{
		return entry.errnum;
	}
	
	static UInt32 length_of_toc()
	{
		return 2 + 2 + global_toc_entries.size() * 4;
	}
	
	template < class DataType, class InputStream >
	void ReadData( InputStream& stream, DataType& outData )
	{
		int bytes = io::read( stream, reinterpret_cast< char* >( &outData ), sizeof outData );
		
		if ( bytes != sizeof outData )
		{
			throw CorruptedSysErrsDotErrFile();
		}
	}
	
	template < class DataType, class InputStream >
	void ReadDataArray( InputStream& stream, DataType* outData, unsigned int count )
	{
		int bytes = io::read( stream, reinterpret_cast< char* >( outData ), sizeof outData * count );
		
		if ( bytes != sizeof outData * count )
		{
			throw CorruptedSysErrsDotErrFile();
		}
	}
	
	static void read_toc_entries( const FSSpec& errFile )
	{
		using N::fsRdPerm;
		
		n::owned< N::FSFileRefNum > fileH( N::FSpOpenDF( errFile, fsRdPerm ) );
		
		toc_header header;
		
		ReadData( fileH, header );
		
		// FIXME:  These should probably return errors or throw exceptions or something.
		switch ( header.magic )
		{
			case 0x0000:
			case 0xFFFF:
				break;
			
			default:
				// Unrecognized SysErrs.err format (unlikely, given that MPW is not undergoing active development)
				throw UnrecognizedSysErrsDotErrFormat();
		}
		
		global_toc_entries.resize( header.count );
		
		ReadDataArray( fileH, &global_toc_entries.front(), header.count );
	}
	
	class matching_errnum
	{
		private:
			OSErr itsErr;
		
		public:
			matching_errnum( OSErr err ) : itsErr( err )
			{
			}
			
			bool operator()( toc_entry entry ) const
			{
				return entry.errnum == itsErr;
			}
	};
	
	static UInt16 get_error_offset( OSErr err )
	{
		typedef std::vector< toc_entry >::const_iterator const_iterator;
		
		const_iterator it = std::find_if( global_toc_entries.begin(),
		                                  global_toc_entries.end(),
		                                  matching_errnum( err ) );
		
		const bool found = it != global_toc_entries.end();
		
		return found ? it->offset : 0;
	}
	
	static std::string get_string_at_offset( UInt16 offset )
	{
		using N::fsRdPerm;
		
		n::owned< N::FSFileRefNum > fileH( N::FSpOpenDF( global_SysErrs_dot_err, fsRdPerm ) );
		
		N::SetFPos( fileH, N::fsFromStart, offset );
		
		enum { bufSize = 512 };
		char buf[ bufSize ];
		
		buf[ bufSize - 1 ] = '\0';
		
		io::read( fileH, buf, bufSize - 1 );
		
		ASSERT( buf[ bufSize - 1 ] == '\0' );
		
		return std::string( buf );
	}
	
	static std::string lookup_error( OSErr err )
	{
		const UInt16 offset = get_error_offset( err );
		
		if ( offset == 0 )
		{
			return "";
		}
		
		if ( offset < length_of_toc() )
		{
			throw CorruptedSysErrsDotErrFile();
		}
		
		return get_string_at_offset( offset );
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		global_SysErrs_dot_err = Find_SysErrsDotErr();
		
		read_toc_entries( global_SysErrs_dot_err );
		
		for ( int i = 1;  i < argc;  ++i )
		{
			int errnum = iota::parse_decimal( argv[ i ] );
			
			// look up and print
			
			std::string error = lookup_error( errnum );
			
			error += "\n";
			
			p7::write( p7::stdout_fileno, error );
		}
		
		return 0;
	}

}

