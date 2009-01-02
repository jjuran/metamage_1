/*	============
 *	FSContents.h
 *	============
 */

#ifndef FSCONTENTS_H
#define FSCONTENTS_H

// Io
#include "io/walk.hh"

// Nitrogen
#include "Nitrogen/Files.h"

// Io: MacFiles
#include "MacFiles.hh"

// Io: UnicodeMacFiles
#include "UnicodeMacFiles.hh"

// Nitrogen Extras / Iteration
#include "Iteration/IndexedValueContainer.h"


namespace Nitrogen
{
	
	template < class Specifics >
	struct FSContents_Specifics
	{
		typedef std::size_t      size_type;
		typedef std::ptrdiff_t   difference_type;
		typedef Nitrogen::Str63  value_type;
		
		//typedef typename Specifics::filename_result           value_type;
		typedef typename Specifics::optimized_directory_spec  ContainerType;
		
		static std::size_t Size( const ContainerType& dir )
		{
			CInfoPBRec pb;
			
			FSpGetCatInfo( Nucleus::Convert< FSDirSpec >( dir ), pb );
			
			return pb.dirInfo.ioDrNmFls;
		}
		
		static value_type GetValue( const ContainerType& dir, size_type position )
		{
			CInfoPBRec pb;
			::Str255 name;
			
			FSpGetCatInfo( Nucleus::Convert< FSDirSpec >( dir ), position + 1, pb, name );  // one-based
			
			return name;
		}
		
		typedef ContainerType ContainerState;
		typedef ContainerState IteratorState;
	};
	
	typedef IndexedValueContainer< FSContents_Specifics< FSSpec_Io_Details > > FSSpecContents_Container;
	typedef IndexedValueContainer< FSContents_Specifics< FSRef_Io_Details  > > FSRefContents_Container;
	
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
		return Nitrogen::FSSpecContents_Container( Nitrogen::FSDirSpec( dir ) );
	}
	
	inline Nitrogen::FSRefContents_Container directory_contents( const FSRef& dir, overload = overload() )
	{
		return Nitrogen::FSRefContents_Container( dir );
	}
	
}

#endif

