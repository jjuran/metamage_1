/*	==========
 *	AsyncIO.hh
 *	==========
 */

#ifndef GENIE_ASYNCIO_HH
#define GENIE_ASYNCIO_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_FILES_TYPES_FSDIRID_HH
#include "Mac/Files/Types/FSDirID.hh"
#endif
#ifndef MAC_FILES_TYPES_FSDIRSPEC_HH
#include "Mac/Files/Types/FSDirSpec.hh"
#endif
#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#include "Mac/Files/Types/FSFileRefNum.hh"
#endif
#ifndef MAC_FILES_TYPES_FSIOPERM_HH
#include "Mac/Files/Types/FSIOPerm.hh"
#endif
#ifndef MAC_FILES_TYPES_FSIOPOSMODE_HH
#include "Mac/Files/Types/FSIOPosMode.hh"
#endif
#ifndef MAC_FILES_TYPES_FSVOLUMEREFNUM_HH
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#endif

#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
#endif

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

