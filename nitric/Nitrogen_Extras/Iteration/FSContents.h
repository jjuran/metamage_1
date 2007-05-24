/*	============
 *	FSContents.h
 *	============
 */

#ifndef FSCONTENTS_H
#define FSCONTENTS_H

// Nitrogen Extras / Iteration
#include "Iteration/IndexedValueContainer.h"


namespace Nitrogen
{
	
	struct FSSpecContents_Specifics
	{
		typedef FSSpec    FileSpec;
		typedef FSDirSpec DirSpec;
	};
	
	struct FSRefContents_Specifics
	{
		typedef FSRef FileSpec;
		typedef FSRef DirSpec;
	};
	
	template < class Specifics >
	struct FSContents_Specifics
	{
		typedef std::size_t                   size_type;
		typedef std::ptrdiff_t                difference_type;
		typedef typename Specifics::FileSpec  value_type;
		
		typedef typename Specifics::DirSpec  ContainerType;
		
		static std::size_t Size( ContainerType dir )
		{
			CInfoPBRec pb;
			
			return FSpGetCatInfo( Nucleus::Convert< FSDirSpec >( dir ), pb ).dirInfo.ioDrNmFls;
		}
		
		static value_type GetValue( ContainerType dir, size_type position )
		{
			CInfoPBRec pb;
			Str255 name;
			
			FSpGetCatInfo( Nucleus::Convert< FSDirSpec >( dir ), position + 1, pb, name );  // one-based
			
			using namespace io::path_descent_operators;
			
			return Nucleus::Convert< value_type >( Nucleus::Convert< FSDirSpec >( dir ) / name );
		}
		
		typedef ContainerType ContainerState;
		typedef ContainerState IteratorState;
	};
	
	typedef IndexedValueContainer< FSContents_Specifics< FSSpecContents_Specifics > > FSSpecContents_Container;
	typedef IndexedValueContainer< FSContents_Specifics< FSRefContents_Specifics  > > FSRefContents_Container;
	
	inline FSSpecContents_Container FSContents( const FSDirSpec& dir )
	{
		return FSSpecContents_Container( dir );
	}
	
	inline FSRefContents_Container FSContents( const FSRef& dir )
	{
		return FSRefContents_Container( dir );
	}
	
}

#endif

