/*
	TabCompletion.cc
	----------------
*/

#include "Genie/TabCompletion.hh"

// POSIX
#include <sys/stat.h>

// plus
#include "plus/mac_utf8.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/functions/resolve_pathname.hh"
#include "vfs/primitives/listdir.hh"
#include "vfs/primitives/lookup.hh"

// relix
#include "relix/api/root.hh"

// Genie
#include "Genie/Utilities/shell_unsafe.hh"


namespace Genie
{

static
bool begins_with( const plus::string& s, const plus::string& word )
{
	return s.substr( 0, word.size() ) == word;
}

TabCompletion tab_complete( const vfs::node* dir, plus::var_string& s, int i, int j )
{
	using plus::mac_from_utf8;
	
	const size_t input_size = j - i;
	
	const plus::string& input = s.substr( i, input_size );
	
	const char* p = input.begin();
	const char* q = p + input_size;
	const char* r = q;
	
	while ( q > p )
	{
		if ( *--q == ' ' )
		{
			++q;
			
			break;
		}
	}
	
	const size_t arg_offset = q - p;
	
	const plus::string& argument = input.substr( arg_offset );
	
	while ( r > q )
	{
		if ( *--r == '/' )
		{
			++r;
			
			break;
		}
	}
	
	const size_t subdirs_size  = r - q;
	const size_t prefix_offset = subdirs_size;
	
	const plus::string& subdirs = argument.substr( 0, subdirs_size );
	const plus::string& prefix  = argument.substr( prefix_offset );
	
	const plus::string prefix_UTF8 = plus::utf8_from_mac( prefix );
	
	vfs::node_ptr subdir = resolve_pathname( *relix::root(),
	                                         subdirs.data(),
	                                         subdirs_size,
	                                         *dir );
	
	vfs::dir_contents list;
	
	listdir( *subdir, list );
	
	std::vector< vfs::dir_entry >& entries = list.get();
	
	int n_matches = 0;
	
	size_t last_match = 0;
	
	for ( size_t i = 0;  i < entries.size();  ++i )
	{
		vfs::dir_entry& node = entries[ i ];
		
		if ( begins_with( node.name, prefix_UTF8 ) )
		{
			++n_matches;
			
			last_match = i;
		}
		else
		{
			node.name.reset();
		}
	}
	
	if ( n_matches == 0 )
	{
		return TabCompletion_failed;
	}
	
	size_t prefix_size = input_size - arg_offset - prefix_offset;
	
	if ( n_matches > 1 )
	{
		size_t n = 0;
		size_t i = 0;
		
		size_t shortest_length = (size_t) -1;
		
		while ( n < n_matches )
		{
			size_t len = entries[ i ].name.size();
			
			if ( len > 0 )
			{
				if ( len < shortest_length )
				{
					shortest_length = len;
				}
				
				if ( i != n )
				{
					/*
						This doesn't swap the inode numbers,
						but we only care about the name.
					*/
					
					entries[ n ].name.swap( entries[ i ].name );
				}
				
				++n;
			}
			
			++i;
		}
		
		n = prefix_UTF8.size();
		
		while ( n < shortest_length )
		{
			char c = entries[ 0 ].name[ n ];
			
			for ( size_t i = 1;  i < n_matches;  ++i )
			{
				if ( entries[ i ].name[ n ] != c )
				{
					goto breakout;
				}
			}
			
			++n;
		}
		
	breakout:
		
		size_t offset = prefix_size;
		
		n -= offset;
		
		const plus::string& addon = entries[ 0 ].name.substr( offset, n );
		
		s.append( mac_from_utf8( safe_prefix( addon ) ) );
		
		return TabCompletion_stuck;
	}
	
	TabCompletion tc = TabCompletion_done;
	
	const plus::string& match = entries[ last_match ].name;
	
	plus::string match_substr = match.substr( prefix_size );
	
	ssize_t index = shell_unsafe_index( match_substr );
	
	if ( index >= 0 )
	{
		match_substr = match_substr.substr( 0, index );
		
		tc = TabCompletion_stuck;
	}
	
	s.append( mac_from_utf8( match_substr ) );
	
	if ( index < 0 )
	{
		vfs::node_ptr file = lookup( *subdir, match );
		
		try
		{
			resolve_links_in_place( *relix::root(), file );
			
			s += S_ISDIR( file->filemode() ) ? '/' : ' ';
		}
		catch ( ... )
		{
		}
	}
	
	return tc;
}

}
