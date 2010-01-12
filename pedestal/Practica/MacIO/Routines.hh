/*	===========
 *	Routines.hh
 *	===========
 */

#ifndef MACIO_ROUTINES_HH
#define MACIO_ROUTINES_HH

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __FILES__
#include <Files.h>
#endif

// MacIO
#include "MacIO/ParamBlocks.hh"


namespace MacIO
{
	
	struct HOpenDF_Traits : HParamBlock_Traits
	{
		static ::OSErr Async( PB& pb )
		{
			return ::PBHOpenDFAsync( &pb );
		}
		
		static ::OSErr Sync( PB& pb )
		{
			return ::PBHOpenDFSync( &pb );
		}
	};
	
	struct HOpenRF_Traits : HParamBlock_Traits
	{
		static ::OSErr Async( PB& pb )
		{
			return ::PBHOpenRFAsync( &pb );
		}
		
		static ::OSErr Sync( PB& pb )
		{
			return ::PBHOpenRFSync( &pb );
		}
	};
	
	struct Read_Traits : ParamBlock_Traits
	{
		static ::OSErr Async( PB& pb )
		{
			return ::PBReadAsync( &pb );
		}
		
		static ::OSErr Sync( PB& pb )
		{
			return ::PBReadSync( &pb );
		}
	};
	
	struct Write_Traits : ParamBlock_Traits
	{
		static ::OSErr Async( PB& pb )
		{
			return ::PBWriteAsync( &pb );
		}
		
		static ::OSErr Sync( PB& pb )
		{
			return ::PBWriteSync( &pb );
		}
	};
	
	struct GetCatInfo_Traits : CInfoPB_Traits
	{
		static ::OSErr Async( PB& pb )
		{
			return ::PBGetCatInfoAsync( &pb );
		}
		
		static ::OSErr Sync( PB& pb )
		{
			return ::PBGetCatInfoSync( &pb );
		}
	};
	
	struct MakeFSSpec_Traits : HParamBlock_Traits
	{
		static ::OSErr Async( PB& pb )
		{
			return ::PBMakeFSSpecAsync( &pb );
		}
		
		static ::OSErr Sync( PB& pb )
		{
			return ::PBMakeFSSpecSync( &pb );
		}
	};
	
}

#endif

