// MacFiles/Classic.hh
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef MACFILES_CLASSIC_HH
#define MACFILES_CLASSIC_HH

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
	
	struct FSSpec_Io_Details
	{
		typedef FSSpec file_spec;
		
		typedef FSDirSpec optimized_directory_spec;
		
		typedef ConstStr63Param filename_parameter;
		
		typedef Nitrogen::Str63 filename_result;
		
		typedef CInfoPBRec file_catalog_record;
		
		typedef Nitrogen::FSFileRefNum stream;
		
		typedef SInt32 byte_count;
		
		typedef SInt32 position_offset;
	};
	
}

namespace io
{
	
	template <> struct filespec_traits< FSSpec              > : public Nitrogen::FSSpec_Io_Details {};
	template <> struct filespec_traits< Nitrogen::FSDirSpec > : public Nitrogen::FSSpec_Io_Details {};
	
	template <> struct iostream_traits< Nitrogen::FSFileRefNum > : public Nitrogen::FSSpec_Io_Details {};
	
	// Get file info
	
	inline Nitrogen::FSDirSpec get_preceding_directory( const FSSpec& file, overload = overload() )
	{
		return nucleus::make< Nitrogen::FSDirSpec >( Nitrogen::FSVolumeRefNum( file.vRefNum ),
		                                             Nitrogen::FSDirID       ( file.parID   ) );
	}
	
	inline Nitrogen::FSDirSpec get_preceding_directory( const Nitrogen::FSDirSpec& dir, overload = overload() )
	{
		return get_preceding_directory( Nitrogen::FSMakeFSSpec( dir ) );
	}
	
	// Path descent
	
	namespace path_descent_operators
	{
		
		inline FSSpec operator/( const Nitrogen::FSDirSpec& dir, const unsigned char* name )
		{
			return Nitrogen::FSMakeFSSpec( dir, name );
		}
		
		inline FSSpec operator/( const FSSpec& dir, const unsigned char* name )
		{
			return Nitrogen::FSpMake_FSDirSpec( dir ) / name;
		}
		
		template < unsigned char n >
		inline FSSpec operator/( const Nitrogen::FSDirSpec& dir, const Nitrogen::Str< n > & name )
		{
			return dir / static_cast< const unsigned char* >( name );
		}
		
		template < unsigned char n >
		inline FSSpec operator/( const FSSpec& dir, const Nitrogen::Str< n > & name )
		{
			return dir / static_cast< const unsigned char* >( name );
		}
		
		template < class String >
		inline FSSpec operator/( const Nitrogen::FSDirSpec& dir, const String& name )
		{
			return dir / Nitrogen::Str63( name );
		}
		
		template < class String >
		inline FSSpec operator/( const FSSpec& dir, const String& name )
		{
			return dir / Nitrogen::Str63( name );
		}
		
	}
	
	// Existence
	
	inline bool item_exists     ( const Nitrogen::FSDirSpec& dir, overload = overload() )  { return true; }
	inline bool file_exists     ( const Nitrogen::FSDirSpec& dir, overload = overload() )  { return false; }
	inline bool directory_exists( const Nitrogen::FSDirSpec& dir, overload = overload() )  { return true; }
	
	inline bool item_exists( const FSSpec& item, overload )
	{
		CInfoPBRec pb;
		
		return Nitrogen::FSpGetCatInfo( item, pb, Nitrogen::FNF_Returns() );
	}
	
	inline bool item_is_directory( const HFileInfo& hFileInfo, overload = overload() )
	{
		return hFileInfo.ioFlAttrib & kioFlAttribDirMask;
	}
	
	inline bool item_is_directory( const CInfoPBRec& cInfo, overload = overload() )
	{
		return item_is_directory( cInfo.hFileInfo );
	}
	
	inline bool item_is_file( const CInfoPBRec& cInfo, overload = overload() )
	{
		return !item_is_directory( cInfo );
	}
	
	inline bool file_exists( const FSSpec& file, overload = overload() )
	{
		CInfoPBRec pb;
		
		const bool exists = Nitrogen::FSpGetCatInfo( file, pb, Nitrogen::FNF_Returns() );
		
		return exists  &&  item_is_file( pb );
	}
	
	inline bool directory_exists( const FSSpec& dir, overload = overload() )
	{
		CInfoPBRec pb;
		
		const bool exists = Nitrogen::FSpGetCatInfo( dir, pb, Nitrogen::FNF_Returns() );
		
		return exists  &&  item_is_directory( pb );
	}
	
	// Delete
	
	inline void delete_file( const FSSpec& file, overload = overload() )
	{
		Nitrogen::FSpDelete( file );
	}
	
	inline void delete_file_only( const FSSpec& file, overload = overload() )
	{
		if ( directory_exists( file ) )
		{
			Nitrogen::ThrowOSStatus( notAFileErr );
		}
		
		delete_file( file );
	}
	
	inline void delete_empty_directory( const FSSpec& dir, overload = overload() )
	{
		Nitrogen::FSpDelete( dir );
	}
	
	inline void delete_empty_directory( const Nitrogen::FSDirSpec& dir, overload = overload() )
	{
		delete_empty_directory( Nitrogen::FSMakeFSSpec( dir ) );
	}
	
	inline void delete_empty_directory_only( const FSSpec& dir, overload = overload() )
	{
		if ( file_exists( dir ) )
		{
			Nitrogen::ThrowOSStatus( dirNFErr );
		}
		
		delete_empty_directory( dir );
	}
	
	inline void delete_empty_directory_only( const Nitrogen::FSDirSpec& dir, overload = overload() )
	{
		delete_empty_directory( dir );
	}
	
	// Open
	
	nucleus::owned< Mac::FSFileRefNum >
	open_data_fork( const FSSpec& file, Mac::FSIOPerm perm );
	
	inline nucleus::owned< Nitrogen::FSFileRefNum > open_for_reading( const FSSpec& file, overload = overload() )
	{
		return open_data_fork( file, Mac::fsRdPerm );
	}
	
	inline nucleus::owned< Nitrogen::FSFileRefNum > open_for_writing( const FSSpec& file, overload = overload() )
	{
		return open_data_fork( file, Mac::fsWrPerm );
	}
	
	inline nucleus::owned< Nitrogen::FSFileRefNum > open_for_io( const FSSpec& file, overload = overload() )
	{
		return open_data_fork( file, Mac::fsRdWrPerm );
	}
	
	inline nucleus::owned< Nitrogen::FSFileRefNum > open_truncated( const FSSpec& file, overload = overload() )
	{
		nucleus::owned< Mac::FSFileRefNum > result = open_data_fork( file, Mac::fsWrPerm );
		
		Nitrogen::SetEOF( result, 0 );
		
		return result;
	}
	
	// Stream operations
	
	inline SInt32 get_file_size( Nitrogen::FSFileRefNum stream, overload = overload() )
	{
		return Nitrogen::GetEOF( stream );
	}
	
	template < class ByteCount >
	inline SInt32 read( Nitrogen::FSFileRefNum input, char* data, ByteCount byteCount, overload = overload() )
	{
		return Nitrogen::FSRead( input, byteCount, data, Nitrogen::ThrowEOF_Never() );
	}
	
	template < class ByteCount >
	inline SInt32 write( Nitrogen::FSFileRefNum output, const char* data, ByteCount byteCount, overload = overload() )
	{
		return Nitrogen::FSWrite( output, byteCount, data );
	}
	
}

#endif
