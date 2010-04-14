/*	===========
 *	demangle.cc
 *	===========
 */

#include "recall/demangle.hh"

// Standard C/C++
#include <cctype>
#include <cstring>

// iota
#include "iota/decimal.hh"
#include "iota/strings.hh"


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
	
	static const Operator gOperators[] =
	{
		{ "nw"  "_", OP( " new"      ) },
		{ "nwa" "_", OP( " new[]"    ) },
		{ "dl"  "_", OP( " delete"   ) },
		{ "dla" "_", OP( " delete[]" ) },
		
		{ "pl"  "_", OP( "+" ) },  // PLus
		{ "mi"  "_", OP( "-" ) },  // MInus
		{ "ml"  "_", OP( "*" ) },  // MuLtiply
		{ "dv"  "_", OP( "/" ) },  // DiVide
		{ "md"  "_", OP( "%" ) },  // MoD
		{ "er"  "_", OP( "^" ) },  // Exclusive oR
		
		{ "ad"  "_", OP( "&" ) },  // AnD
		{ "or"  "_", OP( "|" ) },  // OR
		{ "co"  "_", OP( "~" ) },  // COmplement
		{ "nt"  "_", OP( "!" ) },  // NoT
		{ "as"  "_", OP( "=" ) },  // ASsign
		
		{ "apl"  "_", OP( "+=" ) },  // Assign-PLus
		{ "ami"  "_", OP( "-=" ) },  // Assign-MInus
		{ "amu"  "_", OP( "*=" ) },  // Assign-MUltiply
		{ "adv"  "_", OP( "/=" ) },  // Assign-DiVide
		{ "amd"  "_", OP( "%=" ) },  // Assign-MoD
		{ "aer"  "_", OP( "^=" ) },  // Assign-Exclusive-oR
		{ "aad"  "_", OP( "&=" ) },  // Assign-AnD
		{ "aor"  "_", OP( "|=" ) },  // Assign-OR
		
		{ "ls"  "_", OP( "<<" ) },  // Left Shift
		{ "rs"  "_", OP( ">>" ) },  // Right Shift
		{ "als" "_", OP( "<<=" ) },  // Assign-Left-Shift
		{ "ars" "_", OP( ">>=" ) },  // Assign-Right-Shift
		
		{ "eq"  "_", OP( "==" ) },  // EQual
		{ "ne"  "_", OP( "!=" ) },  // Not Equal
		{ "le"  "_", OP( "<=" ) },  // Less than or Equal
		{ "ge"  "_", OP( ">=" ) },  // Greater than or Equal
		{ "lt"  "_", OP( "<" ) },  // Less Than
		{ "gt"  "_", OP( ">" ) },  // Greater Than
		
		{ "aa"  "_", OP( "&&" ) },
		{ "oo"  "_", OP( "||" ) },
		{ "pp"  "_", OP( "++" ) },
		{ "mm"  "_", OP( "--" ) },
		{ "cl"  "_", OP( "()" ) },  // CaLl
		{ "vc"  "_", OP( "[]" ) },  // VeCtor
		{ "rf"  "_", OP( "->" ) },  // deReFerence
		{ "cm"  "_", OP( "," ) },  // CoMma
		{ "rm"  "_", OP( "->*" ) },  // deReference Member
		
		{ NULL, NULL }
	};
	
	static bool begins_with( const char* s, const char* sub )
	{
		while ( *s != '\0' )
		{
			if ( *s++ != *sub++ )
			{
				return false;
			}
		}
		
		return true;
	}
	
	static inline bool operator==( const Operator& op, const char* code )
	{
		return begins_with( op.code, code );
	}
	
	static const Operator* find_operator( const char* code )
	{
		const Operator* begin = gOperators;
		const Operator* end   = begin + sizeof gOperators / sizeof gOperators[0];
		
		const Operator* it = std::find( begin, end, code );
		
		return it != end ? it : NULL;
	}
	
	static const char* ReadOperator( const char*& p, const char* end )
	{
		const Operator* it = find_operator( p );
		
		if ( it == NULL )
		{
			throw demangle_failed();
		}
		
		p = end;
		
		return it->name;
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
			throw demangle_failed();
		}
		
		const char* type = gBasicTypes[ *p++ - 'a' ];
		
		if ( type == NULL )
		{
			throw demangle_failed();
		}
		
		return sign + type;
	}
	
	static inline unsigned ReadLength( const char*& p )
	{
		return iota::parse_unsigned_decimal( &p );
	}
	
	static std::string ReadLName( const char*& p )
	{
		unsigned length = ReadLength( p );
		
		std::string result( p, length );
		
		p += length;
		
		return result;
	}
	
	static std::string ReadInteger( const char* begin, const char* end )
	{
		if ( *begin != '-' )
		{
			unsigned x = iota::parse_unsigned_decimal( begin );  // *end is either ',' or '>'
			
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
				throw demangle_failed();
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
			throw demangle_failed();
		}
		
		if ( *p == 'F' )
		{
			return ReadFunctionType( ++p );
		}
		
		throw demangle_failed();
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
			throw NotSpecial();
		}
		
		std::string name( p, double_underscore );
		
		if ( name == "__ct" )  return p = double_underscore, "";
		if ( name == "__dt" )  return p = double_underscore, "~";
		
		if ( name == "_vtbl"     )  throw demangle_failed();
		if ( name == "_rttivtbl" )  throw demangle_failed();
		if ( name == "_vbtbl"    )  throw demangle_failed();
		if ( name == "__rtti"    )  throw demangle_failed();
		if ( name == "__ti"      )  throw demangle_failed();
		if ( name == "___ti"     )  throw demangle_failed();
		
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
		
		const char* double_underscore = std::strstr( p + 1, "__" );
		
		const char* id_end = double_underscore ? double_underscore
		                                       : p + std::strlen( p );
		
		if ( params != NULL  &&  params < id_end )
		{
			std::string identifier( p, params );
			
			p = params;
			
			return identifier + ReadTemplateParameters( p );
		}
		
		std::string result( p, id_end );
		
		p = id_end;
		
		return result;
	}
	
	std::string Unmangler::ReadEntityName( const char*& p )
	{
		if ( std::strcmp( p, "__end__catch" ) == 0 )
		{
			p += STRLEN( "__end__catch" );
			
			return "__end__catch";
		}
		
		if ( std::strcmp( p, "__throw_bad_alloc__3stdFv" ) == 0 )
		{
			p += STRLEN( "__throw_bad_alloc__3stdFv" );
			
			return "std::__throw_bad_alloc( void )";
		}
		
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
	
	
	std::string demangle_MWC68K( const std::string& name )
	{
		const char* p = name.c_str();
		
		return MWC68K_Unmangler().ReadSymbol( p );
	}
	
	std::string demangle_MWCPPC( const std::string& name )
	{
		if ( name[0] != '.' )
		{
			return name;
		}
		
		const char* p = name.c_str();
		
		return MWCPPC_Unmangler().ReadSymbol( p );
	}
	
}

