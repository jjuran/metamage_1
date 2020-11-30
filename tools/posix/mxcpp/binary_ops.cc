/*
	binary_ops.cc
	-------------
*/

#include "binary_ops.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <stdint.h>
#include <string.h>

// mxcpp
#include "exception.hh"


namespace tool
{
	
	static value_t times( const value_t& a, const value_t& b )
	{
		return a * b;
	}
	
	static value_t divided_by( const value_t& a, const value_t& b )
	{
		return a / b;
	}
	
	static value_t modulo( const value_t& a, const value_t& b )
	{
		return a % b;
	}
	
	
	static value_t plus( const value_t& a, const value_t& b )
	{
		return a + b;
	}
	
	static value_t minus( const value_t& a, const value_t& b )
	{
		return a - b;
	}
	
	
	static value_t left_shift( const value_t& a, const value_t& b )
	{
		return a << b;
	}
	
	static value_t right_shift( const value_t& a, const value_t& b )
	{
		return a >> b;
	}
	
	
	static value_t bitwise_and( const value_t& a, const value_t& b )
	{
		return a & b;
	}
	
	static value_t bitwise_xor( const value_t& a, const value_t& b )
	{
		return a ^ b;
	}
	
	static value_t bitwise_or( const value_t& a, const value_t& b )
	{
		return a | b;
	}
	
	
	static value_t less( const value_t& a, const value_t& b )
	{
		return a < b;
	}
	
	static value_t greater( const value_t& a, const value_t& b )
	{
		return a > b;
	}
	
	static value_t less_or_equal( const value_t& a, const value_t& b )
	{
		return a <= b;
	}
	
	static value_t greater_or_equal( const value_t& a, const value_t& b )
	{
		return a >= b;
	}
	
	
	static value_t equal( const value_t& a, const value_t& b )
	{
		return a == b;
	}
	
	static value_t not_equal( const value_t& a, const value_t& b )
	{
		return a != b;
	}
	
#ifdef __MC68K__
	
	/*
		Rewriting these functions in assembler is absolutely necessary.
		The output generated MWC68K for the C versions is worse than useless --
		in each case, the effect is to clobber D5 and return true (without
		regard to whether optimizations are active).
		
		logical_and:
		
			LINK     A6,#0
			MOVEA.L  (12,A6),A0
			MOVEQ    #88,D5
			BEQ.S    *+10
			MOVEA.L  (16,A6),A0
			MOVEQ    #88,D5
			BNE.S    *+6
			MOVEQ    #0,D0
			BRA.S    *+4
			MOVEQ    #1,D0
			EXT.W    D0
			EXT.L    D0
			MOVE.L   D0,-(A7)
			MOVE.L   (8,A6),-(A7)
			JSR      __rt_ultoi64
			UNLK     A6
			RTS
		
		logical_or:
		
			LINK     A6,#0
			MOVEA.L  (12,A6),A0
			MOVEQ    #88,D5
			BNE.S    *+14
			MOVEA.L  (16,A6),A0
			MOVEQ    #88,D5
			BNE.S    *+6
			MOVEQ    #0,D0
			BRA.S    *+4
			MOVEQ    #1,D0
			EXT.W    D0
			EXT.L    D0
			MOVE.L   D0,-(A7)
			MOVE.L   (8,A6),-(A7)
			JSR      __rt_ultoi64
			UNLK     A6
			RTS
		
		Avoiding the sign extension of a value which is clearly either 0 or 1
		and the subsequent three-instruction call of a function that does the
		work of two instructions is merely an added bonus.
	*/
	
	static asm value_t logical_and( const value_t& a, const value_t& b )
	{
		LINK     A6,#0
		
		MOVEA.L  8(A6),A0
		CLR.L    (A0)+
		MOVEQ.L  #0,D0
		
		MOVEA.L  12(A6),A1
		TST.L    (A1)+
		BNE.S    test_b
		TST.L    (A1)
		BEQ.S    done
		
	test_b:
		MOVEA.L  16(A6),A1
		TST.L    (A1)+
		BNE.S    is_true
		TST.L    (A1)
		BEQ.S    done
		
	is_true:
		MOVEQ.L  #1,D0
		
	done:
		MOVE.L   D0,(A0)
		
		UNLK     A6
		RTS
	}
	
	static asm value_t logical_or( const value_t& a, const value_t& b )
	{
		LINK     A6,#0
		
		MOVEA.L  8(A6),A0
		CLR.L    (A0)+
		MOVEQ.L  #1,D0
		
		MOVEA.L  12(A6),A1
		TST.L    (A1)+
		BNE.S    done
		TST.L    (A1)
		BNE.S    done
		
		MOVEA.L  16(A6),A1
		TST.L    (A1)+
		BNE.S    done
		TST.L    (A1)
		BNE.S    done
		
		MOVEQ.L  #0,D0
		
	done:
		MOVE.L   D0,(A0)
		
		UNLK     A6
		RTS
	}
	
#else
	
	static value_t logical_and( const value_t& a, const value_t& b )
	{
		return a && b;
	}
	
	static value_t logical_or( const value_t& a, const value_t& b )
	{
		return a || b;
	}
	
#endif
	
	
	static value_t branch( const value_t& a, const value_t& b )
	{
		return a << 32  |  (b & 0xffffffff);
	}
	
	static value_t select( const value_t& a, const value_t& b )
	{
		return a ? b >> 32 : int32_t( b );
	}
	
	
	static value_t but( const value_t& a, const value_t& b )
	{
	//	return a, b;  // "warning: left-hand operand of comma has no effect"
		
		return b;
	}
	
	
	static const binary_operation binary_operators[] =
	{
		//  -1  unary
		//  -2  multiplicative
		//  -3  additive
		//  -4  bit shift
		//  -5  inequality
		//  -6  equality
		//  -7  bitwise AND
		//  -8  bitwise XOR
		//  -9  bitwise OR
		// -10  logical AND
		// -11  logical OR
		// -12  ternary: branch
		// -13  ternary: select
		// -14  but
		// -15  pop
		
		{ "!=",  -6,      &not_equal       },
		{ "%",   -2,  &modulo              },
		{ "&",   -7,       &bitwise_and    },
		{ "&&", -10,          &logical_and },
		{ ")",  -15,               NULL    },
		{ "*",   -2,  &times               },
		{ "+",   -3,   &plus               },
		{ ",",  -14,              &but     },
		{ "-",   -3,   &minus              },
		{ "/",   -2,  &divided_by          },
		{ ":",  -12,            &branch    },
		{ "<",   -5,     &less             },
		{ "<<",  -4,    &left_shift        },
		{ "<=",  -5,     &less_or_equal    },
		{ "==",  -6,      &equal           },
		{ ">",   -5,     &greater          },
		{ ">=",  -5,     &greater_or_equal },
		{ ">>",  -4,    &right_shift       },
		{ "?",  -13,             &select   },
		{ "^",   -8,        &bitwise_xor   },
		{ "|",   -9,         &bitwise_or   },
		{ "||", -11,           &logical_or },
	};
	
	static bool operator<( const binary_operation& op, char c )
	{
		return op.name[0] < c;
	}
	
	static bool operator<( char c, const binary_operation& op )
	{
		return c < op.name[0];
	}
	
	static bool operator==( const binary_operation& op, const char* s )
	{
		return strncmp( op.name, s, 2 ) == 0;
	}
	
	static const binary_operation* find_binary_operator( const char* p )
	{
		const binary_operation* begin = binary_operators;
		const binary_operation* end   = begin + sizeof binary_operators / sizeof binary_operators[0];
		
		const binary_operation* it = std::lower_bound( begin, end, p[0] );
		
		const binary_operation* result = NULL;
		
		if ( it != end  &&  it->name[0] == p[0] )
		{
			if ( const bool has_easy_match = it->name[1] == '\0' )
			{
				result = it;
			}
			
			const binary_operation* bound = std::upper_bound( it, end, p[0] );
			
			it = std::find( begin, bound, p );
			
			if ( it != bound )
			{
				result = it;
			}
		}
		
		return result;
	}
	
	const binary_operation* next_binary_operator( const char* p )
	{
		if ( *p == '\0' )
		{
			return NULL;
		}
		
		if ( const binary_operation* op = find_binary_operator( p ) )
		{
			return op;
		}
		
		throw exception( "bad_binary_operator" );
	}
	
}
