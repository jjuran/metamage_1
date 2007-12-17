/*	===========
 *	Unmangle.cc
 *	===========
 */

#include "Backtrace/Unmangle.hh"

// Standard C/C++
#include <cctype>
#include <cstdlib>

// Standard C++
#include <map>


static std::string join( const char* space, const std::string& a, const std::string& b )
{
	if ( a.empty() || b.empty() )
	{
		space = "";
	}
	
	return a + space + b;
}

namespace Backtrace
{
	
	static const char* gBasicTypes[] =
	{
		NULL,
		"bool",
		"char",
		"double",
		"...",  // ellipsis
		"float",
		NULL,
		NULL,
		"int",
		NULL,
		NULL,
		"long",
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		"long double",  // long, or 'real' long?  (Get it?)
		"short",
		NULL,
		NULL,
		"void",
		"wchar_t",
		"long long",  // 'xtra long
		NULL,
		NULL
	};
	
	struct Operator
	{
		const char* code;
		const char* name;
	};
	
	#define OP( op )  "operator" op
	
	static Operator gOperators[] =
	{
		{ "nw",  OP( " new"      ) },
		{ "nwa", OP( " new[]"    ) },
		{ "dl",  OP( " delete"   ) },
		{ "dla", OP( " delete[]" ) },
		
		{ "pl",  OP( "+" ) },  // PLus
		{ "mi",  OP( "-" ) },  // MInus
		{ "ml",  OP( "*" ) },  // MuLtiply
		{ "dv",  OP( "/" ) },  // DiVide
		{ "md",  OP( "%" ) },  // MoD
		{ "er",  OP( "^" ) },  // Exclusive oR
		
		{ "ad",  OP( "&" ) },  // AnD
		{ "or",  OP( "|" ) },  // OR
		{ "co",  OP( "~" ) },  // COmplement
		{ "nt",  OP( "!" ) },  // NoT
		{ "as",  OP( "=" ) },  // ASsign
		
		{ "apl",  OP( "+=" ) },  // Assign-PLus
		{ "ami",  OP( "-=" ) },  // Assign-MInus
		{ "amu",  OP( "*=" ) },  // Assign-MUltiply
		{ "adv",  OP( "/=" ) },  // Assign-DiVide
		{ "amd",  OP( "%=" ) },  // Assign-MoD
		{ "aer",  OP( "^=" ) },  // Assign-Exclusive-oR
		{ "aad",  OP( "&=" ) },  // Assign-AnD
		{ "aor",  OP( "|=" ) },  // Assign-OR
		
		{ "ls",  OP( "<<" ) },  // Left Shift
		{ "rs",  OP( ">>" ) },  // Right Shift
		{ "als",  OP( "<<=" ) },  // Assign-Left-Shift
		{ "ars",  OP( ">>=" ) },  // Assign-Right-Shift
		
		{ "eq",  OP( "==" ) },  // EQual
		{ "ne",  OP( "!=" ) },  // Not Equal
		{ "le",  OP( "<=" ) },  // Less than or Equal
		{ "ge",  OP( ">=" ) },  // Greater than or Equal
		{ "lt",  OP( "<" ) },  // Less Than
		{ "gt",  OP( ">" ) },  // Greater Than
		
		{ "aa",  OP( "&&" ) },
		{ "oo",  OP( "||" ) },
		{ "pp",  OP( "++" ) },
		{ "mm",  OP( "--" ) },
		{ "cl",  OP( "()" ) },  // CaLl
		{ "vc",  OP( "[]" ) },  // VeCtor
		{ "rf",  OP( "->" ) },  // deReFerence
		{ "cm",  OP( "," ) },  // CoMma
		{ "rm",  OP( "->*" ) },  // deReference Member
		
		{ NULL, NULL }
	};
	
	typedef std::map< std::string, const char* > OperatorMap;
	
	static OperatorMap MakeOperatorMap()
	{
		OperatorMap result;
		
		const unsigned array_length = sizeof gOperators / sizeof (Operator) - 1;
		
		for ( const Operator* op = gOperators;  op < gOperators + array_length;  ++op )
		{
			result[ op->code ] = op->name;
		}
		
		return result;
	}
	
	static const char* GetBuiltinType( char c )
	{
		if ( c < 'a'  ||  c > 'z' )  return NULL;
		
		return gBasicTypes[ c - 'a' ];
	}
	
	static std::string ReadBasicType( const char*& p )
	{
		std::string sign;
		
		if ( *p == 'U' )
		{
			sign = "unsigned ";
			
			++p;
		}
		else if ( *p == 'S' )
		{
			sign = "signed ";
			
			++p;
		}
		
		if ( *p < 'a'  ||  *p > 'z' )
		{
			throw Unmangle_Failed();
		}
		
		const char* type = gBasicTypes[ *p++ - 'a' ];
		
		if ( type == NULL )
		{
			throw Unmangle_Failed();
		}
		
		return sign + type;
	}
	
	static int ReadLength( const char*& p )
	{
		int length = std::atoi( p );
		
		while ( std::isdigit( *++p ) )  continue;
		
		return length;
	}
	
	static std::string ReadLName( const char*& p )
	{
		int length = ReadLength( p );
		
		std::string result( p, length );
		
		p += length;
		
		return result;
	}
	
	static std::string ReadType( const char*& p, const char* end );
	
	static std::string ReadSymbol( const char*& p, const char* end );
	
	static std::string ReadTemplateParameter( const char*& p, const char* end )
	{
		if ( *p == '&' )
		{
			return ReadSymbol( ++p, end );
		}
		
		if ( *p == '-'  ||  std::isdigit( *p ) )
		{
			const char* integer = p;
			
			while ( std::isdigit( *++p ) )
			{
				continue;
			}
			
			if ( *p == ','  ||  *p == '>' )
			{
				return std::string( integer, p );
			}
			
			p = integer;  // backtrack
		}
		
		return ReadType( p, end );
	}
	
	static std::string ExpandTemplates( const std::string& name )
	{
		const char* params = std::strchr( name.c_str(), '<' );
		
		if ( params == NULL )
		{
			return name;
		}
		
		const char* p = params;
		
		std::string result;
		
		while ( true )
		{
			result = join( ", ", result, ReadTemplateParameter( ++p, &*name.end() ) );
			
			if ( *p == '>' )
			{
				break;
			}
			
			if ( *p != ',' )
			{
				throw Unmangle_Failed();
			}
		}
		
		result = std::string( name.data(), params ) + "< " + result + " >";
		
		return result;
	}
	
	static std::string ReadQualName( const char*& p )
	{
		return ExpandTemplates( ReadLName( p ) );
	}
	
	static std::string ReadQualifiedName( const char*& p )
	{
		int count = *p++ - '0';
		
		std::string result;
		
		while ( count-- )
		{
			result = join( "::", result, ReadQualName( p ) );
		}
		
		return result;
	}
	
	static std::string ReadFunctionType( const char*& p, const char* end )
	{
		std::string params;
		std::string return_type;
		
		while ( p < end )
		{
			if ( *p == '>' )
			{
				break;
			}
			
			if ( *p == '_' )
			{
				return_type = ReadType( ++p, end ) + " ";
				
				break;
			}
			
			params = join( ", ", params, ReadType( p, end ) );
		}
		
		return return_type + "( " + params + " )";
	}
	
	static std::string ReadIndirectType( const char*& p, const char* end )
	{
		std::string result;
		
		switch ( *p )
		{
			case 'R':
				result = "&" + result;
				++p;
				
				break;
			
			case 'P':
				result = "*" + result;
				++p;
				
				break;
			
			case 'M':
				result = "::(" + ReadType( ++p, end ) + ")*" + result;
				
				break;
			
			default:
				break;
		}
		
		return ReadType( p, end ) + result;
	}
	
	static std::string ReadQualifiedType( const char*& p, const char* end )
	{
		std::string result;
		
		if ( *p == 'C' )
		{
			result = " const";
			
			++p;
		}
		
		if ( *p == 'V' )
		{
			result = " volatile" + result;
			
			++p;
		}
		
		return ReadType( p, end ) + result;
	}
	
	static std::string ReadType( const char*& p, const char* end )
	{
		if ( *p >= 'a'  &&  *p <= 'z'  ||  *p == 'S'  ||  *p == 'U' )
		{
			return ReadBasicType( p );
		}
		
		if ( *p == 'C'  ||  *p == 'V' )
		{
			return ReadQualifiedType( p, end );
		}
		
		if ( *p == 'R'  ||  *p == 'P'  ||  *p == 'M' )
		{
			return ReadIndirectType( p, end );
		}
		
		if ( std::isdigit( *p ) )
		{
			return ReadQualName( p );
		}
		
		if ( *p == 'Q' )
		{
			return ReadQualifiedName( ++p );
		}
		
		if ( *p == 'A' )
		{
			throw Unmangle_Failed();
		}
		
		if ( *p == 'F' )
		{
			return ReadFunctionType( ++p, end );
		}
		
		throw Unmangle_Failed();
	}
	
	static std::string ReadConversion( const char*& p, const char* end )
	{
		return "operator " + ReadType( p, end );
	}
	
	static const char* ReadOperator( const char*& p, const char* end )
	{
		static OperatorMap map = MakeOperatorMap();
		
		OperatorMap::const_iterator it = map.find( std::string( p, end ) );
		
		if ( it == map.end() )
		{
			throw Unmangle_Failed();
		}
		
		p = end;
		
		return it->second;
	}
	
	static std::string ReadSpecialName( const char*& p )
	{
		const char* q = p;
		
		while ( *q == '_' )
		{
			++q;
		}
		
		const char* double_underscore = std::strstr( q, "__" );
		
		if ( double_underscore == NULL )
		{
			throw Unmangle_Failed();
		}
		
		std::string name( p, double_underscore );
		
		if ( name == "__ct" )  return p = double_underscore, "";
		if ( name == "__dt" )  return p = double_underscore, "~";
		
		if ( name == "_vtbl"     )  throw Unmangle_Failed();
		if ( name == "_rttivtbl" )  throw Unmangle_Failed();
		if ( name == "_vbtbl"    )  throw Unmangle_Failed();
		if ( name == "__rtti"    )  throw Unmangle_Failed();
		if ( name == "__ti"      )  throw Unmangle_Failed();
		if ( name == "___ti"     )  throw Unmangle_Failed();
		
		if ( p[1] == '_' )
		{
			p += 2;
			
			if ( p[0] == 'o'  &&  p[1] == 'p' )
			{
				p += 2;
				
				return ReadConversion( p, double_underscore );
			}
			
			return ReadOperator( p, double_underscore );
		}
		
		throw Unmangle_Failed();
	}
	
	static std::string ReadIdentifier( const char*& p, const char* end )
	{
		std::string result( p, end );
		
		p = end;
		
		return ExpandTemplates( result );
	}
	
	static std::string ReadEntityName( const char*& p, const char* end )
	{
		if ( *p == '_' )
		{
			return ReadSpecialName( p );
		}
		
		return ReadIdentifier( p, end );
	}
	
	static std::string LastName( const std::string& qualified_name )
	{
		std::size_t colon = qualified_name.find_last_of( ":" );
		
		return qualified_name.substr( colon + 1, qualified_name.npos );
	}
	
	static std::string ReadSymbol( const char*& p, const char* end )
	{
		if ( p[0] == '.' )
		{
			++p;
		}
		
		const char* double_underscore = std::strstr( p, "__" );
		
		if ( double_underscore == NULL )
		{
			return p;
		}
		
		std::string function_name = ReadEntityName( p, double_underscore );
		
		std::string class_name;
		
		p += 2;
		
		if ( std::isdigit( *p ) )
		{
			class_name = ReadQualName( p );
		}
		else if ( p[0] == 'Q' )
		{
			class_name = ReadQualifiedName( ++p );
		}
		
		if ( function_name.empty()  ||  function_name == "~" )
		{
			// Set the name for a constructor or destructor
			function_name += LastName( class_name );
		}
		
		if ( !class_name.empty() )
		{
			function_name = class_name + "::" + function_name;
		}
		
		return function_name + ReadType( p, end );
	}
	
	std::string UnmangleMWC68K( const std::string& name )
	{
		const char* p = name.data();
		
		return ReadSymbol( p, p + name.size() );
	}
	
	std::string UnmangleMWCPPC( const std::string& name )
	{
		if ( name[0] != '.' )
		{
			return name;
		}
		
		const char* p = name.data();
		
		return ReadSymbol( p, p + name.size() );
	}
	
	std::string UnmangleGCC( const std::string& name )
	{
		if ( name[0] != '_' )
		{
			return name;
		}
		
		return name;
	}
	
}

