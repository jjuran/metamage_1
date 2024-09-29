/*
	string-utils.cc
	---------------
*/

#include "vlib/string-utils.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// debug
#include "debug/assert.hh"

// plus
#include "plus/reverse.hh"
#include "plus/var_string.hh"

// bignum
#include "bignum/integer.hh"

// vxo
#include "vxo/container.hh"

// vxo-string
#include "vxo-string/string.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/error.hh"
#include "vlib/is_function.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/string.hh"


#undef mempcpy

#define STRLEN( s ) (sizeof "" s - 1)

#define UNDEFINED  "???"


namespace vlib
{
	
	using ::mempcpy;
	
	typedef vxo::Array_of< vxo::String >  Array_of_String;
	
	static inline
	char* mempcpy( char* p, const plus::string& s )
	{
		return (char*) mempcpy( p, s.data(), s.size() );
	}
	
	static inline
	char* mempcpy( char* p, const char* s )
	{
		return (char*) mempcpy( p, s, strlen( s ) );
	}
	
	
	static
	const char* reproduce_op( op_type op )
	{
		switch ( op )
		{
			case Op_denote:     return "\xa9" " (";  // " (" <-> ')' + 0x80
			case Op_subscript:  return "\xdd" "[";  // '[' <-> ']' + 0x80
			
			case Op_record: return "record ";
			case Op_module: return "module ";
			case Op_export: return "export ";
			case Op_var:    return "var ";
			case Op_let:    return "let ";
			case Op_const:  return "const ";
			case Op_def:    return "def ";
			case Op_for:    return "for ";
			
			case Op_function:     return "";
			case Op_prototype:
			case Op_named_unary:  return " ";
			
			case Op_getter:
			case Op_member:  return ".";
			
			case Op_take:  return "take ";
			
			case Op_empower:  return "^";
			
			case Op_recv:     return "<=";
			case Op_preinc:   return "++";
			case Op_predec:   return "--";
			case Op_postinc:  return "++";
			case Op_postdec:  return "--";
			case Op_present:  return "??";
			
			case Op_unary_plus:   return "+";
			case Op_unary_minus:  return "-";
			case Op_unary_negate:  return "~";
			case Op_unary_deref:  return "*";
			case Op_unary_refer:  return "&";
			case Op_unary_count:  return "(+) ";
			
			case Op_postfix_0_1:  return "?";
			case Op_postfix_0_N:  return "*";
			case Op_postfix_1_N:  return "+";
			
			case Op_named_param:  return "=";
			
			case Op_multiply:  return " * ";
			case Op_divide:    return " / ";
			case Op_DIV:       return " div ";
			case Op_percent:   return " % ";
			case Op_modulo:    return " mod ";
			
			case Op_add:       return " + ";
			case Op_subtract:  return " - ";
			
			case Op_gamut:  return " .. ";
			case Op_delta:  return " -> ";
			
			case Op_intersection:  return " & ";
			case Op_exclusion:     return " ~ ";
			case Op_union:         return " | ";
			
			case Op_in:  return " in ";
			
			case Op_not:  return "not ";
			
			case Op_repeat:  return " (*) ";
			
			case Op_try:    return "try ";
			case Op_catch:  return " catch ";
			
			case Op_auto:    return "auto ";
			case Op_begin:   return "begin ";
			case Op_lambda:  return "lambda ";
			
			case Op_via:  return " via ";
			case Op_map:  return " map ";
			case Op_gap:  return " gap ";
			case Op_pop:  return " pop ";
			case Op_ver:  return " ver ";
			case Op_per:  return " per ";
			
			case Op_lt:  return " < ";
			case Op_gt:  return " > ";
			
			case Op_lte:  return " <= ";
			case Op_gte:  return " >= ";
			
			case Op_cmp:  return " <=> ";
			
			case Op_isa:       return " isa ";
			case Op_equal:     return " == ";
			case Op_unequal:   return " != ";
			
			case Op_and:  return " and ";
			case Op_or:   return " or ";
			
			case Op_mapping:  return " => ";
			
			case Op_if:    return "if ";
			case Op_then:  return " then ";
			case Op_else:  return " else ";
			case Op_else_if:  return " else if ";
			
			case Op_while:    return "while ";
			case Op_do:       return "do ";
			case Op_do_2:     return " do ";
			case Op_while_2:  return " while ";
			
			case Op_break:     return "break";
			case Op_continue:  return "continue";
			
			case Op_duplicate:    return " = ";
			case Op_approximate:  return " := ";
			case Op_concat_with:  return " .= ";
			case Op_increase_by:  return " += ";
			case Op_decrease_by:  return " -= ";
			case Op_multiply_by:  return " *= ";
			case Op_divide_by:    return " /= ";
			case Op_div_int_by:   return " /:= ";
			case Op_percent_by:   return " %= ";
			case Op_setAND_with:  return " &= ";
			case Op_setXOR_with:  return " ~= ";
			case Op_setOR_with:   return " |= ";
			
			case Op_forward_init:  return " >-";
			case Op_reverse_init:  return "-< ";
			
			case Op_move:  return " <- ";
			case Op_swap:  return " <-> ";
			case Op_push:  return " <-- ";
			case Op_send:  return " <== ";
			
			case Op_assert:  return "assert ";
			case Op_return:  return "return ";
			case Op_throw:   return "throw ";
			
			case Op_end:  return "; ";
			
			default:  return " <<op>> ";
		}
	}
	
	static inline
	bool use_quotes( stringification mode )
	{
		return mode > Stringified_to_print;
	}
	
	static inline
	bool use_parens( stringification mode )
	{
		return mode > Stringified_to_print;
	}
	
	static inline
	bool use_commas( stringification mode )
	{
		return mode > Stringified_to_print;
	}
	
	
	class string_maker
	{
		private:
			Array_of_String its_cache;
			
			size_t its_index;
		
		public:
			string_maker() : its_index()
			{
			}
			
			size_t composite_length( const Value&     value,
			                         stringification  mode,
			                         bool             print_parens = true );
			
			char* make_string( char*            p,
			                   const Value&     value,
			                   stringification  mode,
			                   bool             print_parens = true );
	};
	
	static
	const stringify* get_str_methods( const Value& v, stringification mode )
	{
		if ( const dispatch* methods = v.dispatch_methods() )
		{
			if ( const stringifiers* s = methods->to_string )
			{
				if ( mode < 0 )
				{
					return s->bin;
				}
				
				if ( mode > 0  &&  s->rep != NULL )
				{
					return s->rep;
				}
				
				return s->str;
			}
		}
		
		return NULL;
	}
	
	size_t string_maker::composite_length( const Value&     value,
	                                       stringification  mode,
	                                       bool             print_parens )
	{
		if ( const stringify* str = get_str_methods( value, mode ) )
		{
			try
			{
				return get_stringified_size( *str, value );
			}
			catch ( const dynamic_data_exception& )
			{
				its_cache.push_back( str->make( value ) );
				
				return its_cache.back().get().size();
			}
		}
		
		if ( mode == Stringified_to_pack )
		{
			switch ( value.type() )
			{
				case Value_empty_array:
					return 0;
				
				case Value_pair:
					break;  // handled below
				
				default:
					THROW( "byte packing not supported for type" );
			}
		}
		
		switch ( value.type() )
		{
			case Value_NIL:
			case Value_nothing:
			case Value_dummy_operand:
				return 0;
			
			case Value_undefined:
				return STRLEN( UNDEFINED );
			
			case Value_empty_list:  // "()", ""
			case Value_empty_array:  // "[]", ""
				return 2 * use_parens( mode );
			
			case Value_pair:
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in composite_length()" );
				break;
		}
		
		Expr* expr = value.expr();
		
		ASSERT( expr != NULL );
		
		if ( expr->op == Op_array  ||  (uint8_t) expr->op == Op_block )
		{
			const size_t n_brackets = use_parens( mode ) * 2;
			
			if ( Expr* e2 = expr->right.expr() )
			{
				if ( (uint8_t) e2->op == Op_scope )
				{
					expr = e2;
				}
			}
			
			if ( is_empty_list( expr->right ) )
			{
				return n_brackets;  // "[]" or "{}"
			}
			
			return n_brackets + composite_length( expr->right, mode, false );
		}
		
		size_t total = composite_length( expr->left, mode );
		
		total += 2 * use_parens( mode ) * print_parens;  // 2 for parentheses
		
		if ( use_parens( mode )  &&  expr->op != Op_list )
		{
			total += strlen( reproduce_op( expr->op ) );
			
			const bool subparens = expr->op != Op_subscript;
			
			if ( subparens  ||  ! is_empty_list( expr->right ) )
			{
				total += composite_length( expr->right, mode, subparens );
			}
			
			return total;
		}
		
		total += 2 * use_commas( mode );  // 2 for first comma
		
		while ( Expr* next = expr->right.expr() )
		{
			if ( next->op != Op_list )
			{
				break;
			}
			
			total += composite_length( next->left, mode );
			
			total += 2 * use_commas( mode );
			
			expr = next;
		}
		
		return total + composite_length( expr->right, mode );
	}
	
	char* string_maker::make_string( char*            p,
	                                 const Value&     value,
	                                 stringification  mode,
	                                 bool             print_parens )
	{
		if ( const stringify* str = get_str_methods( value, mode ) )
		{
			try
			{
				return copy_stringified( p, *str, value );
			}
			catch ( const dynamic_data_exception& )
			{
				const plus::string& s = its_cache[ its_index++ ];
				
				return mempcpy( p, s );
			}
		}
		
		if ( mode == Stringified_to_pack )
		{
			switch ( value.type() )
			{
				case Value_empty_array:
					return p;
				
				case Value_pair:
					break;  // handled below
				
				default:
					THROW( "byte packing not supported for type" );
			}
		}
		
		switch ( value.type() )
		{
			case Value_NIL:
			case Value_nothing:
			case Value_dummy_operand:
				return p;
			
			case Value_undefined:
				return (char*) mempcpy( p, STR_LEN( UNDEFINED ) );
			
			case Value_empty_list:  // ""
			case Value_empty_array:
				if ( use_parens( mode ) )
				{
					const char* tokens[] = { "()", "[]" };
					
					int index = value.type() - Value_empty_list;
					
					const char* empty = tokens[ index ];
					
					p = (char*) mempcpy( p, empty, 2 );
				}
				
				return p;
			
			case Value_pair:
				if ( ! use_parens( mode )  &&  is_function( value ) )
				{
					THROW( "stringification of compound function" );
				}
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in make_string()" );
				break;
		}
		
		Expr* expr = value.expr();
		
		if ( expr->op == Op_array )
		{
			if ( use_parens( mode ) )
			{
				*p++ = '[';
			}
			
			if ( ! is_empty_list( expr->right ) )
			{
				p = make_string( p, expr->right, mode, false );
			}
			
			if ( use_parens( mode ) )
			{
				*p++ = ']';
			}
			
			return p;
		}
		
		if ( (uint8_t) expr->op == Op_block )
		{
			*p++ = '{';
			
			if ( Expr* e2 = expr->right.expr() )
			{
				if ( (uint8_t) e2->op == Op_scope )
				{
					expr = e2;
				}
			}
			
			if ( ! is_empty_list( expr->right ) )
			{
				p = make_string( p, expr->right, mode, false );
			}
			
			*p++ = '}';
			
			return p;
		}
		
		if ( use_parens( mode )  &&  print_parens )
		{
			*p++ = '(';
		}
		
		p = make_string( p, expr->left, mode );
		
		if ( use_parens( mode ) )
		{
			if ( expr->op != Op_list )
			{
				const char* op_str = reproduce_op( expr->op );
				const char* r = op_str;
				
				while ( int8_t( *r ) < 0 )
				{
					++r;
				}
				
				p = mempcpy( p, r );
				
				const bool subparens = expr->op != Op_subscript;
				
				if ( subparens  ||  ! is_empty_list( expr->right ) )
				{
					p = make_string( p, expr->right, mode, subparens );
				}
				
				for ( const char* q = op_str;  q < r; )
				{
					*p++ = *q++ & 0x7f;
				}
				
				if ( print_parens )
				{
					*p++ = ')';
				}
				
				return p;
			}
			
			p = (char*) mempcpy( p, STR_LEN( ", " ) );
		}
		
		while ( Expr* next = expr->right.expr() )
		{
			if ( next->op != Op_list )
			{
				break;
			}
			
			p = make_string( p, next->left, mode );
			
			if ( use_commas( mode ) )
			{
				p = (char*) mempcpy( p, STR_LEN( ", " ) );
			}
			
			expr = next;
		}
		
		p = make_string( p, expr->right, mode );
		
		if ( use_parens( mode )  &&  print_parens )
		{
			*p++ = ')';
		}
		
		return p;
	}
	
	plus::string make_string( const Value& value, stringification mode )
	{
		string_maker maker;
		
		const size_t size = maker.composite_length( value, mode );
		
		plus::string result;
		
		char* p = result.reset( size );
		
		maker.make_string( p, value, mode );
		
		return result;
	}
	
	plus::string repeat( const plus::string& s, plus::string::size_type n )
	{
		if ( n > 0x7fffffff )
		{
			THROW( "excessively large string multiplier" );
		}
		
		const char*  data = s.data();
		const size_t size = s.size();
		
		if ( size > 0x7fffffff )
		{
			THROW( "excessively large string multiplicand" );
		}
		
		const uint64_t product = (uint64_t) n * size;
		
		if ( product > 0x7fffffff )
		{
			THROW( "string multiplication overflow" );
		}
		
		const size_t n_bytes = product;
		
		plus::string result;
		
		char* p = result.reset( n_bytes );
		
		if ( size == 1 )
		{
			memset( p, *data, n );
		}
		else
		{
			while ( n-- > 0 )
			{
				p = (char*) mempcpy( p, data, size );
			}
		}
		
		return result;
	}
	
	plus::string join( const plus::string& glue, const Value& v, unsigned n )
	{
		if ( n <= 1  ||  glue.empty() )
		{
			return make_string( v, Stringified_to_print );
		}
		
		bignum::integer size = glue.size();
		
		size *= n - 1;
		
		string_maker maker;
		
		size += maker.composite_length( v, Stringified_to_print );
		
		if ( size > size_t( -1 ) )
		{
			THROW( "excessively large arguments to join()" );
		}
		
		list_iterator it( v );
		
		plus::string result;
		
		char* p = result.reset( size.clipped() );
		
		goto loop_start;
		
		do
		{
			p = mempcpy( p, glue );
			
		loop_start:
			
			p = maker.make_string( p, it.use(), Stringified_to_print );
		}
		while ( it );
		
		return result;
	}
	
	Value lines( const plus::string& s )
	{
		const char* p = s.data();
		size_t      n = s.size();
		
		if ( n == 0 )
		{
			return Value_empty_array;
		}
		
		const char* end = p + n;
		
		if ( end[ -1 ] != '\n' )
		{
			THROW( "final newline missing" );
		}
		
		list_builder result;
		
		const char* q = p;  // q trails p
		
		while ( p < end )
		{
			while ( *p++ != '\n' )  continue;
			
			result.append( String( plus::string( q, p - 1 ) ) );
			
			q = p;
		}
		
		return make_array( result );
	}
	
	plus::string format( const plus::string& form, const Value& params )
	{
		const char* p = form.c_str();
		
		list_iterator next_param( params );
		
		const Value* remaining = &params;
		
		plus::var_string result;
		
		while ( true )
		{
			const char* q = p;
			
			while ( *q != '\0'  &&  *q != '%' )
			{
				++q;
			}
			
			result.append( p, q );
			
			if ( *q++ == '\0' )
			{
				break;
			}
			
			p = q;
			
			const char c = *p++;
			
			if ( c == '%' )
			{
				result += c;
				continue;
			}
			
			if ( c != 's' )
			{
				THROW( "bad format string" );
			}
			
			if ( ! next_param )
			{
				THROW( "not enough format arguments" );
			}
			
			const Value& param = next_param.use();
			
			result += str( param );
		}
		
		if ( next_param )
		{
			THROW( "too many format arguments" );
		}
		
		return result.move();
	}
	
	Value reversed_bytes( const Value& u )
	{
		Value v = u;
		v.unshare();
		
		const plus::string& s = v.string();
		
		plus::reverse( (char*) s.data(), s.size() );
		
		return v;
	}
	
}
