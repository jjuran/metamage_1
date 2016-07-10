/*
	dirname.cc
	----------
*/

#include "plus/posix/dirname.hh"

// plus
#include "plus/string.hh"


namespace plus
{
	
	string dirname( const string& path )
	{
		typedef string::size_type size_t;
		
		const char*  data = path.data();
		size_t       size = path.size();
		
		if ( size == 0 )
		{
			return ".";
		}
		
		const char* p = data + size;
		
		while ( *--p == '/' )
		{
			if ( p == data )
			{
				return "/";
			}
		}
		
		while ( p > data  &&  *--p != '/' )
		{
		}
		
		if ( p == data )
		{
			return *p == '/' ? "/" : ".";
		}
		
		return path.substr( 0, p - data );
	}
	
}
