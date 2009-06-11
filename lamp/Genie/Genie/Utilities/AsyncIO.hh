/*	==========
 *	AsyncIO.hh
 *	==========
 */

#ifndef GENIE_ASYNCIO_HH
#define GENIE_ASYNCIO_HH

// Nitrogen
#include "Nitrogen/Files.h"

// MacIO
#include "MacIO/ThrowOSStatus.hh"

// Genie
#include "Genie/Process/AsyncYield.hh"


namespace Genie
{
	
	using Nitrogen::ThrowEOF_Always;
	using Nitrogen::ThrowEOF_OnZero;
	using Nitrogen::ThrowEOF_Never;
	
	typedef MacIO::Throw_All   FNF_Throws;
	typedef MacIO::Return_FNF  FNF_Returns;
	
	
	// Async read
	SInt32 FSRead( Nitrogen::FSFileRefNum  file,
	               Nitrogen::FSIOPosMode   positionMode,
	               SInt32                  positionOffset,
	               SInt32                  requestCount,
	               void *                  buffer,
	               ThrowEOF_Always         policy );
	
	SInt32 FSRead( Nitrogen::FSFileRefNum  file,
	               Nitrogen::FSIOPosMode   positionMode,
	               SInt32                  positionOffset,
	               SInt32                  requestCount,
	               void *                  buffer,
	               ThrowEOF_OnZero         policy );
	
	SInt32 FSRead( Nitrogen::FSFileRefNum  file,
	               Nitrogen::FSIOPosMode   positionMode,
	               SInt32                  positionOffset,
	               SInt32                  requestCount,
	               void *                  buffer,
	               ThrowEOF_Never          policy );
	
	// Async write
	SInt32 FSWrite( Nitrogen::FSFileRefNum  file,
	                Nitrogen::FSIOPosMode   positionMode,
	                SInt32                  positionOffset,
	                SInt32                  requestCount,
	                const void *            buffer );
	
	
	// FSpGetCatInfo
	// -------------
	
	template < class Policy >
	typename Policy::Result
	//
	FSpGetCatInfo( CInfoPBRec&               pb,
	               Nitrogen::FSVolumeRefNum  vRefNum,
	               Nitrogen::FSDirID         dirID,
	               unsigned char*            name,
	               SInt16                    index = 0 );
	
	template <>
	void FSpGetCatInfo< FNF_Throws >( CInfoPBRec&               pb,
	                                  Nitrogen::FSVolumeRefNum  vRefNum,
	                                  Nitrogen::FSDirID         dirID,
	                                  unsigned char*            name,
	                                  SInt16                    index );
	
	template <>
	bool FSpGetCatInfo< FNF_Returns >( CInfoPBRec&               pb,
	                                   Nitrogen::FSVolumeRefNum  vRefNum,
	                                   Nitrogen::FSDirID         dirID,
	                                   unsigned char*            name,
	                                   SInt16                    index );
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	FSpGetCatInfo( CInfoPBRec&    pb,
	               const FSSpec&  item )
	{
		Nitrogen::Str255 name = item.name;
		
		return FSpGetCatInfo< Policy >( pb,
		                                Nitrogen::FSVolumeRefNum( item.vRefNum ),
		                                Nitrogen::FSDirID       ( item.parID   ),
		                                name );
	}
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	FSpGetCatInfo( CInfoPBRec&               pb,
	               Nitrogen::FSVolumeRefNum  vRefNum,
	               Nitrogen::FSDirID         dirID )
	{
		return FSpGetCatInfo< Policy >( pb,
		                                vRefNum,
		                                dirID,
		                                NULL,
		                                -1 );
	}
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	FSpGetCatInfo( CInfoPBRec&                 pb,
	               const Nitrogen::FSDirSpec&  dir )
	{
		return FSpGetCatInfo< Policy >( pb,
		                                dir.vRefNum,
		                                dir.dirID );
	}
	
	
	// FSMakeFSSpec
	// ------------
	
	template < class Policy >
	FSSpec FSMakeFSSpec( Nitrogen::FSVolumeRefNum  vRefNum,
	                     Nitrogen::FSDirID         dirID,
	                     const unsigned char*      name );
	
	template <>
	FSSpec FSMakeFSSpec< FNF_Throws >( Nitrogen::FSVolumeRefNum  vRefNum,
	                                   Nitrogen::FSDirID         dirID,
	                                   const unsigned char*      name );
	
	template <>
	FSSpec FSMakeFSSpec< FNF_Returns >( Nitrogen::FSVolumeRefNum  vRefNum,
	                                    Nitrogen::FSDirID         dirID,
	                                    const unsigned char*      name );
	
	template < class Policy >
	inline FSSpec FSMakeFSSpec( const Nitrogen::FSDirSpec&  dir,
	                            const unsigned char*        name )
	{
		return FSMakeFSSpec< Policy >( dir.vRefNum, dir.dirID, name );
	}
	
	
	Nucleus::Owned< Nitrogen::FSFileRefNum >
	//
	FSpOpenDF( const FSSpec&          spec,
	           Nitrogen::FSIOPermssn  permissions );
	
	
	Nucleus::Owned< Nitrogen::FSFileRefNum >
	//
	FSpOpenRF( const FSSpec&          spec,
	           Nitrogen::FSIOPermssn  permissions );
	
}

#endif

