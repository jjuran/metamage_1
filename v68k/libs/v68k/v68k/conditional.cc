/*
	conditional.cc
	--------------
*/

#include "v68k/conditional.hh"


namespace v68k
{
	
	enum
	{
		T,
		F,
		HI,
		LS,
		
		CC,
		CS,
		NE,
		EQ,
		
		VC,
		VS,
		PL,
		MI,
		
		GE,
		LT,
		GT,
		LE
	};
	
	bool test_conditional( uint16_t condition, uint16_t nzvc )
	{
		switch ( condition )
		{
			default:  // Not reached
			
			case T:  return true;
			case F:  return false;
			
			case HI:  return !(nzvc & 0x5);  // !C && !Z
			case LS:  return   nzvc & 0x5;   //  C ||  Z
			
			case CC:  return !(nzvc & 0x1);  // !C
			case CS:  return   nzvc & 0x1;   //  C
			
			case NE:  return !(nzvc & 0x4);  // !Z
			case EQ:  return   nzvc & 0x4;   //  Z
			
			case VC:  return !(nzvc & 0x2);  // !V
			case VS:  return   nzvc & 0x2;   //  V
			
			case PL:  return !(nzvc & 0x8);  // !N
			case MI:  return   nzvc & 0x8;   //  N
			
			case GE:  return ~(nzvc >> 2 ^ nzvc) & 0x2;  //  N == V
			case LT:  return  (nzvc >> 2 ^ nzvc) & 0x2;  //  N ^  V
			
			case GT:  return ~(nzvc >> 2 ^ nzvc) & 0x2  & ~nzvc >>  1;   //  N == V && !Z
			case LE:  return ((nzvc >> 2 ^ nzvc) & 0x2) | (nzvc & 0x4);  //  N ^  V ||  Z
		}
	}
	
}
