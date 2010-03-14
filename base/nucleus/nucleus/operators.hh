// nucleus/operators.h
// -------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


/*
   This file creates a namespace, nucleus::operators, into which every 
   previously-defined nucleus operator is redeclared.  The intent is that
   this file will be included at the end of every nucleus header which
   defines nonmember operator functions, and that, therefore, the declaration
   "using namespace nucleus::operators" will bring all nucleus operators
   into scope.
*/

#ifndef NUCLEUS_OPERATORS_HH
#define NUCLEUS_OPERATORS_HH

#if PRAGMA_ONCE
#pragma once off
#endif


namespace nucleus
{
	// Declare some operators, so there's something for the using-declarations to see:
	// All of them are intentionally left undefined
	
	class nucleus_operator_dummy_type;
	
	void operator+( nucleus_operator_dummy_type );
	void operator-( nucleus_operator_dummy_type );
	void operator*( nucleus_operator_dummy_type );
	void operator/( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator%( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator^( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator&( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator|( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator~( nucleus_operator_dummy_type );
	
	void operator!( nucleus_operator_dummy_type );
	void operator<( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator>( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator+=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator-=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator*=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator/=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator%=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	
	void operator^=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator&=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator|=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator<<( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator>>( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator>>=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator<<=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator==( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator!=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	
	
	void operator<=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator>=( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator&&( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator||( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator++( nucleus_operator_dummy_type );
	void operator--( nucleus_operator_dummy_type );
	void operator,( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	void operator->*( nucleus_operator_dummy_type, nucleus_operator_dummy_type );
	
}

#endif

/*
	These declarations are intentionally left outside the include-guard.
	Since they can only refer to textually preceding operators, it can be
	useful to repeat them in order to collect more operators.
*/

namespace nucleus
{
	
	namespace operators
	{
		
		using nucleus::operator+;
		using nucleus::operator-;
		using nucleus::operator*;
		using nucleus::operator/;
		using nucleus::operator%;
		using nucleus::operator^;
		using nucleus::operator&;
		using nucleus::operator|;
		using nucleus::operator~;
		
		using nucleus::operator!;
		using nucleus::operator<;
		using nucleus::operator>;
		using nucleus::operator+=;
		using nucleus::operator-=;
		using nucleus::operator*=;
		using nucleus::operator/=;
		using nucleus::operator%=;
		
		using nucleus::operator^=;
		using nucleus::operator&=;
		using nucleus::operator|=;
		using nucleus::operator<<;
		using nucleus::operator>>;
		using nucleus::operator>>=;
		using nucleus::operator<<=;
		using nucleus::operator==;
		using nucleus::operator!=;
		
		using nucleus::operator<=;
		using nucleus::operator>=;
		using nucleus::operator&&;
		using nucleus::operator||;
		using nucleus::operator++;
		using nucleus::operator--;
		using nucleus::operator,;
		using nucleus::operator->*;
		
	}
	
}

