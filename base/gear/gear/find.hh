/*
	gear/find.hh
	------------
*/

#ifndef GEAR_FIND_HH
#define GEAR_FIND_HH

// iota
#include "iota/byte_range.hh"


namespace gear
{
	
	static inline
	bool char_matches( char a, char b )
	{
		return a == b;
	}
	
	static inline
	bool char_matches( char c, ::iota::byte_range br )
	{
		return contains( br, c );
	}
	
	bool char_matches( char c, const unsigned char* chars );
	
	
	template < class Pattern >
	const char* find_first_match( const char*  p,
	                              const char*  end,
	                              Pattern      pattern,
	                              const char*  _default = 0,
	                              bool         negated  = false )
	{
		for ( ;  p != end;  ++p )
		{
			if ( char_matches( *p, pattern ) - negated )
			{
				return p;
			}
		}
		
		return _default;
	}
	
	template < class Pattern >
	const char* find_last_match( const char*  p,
	                             const char*  end,
	                             Pattern      pattern,
	                             const char*  _default = 0,
	                             bool         negated  = false )
	{
		const char* begin = p;
		
		p = end;
		
		while ( p != begin )
		{
			if ( char_matches( *--p, pattern ) - negated )
			{
				return p;
			}
		}
		
		return _default;
	}
	
	
	template < class Pattern >
	inline const char* find_first_nonmatch( const char*  p,
	                                        const char*  end,
	                                        Pattern      pattern,
	                                        const char*  _default = 0 )
	{
		return find_first_match( p, end, pattern, _default, true );
	}
	
	template < class Pattern >
	inline const char* find_last_nonmatch( const char*  p,
	                                       const char*  end,
	                                       Pattern      pattern,
	                                       const char*  _default = 0 )
	{
		return find_last_match( p, end, pattern, _default, true );
	}
	
	
	const char* find_first_match( const char*  p,
	                              const char*  end,
	                              const char*  sub,
	                              unsigned     sub_length,
	                              const char*  _default = 0 );
	
	const char* find_last_match( const char*  p,
	                             const char*  end,
	                             const char*  sub,
	                             unsigned     sub_length,
	                             const char*  _default = 0 );
	
	
	template < class Pattern >
	inline const char* find_first_match( const char*  p,
	                                     unsigned     length,
	                                     Pattern      pattern,
	                                     const char*  _default = 0,
	                                     bool         negated  = false )
	{
		return find_first_match( p, p + length, pattern, _default );
	}
	
	template < class Pattern >
	inline const char* find_last_match( const char*  p,
	                                    unsigned     length,
	                                    Pattern      pattern,
	                                    const char*  _default = 0,
	                                    bool         negated  = false )
	{
		return find_last_match( p, p + length, pattern, _default );
	}
	
	
	template < class Pattern >
	inline const char* find_first_nonmatch( const char*  p,
	                                        unsigned     length,
	                                        Pattern      pattern,
	                                        const char*  _default = 0 )
	{
		return find_first_match( p, p + length, pattern, _default, true );
	}
	
	template < class Pattern >
	inline const char* find_last_nonmatch( const char*  p,
	                                       unsigned     length,
	                                       Pattern      pattern,
	                                       const char*  _default = 0 )
	{
		return find_last_match( p, p + length, pattern, _default, true );
	}
	
	
	inline const char* find_first_match( const char*  p,
	                                     unsigned     length,
	                                     const char*  sub,
	                                     unsigned     sub_length,
	                                     const char*  _default = 0 )
	{
		return find_first_match( p, p + length, sub, sub_length, _default );
	}
	
	inline const char* find_last_match( const char*  p,
	                                    unsigned     length,
	                                    const char*  sub,
	                                    unsigned     sub_length,
	                                    const char*  _default = 0 )
	{
		return find_last_match( p, p + length, sub, sub_length, _default );
	}
	
}

#endif
