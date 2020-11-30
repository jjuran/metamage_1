/*
	directive.cc
	------------
*/

#include "directive.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <string.h>

// gear
#include "gear/find.hh"

// Relix
#include "relix/recurse.hh"

// mxcpp
#include "conditional.hh"
#include "exception.hh"
#include "expression.hh"
#include "include.hh"
#include "macro.hh"
#include "pragma.hh"
#include "preprocess.hh"


namespace tool
{
	
	static bool globally_emitting = true;
	static bool globally_eligible_for_else = false;
	
	
	bool emitting()
	{
		return globally_emitting;
	}
	
	
	static bool do_define( const plus::string& target, const plus::string& param )
	{
		if ( globally_emitting )
		{
			define_macro( target, param );
		}
		
		return false;
	}
	
	static bool do_include( const plus::string& target, const plus::string& )
	{
		if ( globally_emitting )
		{
			relix::recurse( &include_file, target );
		}
		
		return false;
	}
	
	static bool do_if( const plus::string& target, const plus::string& )
	{
		push_conditional( globally_emitting );
		
		if ( globally_emitting )
		{
			globally_emitting = expression_is_true( target );
			
			globally_eligible_for_else = !globally_emitting;
		}
		
		return false;
	}
	
	static bool do_elif( const plus::string& target, const plus::string& )
	{
		if ( get_conditional_top() )
		{
			globally_emitting = globally_eligible_for_else && expression_is_true( target );
			
			globally_eligible_for_else = globally_eligible_for_else  &&  !globally_emitting;
		}
		
		return false;
	}
	
	static bool do_else( const plus::string&, const plus::string& )
	{
		if ( get_conditional_top() )
		{
			globally_emitting = globally_eligible_for_else;
		}
		
		return false;
	}
	
	static bool do_ifdef( const plus::string& target, const plus::string& )
	{
		push_conditional( globally_emitting );
		
		if ( globally_emitting )
		{
			globally_emitting = is_defined( target );
			
			globally_eligible_for_else = !globally_emitting;
		}
		
		return false;
	}
	
	static bool do_ifndef( const plus::string& target, const plus::string& )
	{
		push_conditional( globally_emitting );
		
		if ( globally_emitting )
		{
			globally_emitting = !is_defined( target );
			
			globally_eligible_for_else = !globally_emitting;
		}
		
		return false;
	}
	
	static bool do_endif( const plus::string&, const plus::string& )
	{
		globally_emitting = pop_conditional();
		
		if ( globally_emitting )
		{
			globally_eligible_for_else = false;
		}
		
		return false;
	}
	
	static bool do_undef( const plus::string& target, const plus::string& )
	{
		if ( globally_emitting )
		{
			undef_macro( target );
		}
		
		return false;
	}
	
	static bool do_error( const plus::string& target, const plus::string& )
	{
		if ( globally_emitting )
		{
			
		}
		
		return globally_emitting;
	}
	
	static bool do_warning( const plus::string& target, const plus::string& )
	{
		if ( globally_emitting )
		{
			
		}
		
		return globally_emitting;
	}
	
	static bool do_line( const plus::string& target, const plus::string& param )
	{
		set_line( target, param );
		
		return true;
	}
	
	static bool do_pragma( const plus::string& target, const plus::string& param )
	{
		return globally_emitting  &&  pragma( target, param );
	}
	
	
	#define DEFINE_DIRECTIVE( name )  { #name, &do_##name, &scan_##name::apply }
	
	static bool operator==( const directive& a, const plus::string& b )
	{
		return strcmp( a.name, b.c_str() ) == 0;
	}
	
	static bool operator<( const directive& a, const plus::string& b )
	{
		return strcmp( a.name, b.c_str() ) < 0;
	}
	
	
	struct scan_all
	{
		static const char* apply( const char* p, const char* q )
		{
			return q;
		}
	};
	
	struct scan_none
	{
		static const char* apply( const char* p, const char* q )
		{
			return p;
		}
	};
	
	static const unsigned char* whitespace = (const unsigned char*) "\4 \t\n\r";
	
	static inline
	const char* find_whitespace( const char* p, const char* q )
	{
		return gear::find_first_match( p, q, whitespace, q );
	}
	
	struct scan_word
	{
		static const char* apply( const char* p, const char* q )
		{
			return find_whitespace( p, q );
		}
	};
	
	struct scan_define
	{
		static const char* apply( const char* p, const char* q )
		{
			const char* space = find_whitespace( p, q );
			const char* paren = std::find      ( p, q, '(' );
			
			if ( space <= paren )
			{
				return space;
			}
			
			paren = std::find( paren, q, ')' );
			
			if ( paren == q )
			{
				throw exception( "unclosed_macro_definition" );
			}
			
			return paren + 1;
		}
	};
	
	typedef scan_all  scan_if, scan_elif, scan_include, scan_error, scan_warning;
	
	typedef scan_none scan_else, scan_endif;
	
	typedef scan_word  scan_ifdef, scan_ifndef, scan_undef, scan_pragma, scan_line;
	
	
	static const directive global_directives[] =
	{
		DEFINE_DIRECTIVE( define  ),
		DEFINE_DIRECTIVE( elif    ),
		DEFINE_DIRECTIVE( else    ),
		DEFINE_DIRECTIVE( endif   ),
		DEFINE_DIRECTIVE( error   ),
		DEFINE_DIRECTIVE( if      ),
		DEFINE_DIRECTIVE( ifdef   ),
		DEFINE_DIRECTIVE( ifndef  ),
		DEFINE_DIRECTIVE( include ),
		DEFINE_DIRECTIVE( line    ),
		DEFINE_DIRECTIVE( pragma  ),
		DEFINE_DIRECTIVE( undef   ),
		DEFINE_DIRECTIVE( warning )
	};
	
	const directive* find_directive( const plus::string& name )
	{
		const directive* begin = global_directives;
		const directive* end   = begin + sizeof global_directives / sizeof global_directives[0];
		
		const directive* it = std::lower_bound( begin, end, name );
		
		if ( it < end  &&  *it == name )
		{
			return it;
		}
		
		return NULL;
	}
	
}
