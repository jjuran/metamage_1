/*	===========
 *	err2text.cc
 *	===========
 */

// Standard C++
#include <algorithm>
#include <vector>

// gear
#include "gear/parse_decimal.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/open.hh"
#include "poseven/functions/pread.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/write.hh"

// one_path
#include "one_path/find_MPW_dir.hh"

// Orion
#include "Orion/Main.hh"

#ifndef __MACTYPES__
typedef unsigned short UInt16;
typedef          short SInt16, OSErr;
#endif


namespace tool
{
	
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
	
	static p7::fd_t global_SysErrs_dot_err_fd;
	
	static std::vector< toc_entry > global_toc_entries;
	
	
	static plus::string Find_SysErrsDotErr()
	{
		plus::var_string path = find_MPW_dir();
		
		path += "/SysErrs.err";
		
		return path;
	}
	
	static OSErr get_error_from_entry( toc_entry entry )
	{
		return entry.errnum;
	}
	
	static size_t length_of_toc()
	{
		return 2 + 2 + global_toc_entries.size() * 4;
	}
	
	template < class DataType, class InputStream >
	void ReadData( InputStream& stream, DataType& outData )
	{
		int bytes = p7::read( stream, (char*) &outData, sizeof outData );
		
		if ( bytes != sizeof outData )
		{
			throw CorruptedSysErrsDotErrFile();
		}
	}
	
	template < class DataType, class InputStream >
	void ReadDataArray( InputStream& stream, DataType* outData, unsigned int count )
	{
		int bytes = p7::read( stream, (char*) outData, sizeof outData * count );
		
		if ( bytes != sizeof outData * count )
		{
			throw CorruptedSysErrsDotErrFile();
		}
	}
	
	static void read_toc_entries( p7::fd_t fileH )
	{
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
	
	static plus::string get_string_at_offset( UInt16 offset )
	{
		enum { bufSize = 512 };
		char buf[ bufSize ];
		
		buf[ bufSize - 1 ] = '\0';
		
		p7::pread( global_SysErrs_dot_err_fd, buf, bufSize - 1, offset );
		
		ASSERT( buf[ bufSize - 1 ] == '\0' );
		
		return plus::string( buf );
	}
	
	static plus::string lookup_error( OSErr err )
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
	
	
	int Main( int argc, char** argv )
	{
		global_SysErrs_dot_err_fd = p7::open( Find_SysErrsDotErr(), p7::o_rdonly ).release();
		
		read_toc_entries( global_SysErrs_dot_err_fd );
		
		for ( int i = 1;  i < argc;  ++i )
		{
			int errnum = gear::parse_decimal( argv[ i ] );
			
			// look up and print
			
			plus::var_string error = lookup_error( errnum );
			
			error += "\n";
			
			p7::write( p7::stdout_fileno, error );
		}
		
		return 0;
	}

}
