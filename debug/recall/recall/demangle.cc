/*	===========
 *	demangle.cc
 *	===========
 */

#include "recall/demangle.hh"

// Standard C/C++
#include <cctype>
#include <cstdlib>
#include <cstring>

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

namespace recall
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
	
	static std::string LastName( const std::string& qualified_name )
	{
		std::size_t colon = qualified_name.find_last_of( ":" );
		
		return qualified_name.substr( colon + 1, qualified_name.npos );
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
	
	static std::string ReadInteger( const char* begin, const char* end )
	{
		if ( *begin != '-' )
		{
			unsigned x = std::atoi( begin );  // *end is either ',' or '>'
			
			char code[] = "'code'";
			
			code[4] =  x        & 0xff;
			code[3] = (x >>= 8) & 0xff;
			code[2] = (x >>= 8) & 0xff;
			code[1] = (x >>= 8) & 0xff;
			
			bool isCode =    std::isprint( code[1] )
			              && std::isprint( code[2] )
			              && std::isprint( code[3] )
			              && std::isprint( code[4] );
			
			if ( isCode )
			{
				return code;
			}
		}
		
		return std::string( begin, end );
	}
	
	
	class Unmangler
	{
		public:
			virtual const char* FindTemplateParameters( const char* p ) = 0;
			
			virtual bool TemplateParameterFollows( const char* p ) = 0;
			
			virtual bool TemplateParameterListEndsHere( const char* p ) = 0;
			
			
			std::string ReadTemplateParameter( const char*& p );
			
			std::string ReadTemplateParameters( const char*&p );
			
			std::string ExpandTemplates( const std::string& name );
			
			std::string ReadQualName( const char*& p );
			
			std::string ReadQualifiedName( const char*& p );
			
			std::string ReadFunctionType( const char*& p );
			
			std::string ReadIndirectType( const char*& p );
			
			std::string ReadQualifiedType( const char*& p );
			
			std::string ReadType( const char*& p );
			
			std::string ReadConversion( const char*& p )  { return "operator " + ReadType( p ); }
			
			std::string ReadSpecialName( const char*& p );
			
			std::string ReadIdentifier( const char*& p );
			
			std::string ReadEntityName( const char*& p );
			
			std::string ReadSymbol( const char*& p );
	};
	
	class MWC68K_Unmangler : public Unmangler
	{
		public:
			const char* FindTemplateParameters( const char* name );
			
			bool TemplateParameterFollows( const char* p );
			
			bool TemplateParameterListEndsHere( const char* p );
	};
	
	class MWCPPC_Unmangler : public Unmangler
	{
		public:
			const char* FindTemplateParameters( const char* p )  { return std::strchr( p, '<' ); }
			
			bool TemplateParameterFollows( const char* p )  { return *p == ','; }
			
			bool TemplateParameterListEndsHere( const char* p )  { return *p == '>'; }
	};
	
	
	bool MWC68K_Unmangler::TemplateParameterFollows( const char* p )
	{
		if ( p[0] != '_' )  return false;
		
		if ( std::isdigit( p[1] ) )  return true;
		
		if ( p[1] == 'Q'  &&  std::isdigit( p[2] ) )  return true;
		
		if ( GetBuiltinType( p[1] ) != NULL  &&  p[2] == '_' )  return true;
		
		return false;
	}
	
	bool MWC68K_Unmangler::TemplateParameterListEndsHere( const char* p )
	{
		return p[0] == '_'  &&  ( p[1] == '_'  ||  p[1] == '\0' );
	}
	
	const char* MWC68K_Unmangler::FindTemplateParameters( const char* name )
	{
		while ( const char* underscore = std::strchr( name, '_' ) )
		{
			if ( TemplateParameterFollows( underscore ) )
			{
				return underscore;
			}
			
			name = underscore + 1;
		}
		
		return NULL;
	}
	
	
	std::string Unmangler::ReadTemplateParameter( const char*& p )
	{
		if ( *p == '&' )
		{
			return ReadSymbol( ++p );
		}
		
		if ( *p == '-'  ||  std::isdigit( *p ) )
		{
			const char* integer = p;
			
			while ( std::isdigit( *++p ) )
			{
				continue;
			}
			
			if ( TemplateParameterListEndsHere( p ) || TemplateParameterFollows( p ) )
			{
				return ReadInteger( integer, p );
			}
			
			p = integer;  // backtrack
		}
		
		return ReadType( p );
	}
	
	std::string Unmangler::ReadTemplateParameters( const char*&p )
	{
		std::string result;
		
		while ( true )
		{
			result = join( ", ", result, ReadTemplateParameter( ++p ) );
			
			if ( TemplateParameterListEndsHere( p ) )
			{
				++p;
				break;
			}
			
			if ( !TemplateParameterFollows( p ) )
			{
				throw Unmangle_Failed();
			}
		}
		
		result = "< " + result + " >";
		
		return result;
	}
	
	std::string Unmangler::ExpandTemplates( const std::string& name )
	{
		const char* params = FindTemplateParameters( name.c_str() );
		
		if ( params == NULL )
		{
			return name;
		}
		
		const char* p = params;
		
		std::string result = ReadTemplateParameters( p );
		
		result = std::string( name.data(), params ) + result;
		
		return result;
	}
	
	std::string Unmangler::ReadQualName( const char*& p )
	{
		return ExpandTemplates( ReadLName( p ) );
	}
	
	std::string Unmangler::ReadQualifiedName( const char*& p )
	{
		int count = *p++ - '0';
		
		std::string result;
		
		while ( count-- )
		{
			result = join( "::", result, ReadQualName( p ) );
		}
		
		return result;
	}
	
	std::string Unmangler::ReadFunctionType( const char*& p )
	{
		std::string params;
		std::string return_type;
		
		while ( *p != '\0' )
		{
			if ( *p == '>' )
			{
				break;
			}
			
			if ( *p == '_' )
			{
				return_type = ReadType( ++p ) + " ";
				
				break;
			}
			
			params = join( ", ", params, ReadType( p ) );
		}
		
		return return_type + "( " + params + " )";
	}
	
	std::string Unmangler::ReadIndirectType( const char*& p )
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
				result = "::(" + ReadType( ++p ) + ")*" + result;
				
				break;
			
			default:
				break;
		}
		
		return ReadType( p ) + result;
	}
	
	std::string Unmangler::ReadQualifiedType( const char*& p )
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
		
		return ReadType( p ) + result;
	}
	
	std::string Unmangler::ReadType( const char*& p )
	{
		if ( *p >= 'a'  &&  *p <= 'z'  ||  *p == 'S'  ||  *p == 'U' )
		{
			return ReadBasicType( p );
		}
		
		if ( *p == 'C'  ||  *p == 'V' )
		{
			return ReadQualifiedType( p );
		}
		
		if ( *p == 'R'  ||  *p == 'P'  ||  *p == 'M' )
		{
			return ReadIndirectType( p );
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
			return ReadFunctionType( ++p );
		}
		
		throw Unmangle_Failed();
	}
	
	struct NotSpecial {};
	
	std::string Unmangler::ReadSpecialName( const char*& p )
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
				
				return ReadConversion( p );
			}
			
			return ReadOperator( p, double_underscore );
		}
		
		throw NotSpecial();
	}
	
	std::string Unmangler::ReadIdentifier( const char*& p )
	{
		const char* params = FindTemplateParameters( p );
		
		const char* double_underscore = std::strstr( p, "__" );
		
		if ( double_underscore == NULL )
		{
			double_underscore = p + std::strlen( p );
		}
		
		if ( params != NULL  &&  params < double_underscore )
		{
			std::string identifier( p, params );
			
			p = params;
			
			return identifier + ReadTemplateParameters( p );
		}
		
		std::string result( p, double_underscore );
		
		p = double_underscore;
		
		return result;
	}
	
	std::string Unmangler::ReadEntityName( const char*& p )
	{
		if ( *p == '_' )
		{
			try
			{
				return ReadSpecialName( p );
			}
			catch ( const NotSpecial& )
			{
				// This happens e.g. with _exit
			}
		}
		
		return ReadIdentifier( p );
	}
	
	std::string Unmangler::ReadSymbol( const char*& p )
	{
		if ( p[0] == '.' )
		{
			++p;
		}
		
		std::string function_name = ReadEntityName( p );
		
		if ( *p == '\0' )
		{
			return function_name;
		}
		
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
		
		return function_name + ReadType( p );
	}
	
	
	std::string UnmangleMWC68K( const std::string& name )
	{
		const char* p = name.c_str();
		
		return MWC68K_Unmangler().ReadSymbol( p );
	}
	
	std::string UnmangleMWCPPC( const std::string& name )
	{
		if ( name[0] != '.' )
		{
			return name;
		}
		
		const char* p = name.c_str();
		
		return MWCPPC_Unmangler().ReadSymbol( p );
	}
	
}

