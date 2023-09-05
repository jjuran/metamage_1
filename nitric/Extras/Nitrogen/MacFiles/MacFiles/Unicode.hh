// MacFiles/Unicode.hh
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef MACFILES_UNICODE_HH
#define MACFILES_UNICODE_HH

// Io
#ifndef IO_IO_HH
#include "io/io.hh"
#endif
#ifndef IO_FILES_HH
#include "io/files.hh"
#endif

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif


namespace Nitrogen
{
	
	struct FSRef_Io_Details
	{
		typedef FSRef file_spec;
		
		typedef FSRef optimized_directory_spec;
		
		typedef const HFSUniStr255& filename_parameter;
		
		typedef HFSUniStr255 filename_result;
		
		typedef Nitrogen::FSForkRefNum stream;
		
		typedef ::ByteCount byte_count;
		
		typedef SInt64 position_offset;
	};
	
}

namespace io
{
	
	template <> struct filespec_traits< FSRef  > : public Nitrogen::FSRef_Io_Details {};
	
	template <> struct iostream_traits< Nitrogen::FSForkRefNum > : public Nitrogen::FSRef_Io_Details {};
	
	// Get file info
	
	inline FSRef get_preceding_directory( const FSRef& file, overload = overload() )
	{
		FSCatalogInfo info;
		
		FSRef parent;
		
		Nitrogen::FSGetCatalogInfo( file, kFSCatInfoNone, &info, NULL, NULL, &parent );
		
		return parent;
	}
	
	// Path descent
	
	namespace path_descent_operators
	{
		
		inline FSRef operator/( const FSRef& dir, const HFSUniStr255& name )
		{
			return Nitrogen::FSMakeFSRefUnicode( dir, name );
		}
		
	}
	
	// Existence
	
	inline bool item_exists( const FSRef& item, overload = overload() )  { return true; }
	
	inline bool directory_exists( const FSRef& dir, overload = overload() )
	{
		FSCatalogInfo info;
		
		Nitrogen::FSGetCatalogInfo( dir, kFSCatInfoNodeFlags, &info, NULL, NULL, NULL );
		
		return info.nodeFlags & kFSNodeIsDirectoryMask;
	}
	
	inline bool file_exists( const FSRef& file, overload = overload() )
	{
		return !directory_exists( file );
	}
	
	// Delete
	
	inline void delete_file( const FSRef& file, overload = overload() )
	{
		Nitrogen::FSDeleteObject( file );
	}
	
	inline void delete_file_only( const FSRef& file, overload = overload() )
	{
		if ( directory_exists( file ) )
		{
			Nitrogen::ThrowOSStatus( notAFileErr );
		}
		
		delete_file( file );
	}
	
	inline void delete_empty_directory( const FSRef& dir, overload = overload() )
	{
		Nitrogen::FSDeleteObject( dir );
	}
	
	inline void delete_empty_directory_only( const FSRef& dir, overload = overload() )
	{
		if ( file_exists( dir ) )
		{
			Nitrogen::ThrowOSStatus( dirNFErr );
		}
		
		delete_empty_directory( dir );
	}
	
	// Open
	
	inline nucleus::owned< Nitrogen::FSForkRefNum > open_for_reading( const FSRef& file, overload = overload() )
	{
		return Nitrogen::FSOpenFork( file, Nitrogen::fsRdPerm );
	}
	
	inline nucleus::owned< Nitrogen::FSForkRefNum > open_for_writing( const FSRef& file, overload = overload() )
	{
		return Nitrogen::FSOpenFork( file, Nitrogen::fsWrPerm );
	}
	
	inline nucleus::owned< Nitrogen::FSForkRefNum > open_for_io( const FSRef& file, overload = overload() )
	{
		return Nitrogen::FSOpenFork( file, Nitrogen::fsRdWrPerm );
	}
	
	inline nucleus::owned< Nitrogen::FSForkRefNum > open_truncated( const FSRef& file, overload = overload() )
	{
		nucleus::owned< Nitrogen::FSForkRefNum > result = Nitrogen::FSOpenFork( file, Nitrogen::fsRdWrPerm );
		
		Nitrogen::FSSetForkSize( result, Nitrogen::fsFromStart, 0 );
		
		return result;
	}
	
	// Stream operations
	
	inline SInt64 get_file_size( Nitrogen::FSForkRefNum stream, overload = overload() )
	{
		return Nitrogen::FSGetForkSize( stream );
	}
	
	template < class ByteCount >
	inline ByteCount read( Nitrogen::FSForkRefNum input, char* data, ByteCount byteCount, overload = overload() )
	{
		return Nitrogen::FSReadFork( input, byteCount, data, Nitrogen::ThrowEOF_Never() );
	}
	
	template < class ByteCount >
	inline ByteCount write( Nitrogen::FSForkRefNum output, const char* data, ByteCount byteCount, overload = overload() )
	{
		return Nitrogen::FSWriteFork( output, byteCount, data );
	}
	
}

#endif
