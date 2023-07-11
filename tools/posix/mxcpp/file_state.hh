/*
	file_state.hh
	-------------
*/

#ifndef MXCPP_FILESTATE_HH
#define MXCPP_FILESTATE_HH

// Standard C
#include <stddef.h>

// plus
#include "plus/string.hh"

// text-input
#include "text_input/feed.hh"

// poseven
#ifndef POSEVEN_EXTRAS_FDREADER_HH
#include "poseven/extras/fd_reader.hh"
#endif


namespace tool
{
	
	extern int global_include_depth;
	
	
	class file_state
	{
		private:
			plus::string        its_line_directed_file;
			plus::string        its_path;
			const char*         its_dir_path;
			text_input::feed    its_feed;
			poseven::fd_reader  its_reader;
			size_t              its_nth_line;
			plus::string        its_logical_line;
		
		public:
			size_t  its_buffered_blank_line_count;
			
			file_state( const char*          dir,
			            const plus::string&  path,
			            poseven::fd_t        fd )
			:
				its_path    ( path ),
				its_dir_path( dir  ),
				its_reader  ( fd   ),
				its_nth_line( 0    ),
				its_buffered_blank_line_count()
			{
			}
			
			const plus::string& get_path() const  { return its_path; }
			
			const plus::string& get_FILE() const  { return its_line_directed_file; }
			size_t              get_LINE() const  { return its_nth_line;           }
			
			const plus::string* get_next_line();
			
			const plus::string* get_logical_line();
			
			void set_line( size_t nth_line, const plus::string& path );
			
			plus::string get_line_directive( bool with_file ) const;
	};
	
}

#endif
