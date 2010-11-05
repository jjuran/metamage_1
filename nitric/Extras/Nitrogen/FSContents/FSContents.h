/*	============
 *	FSContents.h
 *	============
 */

#ifndef FSCONTENTS_H
#define FSCONTENTS_H

// Io
#ifndef IO_WALK_HH
#include "io/walk.hh"
#endif

// nucleus
#ifndef NUCLEUS_INDEXEDVALUESEQUENCE_HH
#include "nucleus/indexed_value_sequence.hh"
#endif

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif

// Io: MacFiles
#include "MacFiles/Classic.hh"
#include "MacFiles/Unicode.hh"


namespace Nitrogen
{
	
	inline void FSpGetCatInfo( const FSDirSpec&  dir,
	                           SInt16            index,
	                           CInfoPBRec&       cInfo,
	                           StringPtr         name )
	{
		nucleus::initialize< CInfoPBRec >( cInfo,
		                                   dir.vRefNum,
		                                   dir.dirID,
		                                   name,
		                                   index );
		
		PBGetCatInfoSync( cInfo, FNF_Throws() );
	}
	
	inline void FSpGetCatInfo( const FSDirSpec&  dir,
	                           CInfoPBRec&       cInfo,
	                           StringPtr         name )
	{
		FSpGetCatInfo( dir, -1, cInfo, name );
	}
	
	template < class Specifics >
	struct FSContents_Specifics
	{
		typedef std::size_t      size_type;
		typedef std::ptrdiff_t   difference_type;
		typedef Nitrogen::Str63  value_type;
		
		//typedef typename Specifics::filename_result           value_type;
		typedef typename Specifics::optimized_directory_spec  context_type;
		
		static std::size_t size( const context_type& dir )
		{
			CInfoPBRec pb;
			
			FSpGetCatInfo( nucleus::convert< FSDirSpec >( dir ), pb, NULL );
			
			return pb.dirInfo.ioDrNmFls;
		}
		
		static value_type get_value( const context_type& dir, size_type position )
		{
			CInfoPBRec pb;
			::Str255 name;
			
			FSpGetCatInfo( nucleus::convert< FSDirSpec >( dir ), position + 1, pb, name );  // one-based
			
			return name;
		}
	};
	
	typedef nucleus::indexed_value_sequence< FSContents_Specifics< FSSpec_Io_Details > > FSSpecContents_Container;
	typedef nucleus::indexed_value_sequence< FSContents_Specifics< FSRef_Io_Details  > > FSRefContents_Container;
	
	inline FSSpecContents_Container FSContents( const FSDirSpec& dir )
	{
		return FSSpecContents_Container( dir );
	}
	
	inline FSRefContents_Container FSContents( const FSRef& dir )
	{
		return FSRefContents_Container( dir );
	}
	
}

namespace io
{
	
	template <> struct directory_contents_traits< Nitrogen::FSDirSpec >
	{
		typedef Nitrogen::FSSpecContents_Container container_type;
	};
	
	template <> struct directory_contents_traits< FSSpec >
	{
		typedef Nitrogen::FSSpecContents_Container container_type;
	};
	
	template <> struct directory_contents_traits< FSRef >
	{
		typedef Nitrogen::FSRefContents_Container container_type;
	};
	
	
	inline Nitrogen::FSSpecContents_Container directory_contents( const Nitrogen::FSDirSpec& dir, overload = overload() )
	{
		return Nitrogen::FSSpecContents_Container( dir );
	}
	
	inline Nitrogen::FSSpecContents_Container directory_contents( const FSSpec& dir, overload = overload() )
	{
		return Nitrogen::FSSpecContents_Container( Nitrogen::FSpMake_FSDirSpec( dir ) );
	}
	
	inline Nitrogen::FSRefContents_Container directory_contents( const FSRef& dir, overload = overload() )
	{
		return Nitrogen::FSRefContents_Container( dir );
	}
	
}

#endif

