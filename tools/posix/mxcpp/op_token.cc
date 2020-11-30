/*
	op_token.cc
	-----------
*/

#include "op_token.hh"

// Standard C++
#include <algorithm>


namespace tool
{
	
	struct op_token
	{
		const char* name;
		
		operator char() const  { return name[0]; }
	};
	
	static const op_token op_tokens[] =
	{
		{ "!="  },
		{ "##"  },
		{ "%="  },
		{ "&&"  },
		{ "&="  },
		{ "*="  },
		{ "++"  },
		{ "+="  },
		{ "--"  },
		{ "-="  },
		{ "->"  },
		{ "->*" },
		{ ".*"  },
		{ "..." },
		{ "/="  },
		{ "::"  },
		{ "<<"  },
		{ "<<=" },
		{ "<="  },
		{ "=="  },
		{ ">="  },
		{ ">>"  },
		{ ">>=" },
		{ "^="  },
		{ "|="  },
		{ "||"  },
	};
	
	/*
	static bool operator<( const op_token& op, char c )
	{
		return op[0] < c;
	}
	
	static bool operator<( char c, const char* op )
	{
		return c < op[0];
	}
	*/
	
	static inline bool matches( const char* pattern, const char* s )
	{
		while ( *pattern != '\0' )
		{
			if ( *pattern++ != *s++ )
			{
				return false;
			}
		}
		
		return true;
	}
	
	static const op_token* longest_match( const op_token*  begin,
	                                      const op_token*  end,
	                                      const char*      text )
	{
		while ( end-- > begin )
		{
			if ( matches( end->name, text ) )
			{
				return end;
			}
		}
		
		return NULL;
	}
	
	const char* find_op_token( const char* p )
	{
		const op_token* begin = op_tokens;
		const op_token* end   = begin + sizeof op_tokens / sizeof op_tokens[0];
		
		const op_token* it = std::lower_bound( begin, end, p[0] );
		
		const op_token* result = NULL;
		
		if ( it != end  &&  it->name[0] == p[0] )
		{
			const op_token* bound = std::upper_bound( it, end, p[0] );
			
			result = longest_match( it, bound, p );
		}
		
		return result ? result->name : NULL;
	}
	
}
