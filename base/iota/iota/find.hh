/*
	iota/find.hh
	------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef IOTA_FIND_HH
#define IOTA_FIND_HH


namespace iota
{
	
	const char* find_first_match( const char* p, const char* end, char c );
	const char* find_last_match ( const char* p, const char* end, char c );
	
	const char* find_first_match( const char*           p,
	                              const char*           end,
	                              const unsigned char*  chars );
	
	const char* find_last_match( const char*           p,
	                             const char*           end,
	                             const unsigned char*  chars );
	
	const char* find_first_match( const char*  p,
	                              const char*  end,
	                              const char*  sub,
	                              unsigned     sub_length );
	
	const char* find_last_match( const char*  p,
	                             const char*  end,
	                             const char*  sub,
	                             unsigned     sub_length );
	
	inline const char* find_first_match( const char* p, unsigned length, char c )
	{
		return find_first_match( p, p + length, c );
	}
	
	inline const char* find_last_match( const char* p, unsigned length, char c )
	{
		return find_last_match( p, p + length, c );
	}
	
	inline const char* find_first_match( const char*           p,
	                                     unsigned              length,
	                                     const unsigned char*  chars )
	{
		return find_first_match( p, p + length, chars );
	}
	
	inline const char* find_last_match( const char*           p,
	                                    unsigned              length,
	                                    const unsigned char*  chars )
	{
		return find_last_match( p, p + length, chars );
	}
	
	inline const char* find_first_match( const char*  p,
	                                     unsigned     length,
	                                     const char*  sub,
	                                     unsigned     sub_length )
	{
		return find_first_match( p, p + length, sub, sub_length );
	}
	
	inline const char* find_last_match( const char*  p,
	                                    unsigned     length,
	                                    const char*  sub,
	                                    unsigned     sub_length )
	{
		return find_last_match( p, p + length, sub, sub_length );
	}
	
}

#endif

