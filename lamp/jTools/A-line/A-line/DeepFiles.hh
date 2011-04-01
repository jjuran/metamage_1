/*	============
 *	DeepFiles.hh
 *	============
 */

#ifndef ALINE_DEEPFILES_HH
#define ALINE_DEEPFILES_HH

// C++
#include <vector>

// plus
#include "plus/string.hh"


namespace tool
{
	
	typedef bool (*deep_file_filter)( const plus::string& );
	
	
	class DeepFileSearch
	{
		private:
			deep_file_filter filter;
			std::vector< plus::string >  result;
		
		public:
			DeepFileSearch( deep_file_filter filter ) : filter( filter )  {}
			
			operator const std::vector< plus::string >&() const  { return result; }
			
			DeepFileSearch& SearchItem( plus::string item );
			
			DeepFileSearch& SearchDir( const plus::string& dir );
	};
	
	
	inline std::vector< plus::string > DeepFiles( const plus::string& item, deep_file_filter filter )
	{
		return DeepFileSearch( filter ).SearchItem( item );
	}
	
}

#endif

