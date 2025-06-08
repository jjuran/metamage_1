/*
	utf16.cc
	--------
*/

#include "encoding/utf16.hh"


namespace chars
{

typedef unsigned short uint16_t;

static inline
bool is_surrogate( utf16_t uc )
{
	return uint16_t( uc - 0xD800 ) < 0x0800;
}

enum surrogacy_t
{
	Surrogate_none     =      0,
	Surrogate_leading  =      1,
	Surrogate_trailing = -32767,
};

static inline
surrogacy_t surrogacy( utf16_t uc )
{
	/*
		Returns either 1, -32767, or 0.
	*/
	
	short s = is_surrogate( uc ) ? (uc << 5 & 0x8000) + 1 : 0;
	
	return surrogacy_t( s );
}

static inline
bool is_surrogate( surrogacy_t s )
{
	return s;
}

static inline
bool leads( surrogacy_t s )
{
	return s > 0;
}

static inline
bool trails( surrogacy_t s )
{
	return s < 0;
}

unichar_t get_next_code_point_from_utf16( const utf16_t*&  p,
                                          const utf16_t*   end )
{
	const utf16_t uc = *p++;
	
	surrogacy_t s = surrogacy( uc );
	
	if ( ! is_surrogate( s ) )
	{
		return uc;
	}
	
	utf16_t uc2 = 0;
	
	if ( leads( s )  &&  p < end  &&  trails( surrogacy( uc2 = *p++ ) ) )
	{
		return + 0x10000
		       | (uc  & 0x3FF) << 10
		       | (uc2 & 0x3FF);
	}
	
	return (unichar_t) -1;
}

}
