/*
	var_string.hh
	-------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef PLUS_VARSTRING_HH
#define PLUS_VARSTRING_HH

// plus
#include "plus/string.hh"


namespace plus
{
	
	class var_string : public string
	{
		public:
			var_string()
			{
			}
			
			var_string( const char*    p,
			            size_type      length,
			            delete_policy  policy ) : string( p, length, policy )
			{
			}
			
			var_string( const char* p, size_type length ) : string( p, length )
			{
			}
			
			var_string( const char* p, const char* q ) : string( p, q )
			{
			}
			
			var_string( const char* s ) : string( s )
			{
			}
			
			var_string( const string& other ) : string( other )  {}
			
			var_string& operator=( const string& other )
			{
				string::operator=( other );
				
				return *this;
			}
			
			var_string& operator=( const char* s )  { assign( s );  return *this; }
	};
	
}

#endif

