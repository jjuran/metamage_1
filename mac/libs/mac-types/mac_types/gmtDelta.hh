/*
	gmtDelta.hh
	-----------
*/

#ifndef MACTYPES_GMTDELTA_HH
#define MACTYPES_GMTDELTA_HH


namespace mac   {
namespace types {

#ifdef __MC68K__

inline
asm
int gmtDelta_seconds( long gmtDelta : __D0 )
{
	/*
		gmtDelta is $xxUVWXYZ.
		
		We need to sign-extend three bytes to four.
	*/
	
	SWAP     D0        // $WXYZxxUV
	EXT.W    D0        // $WXYZSTUV
	SWAP     D0        // $STUVWXYZ
}

#else

inline
int gmtDelta_seconds( long gmtDelta )
{
	if ( gmtDelta & 0x00800000 )
	{
		gmtDelta |= 0xFF000000;
	}
	else
	{
		gmtDelta &= 0x00FFFFFF;
	}
	
	return gmtDelta;
}

#endif

}
}

#endif
