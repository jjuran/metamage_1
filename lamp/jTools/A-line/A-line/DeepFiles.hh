/*	============
 *	DeepFiles.hh
 *	============
 */

#ifndef ALINE_DEEPFILES_HH
#define ALINE_DEEPFILES_HH

// plus
#include "plus/string.hh"

// A-line
#include "A-line/StringVector.hh"


namespace tool
{
	
	typedef bool (*deep_file_filter)( const plus::string& );
	
	
	class DeepFileSearch
	{
		private:
			deep_file_filter  filter;
			StringVector      result;
		
		public:
			DeepFileSearch( deep_file_filter filter ) : filter( filter )  {}
			
			operator const StringVector&() const  { return result; }
			
			DeepFileSearch& SearchItem( plus::string item );
			
			DeepFileSearch& SearchDir( const plus::string& dir );
	};
	
	
	inline
	StringVector DeepFiles( const plus::string& item, deep_file_filter filter )
	{
		return DeepFileSearch( filter ).SearchItem( item );
	}
	
}

#endif
