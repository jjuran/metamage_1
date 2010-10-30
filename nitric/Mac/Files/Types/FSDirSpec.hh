/*
	Mac/Files/Types/FSDirSpec.hh
	----------------------------
*/

#ifndef MAC_FILES_TYPES_FSDIRSPEC_HH
#define MAC_FILES_TYPES_FSDIRSPEC_HH

#ifndef __FILES__
#include <Files.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_MAKE_HH
#include "nucleus/make.hh"
#endif

// Nitrogen
#ifndef MAC_FILES_TYPES_FSDIRID_HH
#include "Mac/Files/Types/FSDirID.hh"
#endif
#ifndef MAC_FILES_TYPES_FSVOLUMEREFNUM_HH
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#endif


namespace Mac
{
	
	struct FSDirSpec
	{
		FSVolumeRefNum  vRefNum;
		FSDirID         dirID;
		
		FSDirSpec() : vRefNum(), dirID()
		{
		}
	};
	
	inline bool operator==( const FSDirSpec& a, const FSDirSpec& b )
	{
		return a.vRefNum == b.vRefNum
		    && a.dirID   == b.dirID;
	}
	
	inline bool operator!=( const FSDirSpec& a, const FSDirSpec& b )
	{
		return !( a == b );
	}
	
}

namespace nucleus
{
	
	template <>
	struct maker< Mac::FSDirSpec >
	{
		Mac::FSDirSpec operator()( Mac::FSVolumeRefNum  vRefNum,
		                           Mac::FSDirID         dirID ) const
		{
			Mac::FSDirSpec result;
			
			result.vRefNum = vRefNum;
			result.dirID   = dirID;
			
			return result;
		}
		
		Mac::FSDirSpec operator()( const ::DirInfo& dirInfo ) const
		{
			return operator()( Mac::FSVolumeRefNum( dirInfo.ioVRefNum ),
			                   Mac::FSDirID       ( dirInfo.ioDrDirID ) );
		}
		
		Mac::FSDirSpec operator()( const ::CInfoPBRec& cInfo ) const
		{
			return operator()( cInfo.dirInfo );
		}
	};
	
}

#endif

