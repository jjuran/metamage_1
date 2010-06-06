/*	==========
 *	AsyncIO.hh
 *	==========
 */

#ifndef GENIE_ASYNCIO_HH
#define GENIE_ASYNCIO_HH

// nucleus
#include "nucleus/owned.hh"

// Nitrogen
#include "Mac/Files/Types/FSDirID.hh"
#include "Mac/Files/Types/FSDirSpec.hh"
#include "Mac/Files/Types/FSFileRefNum.hh"
#include "Mac/Files/Types/FSIOPerm.hh"
#include "Mac/Files/Types/FSIOPosMode.hh"
#include "Mac/Files/Types/FSVolumeRefNum.hh"

#include "Nitrogen/Str.hh"

// MacIO
#include "MacIO/EOF_Policy.hh"
#include "MacIO/ThrowOSStatus.hh"

// Genie
#include "Genie/Process/AsyncYield.hh"


#ifndef O_MAC_ASYNC
#define O_MAC_ASYNC  0
#endif


namespace Genie
{
	
	typedef MacIO::Throw_All   FNF_Throws;
	typedef MacIO::Return_FNF  FNF_Returns;
	
	
	SInt32 FSRead( MacIO::EOF_Policy  policy,
	               Mac::FSFileRefNum  file,
	               Mac::FSIOPosMode   positionMode,
	               SInt32             positionOffset,
	               SInt32             requestCount,
	               void *             buffer,
	               bool               async );
	
	
	SInt32 FSWrite( Mac::FSFileRefNum  file,
	                Mac::FSIOPosMode   positionMode,
	                SInt32             positionOffset,
	                SInt32             requestCount,
	                const void *       buffer,
	                bool               async );
	
	
	// FSpGetCatInfo
	// -------------
	
	template < class Policy >
	typename Policy::Result
	//
	FSpGetCatInfo( CInfoPBRec&          pb,
	               bool                 async,
	               Mac::FSVolumeRefNum  vRefNum,
	               Mac::FSDirID         dirID,
	               unsigned char*       name,
	               SInt16               index = 0 );
	
	template <>
	void FSpGetCatInfo< FNF_Throws >( CInfoPBRec&          pb,
	                                  bool                 async,
	                                  Mac::FSVolumeRefNum  vRefNum,
	                                  Mac::FSDirID         dirID,
	                                  unsigned char*       name,
	                                  SInt16               index );
	
	template <>
	bool FSpGetCatInfo< FNF_Returns >( CInfoPBRec&          pb,
	                                   bool                 async,
	                                   Mac::FSVolumeRefNum  vRefNum,
	                                   Mac::FSDirID         dirID,
	                                   unsigned char*       name,
	                                   SInt16               index );
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	FSpGetCatInfo( CInfoPBRec&    pb,
	               bool           async,
	               const FSSpec&  item )
	{
		Nitrogen::Str255 name = item.name;
		
		return FSpGetCatInfo< Policy >( pb,
		                                async,
		                                Mac::FSVolumeRefNum( item.vRefNum ),
		                                Mac::FSDirID       ( item.parID   ),
		                                name );
	}
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	FSpGetCatInfo( CInfoPBRec&          pb,
	               bool                 async,
	               Mac::FSVolumeRefNum  vRefNum,
	               Mac::FSDirID         dirID )
	{
		return FSpGetCatInfo< Policy >( pb,
		                                async,
		                                vRefNum,
		                                dirID,
		                                NULL,
		                                -1 );
	}
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	FSpGetCatInfo( CInfoPBRec&            pb,
	               bool                   async,
	               const Mac::FSDirSpec&  dir )
	{
		return FSpGetCatInfo< Policy >( pb,
		                                async,
		                                dir.vRefNum,
		                                dir.dirID );
	}
	
	
	// FSMakeFSSpec
	// ------------
	
	template < class Policy >
	FSSpec FSMakeFSSpec( Mac::FSVolumeRefNum   vRefNum,
	                     Mac::FSDirID          dirID,
	                     const unsigned char*  name );
	
	template <>
	FSSpec FSMakeFSSpec< FNF_Throws >( Mac::FSVolumeRefNum   vRefNum,
	                                   Mac::FSDirID          dirID,
	                                   const unsigned char*  name );
	
	template <>
	FSSpec FSMakeFSSpec< FNF_Returns >( Mac::FSVolumeRefNum   vRefNum,
	                                    Mac::FSDirID          dirID,
	                                    const unsigned char*  name );
	
	template < class Policy >
	inline FSSpec FSMakeFSSpec( const Mac::FSDirSpec&  dir,
	                            const unsigned char*   name )
	{
		return FSMakeFSSpec< Policy >( dir.vRefNum, dir.dirID, name );
	}
	
	
	nucleus::owned< Mac::FSFileRefNum >
	//
	FSpOpenDF( const FSSpec&  spec,
	           Mac::FSIOPerm  permissions );
	
	
	nucleus::owned< Mac::FSFileRefNum >
	//
	FSpOpenRF( const FSSpec&  spec,
	           Mac::FSIOPerm  permissions );
	
}

#endif

