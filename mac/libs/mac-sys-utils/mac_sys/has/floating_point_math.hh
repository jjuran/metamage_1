/*
	floating_point_math.hh
	----------------------
*/

#ifndef MACSYS_HAS_FLOATINGPOINTMATH_HH
#define MACSYS_HAS_FLOATINGPOINTMATH_HH


namespace mac {
namespace sys {
	
#ifdef __MC68K__
	
	char** AppPacks[ 8 ] : 0x0AB8;
	
	inline
	int has_floating_point_math()
	{
		/*
			Return true if 'PACK' 4 has been loaded.
		*/
		
		return (int) AppPacks[ 4 ];
	}
	
#else
	
	inline
	int has_floating_point_math()
	{
		return true;
	}
	
#endif
	
}
}

#endif
