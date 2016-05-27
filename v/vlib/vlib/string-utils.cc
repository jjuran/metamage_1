/*
	string-utils.cc
	---------------
*/

#include "vlib/string-utils.hh"

// Standard C
#include <string.h>

// Standard C++
#include <vector>

// more-libc
#include "more/string.h"

// gear
#include "gear/hexadecimal.hh"

// debug
#include "debug/assert.hh"

// plus
#include "plus/decimal.hh"
#include "plus/var_string.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/proc_info.hh"
#include "vlib/quote.hh"
#include "vlib/symbol.hh"
#include "vlib/type_info.hh"


namespace vlib
{
	
	using ::mempcpy;
	
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
			
			case Op_var:    return "var ";
			case Op_const:  return "const ";
			
			case Op_function:     return "";
			case Op_named_unary:  return " ";
			
			case Op_member:  return ".";
			
			case Op_empower:  return "^";
			
			case Op_preinc:   return "++";
			case Op_predec:   return "--";
			case Op_postinc:  return "++";
			case Op_postdec:  return "--";
			
			case Op_unary_plus:   return "+";
			case Op_unary_minus:  return "-";
			case Op_unary_deref:  return "*";
			case Op_unary_count:  return "(+) ";
			
			case Op_multiply:  return " * ";
			case Op_divide:    return " / ";
			case Op_percent:   return " % ";
			case Op_modulo:    return " mod ";
			
			case Op_add:       return " + ";
			case Op_subtract:  return " - ";
			
			case Op_intersection:  return " & ";
			case Op_union:         return " | ";
			
			case Op_repeat:  return " (*) ";
			
			case Op_try:    return "try ";
			case Op_catch:  return " catch ";
			
			case Op_map:  return " map ";
			
			case Op_lt:  return " < ";
			case Op_gt:  return " > ";
			
			case Op_lte:  return " <= ";
			case Op_gte:  return " >= ";
			
			case Op_isa:       return " isa ";
			case Op_equal:     return " == ";
			case Op_unequal:   return " != ";
			
			case Op_mapping:  return " => ";
			
			case Op_if:    return "if ";
			case Op_then:  return " then ";
			case Op_else:  return " else ";
			
			case Op_while:    return "while ";
			case Op_do:       return "do ";
			case Op_do_2:     return " do ";
			case Op_while_2:  return " while ";
			
			case Op_break:     return "break";
			case Op_continue:  return "continue";
			
			case Op_duplicate:    return " = ";
			case Op_approximate:  return " := ";
			case Op_increase_by:  return " += ";
			case Op_decrease_by:  return " -= ";
			case Op_multiply_by:  return " *= ";
			case Op_divide_by:    return " /= ";
			case Op_percent_by:   return " %= ";
			
			case Op_assert:  return "assert ";
			case Op_throw:   return "throw ";
			
			case Op_end:  return "; ";
			
			default:  return " ?? ";
		}
	}
	
	static inline
	bool use_quotes( stringification mode )
	{
		return mode != Stringified_to_print;
	}
	
	static inline
	bool use_parens( stringification mode )
	{
		return mode != Stringified_to_print;
	}
	
	static inline
	bool use_commas( stringification mode )
	{
		return mode != Stringified_to_print;
	}
	
	
	static
	size_t composite_length( const Value&     value,
	                         stringification  mode,
	                         bool             print_parens = true )
	{
		switch ( value.type() )
		{
			case Value_nothing:
			case Value_dummy_operand:
				return 0;
			
			case Value_empty_list:  // "()", ""
				return 2 * use_parens( mode );
			
			case Value_byte:
				if ( use_quotes( mode ) )
				{
					return quote_byte( value.number().clipped() ).size();
				}
				
				return 1;
			
			case Value_data:
				return value.string().size() * 2 + use_quotes( mode ) * 3;
			
			case Value_string:
				if ( use_quotes( mode ) )
				{
					return quote_string( get_str( value ) ).size();
				}
				
				return get_str( value ).size();
			
			case Value_function:
				return strlen( get_proc( value ).name );
			
			case Value_base_type:
				return strlen( value.typeinfo().name );
			
			case Value_symbol:
				return value.sym()->name().size();
			
			case Value_boolean:
				return 4 + ! get_bool( value );  // "true" or "false"
			
			case Value_number:
				return decimal_length( get_int( value ) );
			
			case Value_pair:
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in composite_length()" );
				break;
		}
		
		Expr* expr = get_expr( value );
		
		ASSERT( expr != NULL );
		
		if ( (expr->op & 0xFF) == Op_block )
		{
			expr = expr->right.expr();
			
			ASSERT( expr != NULL );
		}
		
		if ( expr->op == Op_array  ||  (expr->op & 0xFF) == Op_scope )
		{
			const size_t n_brackets = use_parens( mode ) * 2;
			
			if ( is_empty( expr->right ) )
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
			
			if ( subparens  ||  ! is_empty( expr->right ) )
			{
				total += composite_length( expr->right, mode, subparens );
			}
			
			return total;
		}
		
		total += 2 * use_commas( mode );  // 2 for first comma
		
		while ( Expr* next = get_expr( expr->right ) )
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
	
	static
	char* make_string( char*            p,
	                   const Value&     value,
	                   stringification  mode,
	                   bool             print_parens = true )
	{
		switch ( value.type() )
		{
			case Value_nothing:
			case Value_dummy_operand:
				return p;
			
			case Value_empty_list:  // ""
				if ( use_parens( mode ) )
				{
					p = (char*) mempcpy( p, STR_LEN( "()" ) );
				}
				
				return p;
			
			case Value_byte:
				if ( use_quotes( mode ) )
				{
					return mempcpy( p, quote_byte( value.number().clipped() ) );
				}
				
				*p++ = value.number().clipped();
				
				return p;
			
			case Value_data:
				if ( use_quotes( mode ) )
				{
					*p++ = 'x';
					*p++ = '"';
				}
				
				{
					const plus::string& s = value.string();
					
					p = gear::hexpcpy_lower( p, s.data(), s.size() );
				}
				
				if ( use_quotes( mode ) )
				{
					*p++ = '"';
				}
				
				return p;
			
			case Value_string:
				if ( use_quotes( mode ) )
				{
					return mempcpy( p, quote_string( get_str( value ) ) );
				}
				
				return mempcpy( p, get_str( value ) );
			
			case Value_function:
				return mempcpy( p, get_proc( value ).name );
				
			case Value_base_type:
				return mempcpy( p, value.typeinfo().name );
			
			case Value_symbol:
				return mempcpy( p, value.sym()->name() );
			
			case Value_boolean:
				if ( ! get_bool( value ) )
				{
					return (char*) mempcpy( p, STR_LEN( "false" ) );
				}
				else
				{
					return (char*) mempcpy( p, STR_LEN( "true" ) );
				}
			
			case Value_number:
				return encode_decimal( p, get_int( value ) );
			
			case Value_pair:
				if ( ! use_parens( mode )  &&  is_function( value ) )
				{
					TYPE_ERROR( "stringification of compound function" );
				}
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in make_string()" );
				break;
		}
		
		Expr* expr = get_expr( value );
		
		if ( (expr->op & 0xFF) == Op_block )
		{
			expr = expr->right.expr();
		}
		
		if ( expr->op == Op_array )
		{
			if ( use_parens( mode ) )
			{
				*p++ = '[';
			}
			
			if ( ! is_empty( expr->right ) )
			{
				p = make_string( p, expr->right, mode, false );
			}
			
			if ( use_parens( mode ) )
			{
				*p++ = ']';
			}
			
			return p;
		}
		
		if ( (expr->op & 0xFF) == Op_scope )
		{
			*p++ = '{';
			
			if ( ! is_empty( expr->right ) )
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
				
				if ( subparens  ||  ! is_empty( expr->right ) )
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
		
		while ( Expr* next = get_expr( expr->right ) )
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
		const size_t size = composite_length( value, mode );
		
		plus::string result;
		
		char* p = result.reset( size );
		
		make_string( p, value, mode );
		
		return result;
	}
	
	plus::string repeat( const plus::string& s, plus::string::size_type n )
	{
		if ( n > 65535 )
		{
			DOMAIN_ERROR( "excessively large string multiplier" );
		}
		
		const char*  data = s.data();
		const size_t size = s.size();
		
		if ( size > 65535 )
		{
			DOMAIN_ERROR( "excessively large string multiplicand" );
		}
		
		const size_t n_bytes = n * size;
		
		plus::string result;
		
		char* p = result.reset( n_bytes );
		
		while ( n-- > 0 )
		{
			p = (char*) mempcpy( p, data, size );
		}
		
		return result;
	}
	
	plus::string join( const plus::string& glue, const Value& v, unsigned n )
	{
		if ( n <= 1  ||  glue.empty() )
		{
			return make_string( v, Stringified_to_print );
		}
		
		plus::integer size = glue.size();
		
		size *= n - 1;
		
		size += composite_length( v, Stringified_to_print );
		
		if ( size > size_t( -1 ) )
		{
			DOMAIN_ERROR( "excessively large arguments to join()" );
		}
		
		plus::string result;
		
		char* p = result.reset( size.clipped() );
		
		const Value* next = &v;
		
		while ( Expr* expr = get_expr( *next ) )
		{
			p = make_string( p, expr->left, Stringified_to_print );
			
			p = mempcpy( p, glue );
			
			next = &expr->right;
		}
		
		make_string( p, *next, Stringified_to_print );
		
		return result;
	}
	
	Value lines( const plus::string& s )
	{
		const char* p = s.data();
		size_t      n = s.size();
		
		if ( n == 0 )
		{
			return make_array( Value_empty_list );
		}
		
		const char* end = p + n;
		
		if ( end[ -1 ] != '\n' )
		{
			DOMAIN_ERROR( "final newline missing" );
		}
		
		std::vector< const char* > starts;
		
		while ( p < end )
		{
			starts.push_back( p );
			
			while ( *p++ != '\n' )  continue;
		}
		
		typedef std::vector< char const* >::const_iterator Iter;
		
		Iter begin = starts.begin();
		Iter it    = starts.end();
		
		const char* start = *--it;
		
		Value result = plus::string( start, p - 1 );
		
		while ( it > begin )
		{
			p = start;
			
			start = *--it;
			
			plus::string line( start, p - 1 );
			
			result = Value( Value( line ), result );
		}
		
		return make_array( result );
	}
	
	plus::string format( const plus::string& form, const Value& params )
	{
		const char* p = form.c_str();
		
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
				TYPE_ERROR( "bad format string" );
			}
			
			const Value& param = first( *remaining );
			
			if ( is_empty( param ) )
			{
				TYPE_ERROR( "not enough format arguments" );
			}
			
			result += make_string( param, Stringified_to_print );
			
			remaining = &rest( *remaining );
		}
		
		if ( ! is_empty( *remaining ) )
		{
			TYPE_ERROR( "too many format arguments" );
		}
		
		return result.move();
	}
	
}
