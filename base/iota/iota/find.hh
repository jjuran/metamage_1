/*
	iota/find.hh
	------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef IOTA_FIND_HH
#define IOTA_FIND_HH


namespace iota
{
	
	const char* find_first_match( const char* p, unsigned length, char c );
	const char* find_last_match ( const char* p, unsigned length, char c );
	
	const char* find_first_match( const char*           p,
	                              unsigned              length,
	                              const unsigned char*  chars );
	
	const char* find_last_match( const char*           p,
	                             unsigned              length,
	                             const unsigned char*  chars );
	
}

#endif

