/*
	ascii.hh
	--------
*/

#ifndef CHARSETS_ASCII_HH
#define CHARSETS_ASCII_HH


namespace chars
{
	
	const char* find_non_ascii( const char* begin,
	                            const char* end,
	                            const char* fallback = 0 );  // NULL
	
}

#endif
