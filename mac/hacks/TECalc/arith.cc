/*

Transparent recursive descent

From comp.compilers
Newsgroups: 	comp.compilers
From: 	doug@netcom.com (Doug Merritt)
Summary: 	Expression parsing made easy
Keywords: 	parse, code, comment
Organization: 	Netcom Online Communications Services
Date: 	Mon, 1 Nov 1993 16:38:01 GMT

In past years I've been disgruntled that the classic approach to recursive
descent parsing of arithmetic expressions is inherently complex, long, and
unintuitive compared with the conceptual simplicity of expressions.

I recently found a way around that, by writing the descent in its most
intuitive form, and avoiding ambiguity and backtracking by use of a
trivial form of memoization and operator precedence.

The resulting C code is only two core functions, comprising a few dozen
lines of code including basic error checking (roughly one printed page) to
implement the core parser, along with a number of conceptually trivial
support functions.

The code is *completely* obvious and transparent to read, which I regard
as the novel and interesting feature that may make this worthwhile to
post...correctness can almost be ascertained by casual reading. (I refrain
from claiming that it is bug-free, however. ;-) It also may be somewhat
faster than the classic approach, because fewer function calls will be
required to implement the parse.

It's terse, it's fast, it's intuitive -- I'm happy.

One or two error checks have been elided from the code below for (a)
simplicity and (b) to help make it correct for both immediate evaluation
as well as syntax tree building. It was lightly tested prior to editing
for those purposes. It does not currently handle left-associative and
non-associative operators, but those can be trivially added.
Doug

*/

/*
* The following code was copyright 1993 Douglas R. Merritt.
* I hereby release it to the public domain.
*
* Transparent recursive descent expression parsing.
*
* Functions included below:
*	expression()	Calls expr() and checks for end-of-input.
*	expr()	Parses an entire expression including operators
*	item()	Parses a single operand (number or similar thing)
*
* "Trivial" support functions required, but not included below:
*	is_eoi(c)	Is 'c' an end-of-input token?
*	peek()	Returns the next pending input character/token
*	consume()	Returns value of current token, sets up for peek()
*	to read a new token. (Usually named "advance",
*	but this returns a value)
*	handle_eoi()	Gets rid of all characters up to end of input
*	class_ident(c)	Returns true if 'c' is identifier or constant.
*	class_unary(c)	Returns true if 'c' is a unary operator
*	(e.g. -, !, ~)
*	class_binary(c)	Returns true if 'c' is a binary operator
*	(e.g. +, -, *, /, etc)
*	greater_precedence(a, b)
*	Returns true if the precedence of operator b is
*	larger than that of a. (E.g. * is greater than +)
*	unary_op(op, expr)
*	Performs unary operation 'op' on expression 'expr'
*	(or builds a unary syntax tree node)
*	binary_op(expr1, op, expr2)
*	Performs binary operation 'op' on expressions
*	'expr1' and 'expr2' (e.g. 5 * 3)
*	(or builds a binary syntax tree node)
*	error()	Prints an error message.
*
* Types:
*	TOKEN	Either a #define for SYMBOL etc, or a single character
*	for simple things like +, *, <, >, etc.
*	VALUE	If the grammar is used to evaluate constant
*	expressions, this is an "int" or a "double". Otherwise
*	this is a pointer to a node in a tree that represents
*	the operator and the one or two operands.
*/

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// <Sound.h>
extern "C" pascal void SysBeep( short ticks )  ONEWORDINLINE( 0xA9C8 );

// iota
#include "iota/char_types.hh"


#ifndef NULL
#define NULL  0L
#endif

#define TOKEN char
#define VALUE long

extern VALUE expression(const char* expr);


#define is_eoi(c) ((c) == '\0')
#define peek() (*gExpression)
#define consume() (*gExpression++)
#define handle_eoi(p) 0
#define class_ident(c) (iota::is_digit(c))
#define class_unary(c) ((c) == '-')

static short class_binary(char c)
{
	switch (c) {
	case '+':
	case '-':
	case '*':
	case '/':
		return 1;
		break;
	default:
		return 0;
		break;
	}
}

static short greater_precedence(char a, char b)
{
	return ((a == '+') || (a == '-'))  &&  ((b == '*') || (b == '/'));
}

static long unary_op(char op, long value)
{
	return -value;
}

static long binary_op(long val1, char op, long val2)
{
	switch (op) {
	case '+':
		return val1 + val2;
		break;
	case '-':
		return val1 - val2;
		break;
	case '*':
		return val1 * val2;
		break;
	case '/':
		return val1 / val2;
		break;
	default:
		return 0;
	}
}

#define error(str) SysBeep( 30 )

#define char2val(c) ((c) - '0')

static const char* gExpression;

static VALUE Expr(VALUE e1, TOKEN op1);

extern
VALUE
expression(const char* expr)	/* This just calls expr() and handles any following garbage */
{
	TOKEN c;
	VALUE e;
	
	gExpression = expr;
	
	e = Expr(0, 0);
	if (!is_eoi(c=peek()))
	error("garbage after expression");
	consume();
	handle_eoi(c);	/* discard everything up to end of input */
	return e;
}

/*
* Parse a single operand: "(expr)", "symbol", "123", "-expr".
* Co-recursive with expr().
*/
static
VALUE
item()
{
	TOKEN t;
	VALUE e;

	if ((t=peek()) == '(') {
		consume();
		e = Expr(0, 0);	/* nothing memoized */
		if (peek() == ')')
			consume();
		else
			error("Missing ending ')'");
		return e;
	} else if (class_unary(t)) {
		TOKEN oper = t;
		consume();
		e = Expr(0, 0);	/* nothing memoized */
		return unary_op(oper, e);
	} else if (class_ident(t)) {
		return char2val(consume());
	}
	error("Expected operand");
	return NULL;
}

VALUE
Expr(VALUE e1, TOKEN op1)
//VALUE e1;	/* The two parameters are memoization to avoid */
//TOKEN op1;	/* backtracking */
{
	VALUE e2;	/* read-ahead of a second operator and second */
	TOKEN op2;	/* operand */

	if (!e1) {
		e1 = item();
		if (is_eoi(peek()))	/* nothing else? */
			return e1;
	}
	if (!op1) {
		if (!class_binary(op1 = peek())) {
			error("Expected operator");
			return e1;
		}
		consume();
	}
	e2 = item();	/* get second operand */
	if (class_binary(op2=peek())) {
		consume();
		/*
		* left- and non-associative operators could be trivally
		* added here
		*/
		if (greater_precedence(op1, op2))
			return binary_op(e1, op1, Expr(e2, op2));
		else
			return Expr(binary_op(e1, op1, e2), op2);
	}
	return binary_op(e1, op1, e2);
} /* Expr */

/*
--
Doug Merritt	doug@netcom.com
[I can't say that this looks all that different from any other R.D.
expression parser I've seen, but it's nice to have the code for the
archives. -John]
-- 
------------------------------------------------------------------------

 Return to the comp.compilers page. 
 Search the comp.compilers archives again. 

*/
