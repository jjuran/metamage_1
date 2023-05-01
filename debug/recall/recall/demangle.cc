/*	===========
 *	demangle.cc
 *	===========
 */

#include "recall/demangle.hh"

// Standard C
#include <string.h>

// Standard C++
#include <algorithm>

// iota
#include "iota/char_types.hh"
#include "iota/strings.hh"

// gear
#include "gear/parse_decimal.hh"

// plus
#include "plus/var_string.hh"


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
	while ( *sub != '\0' )
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
	return begins_with( code, op.code );
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

static plus::string LastName( const plus::string& qualified_name )
{
	size_t colon = qualified_name.find_last_of( ":" );
	
	return qualified_name.substr( colon + 1 );
}

static inline
const char* basic_type_entry( unsigned char c )
{
	const unsigned char i = c - 'a';
	
	if ( i > 'z' - 'a' )  return NULL;
	
	return gBasicTypes[ i ];
}

static void ReadBasicType( plus::var_string& out, const char*& p )
{
	if ( *p == 'U' )
	{
		out += "unsigned ";
		
		++p;
	}
	else if ( *p == 'S' )
	{
		out += "signed ";
		
		++p;
	}
	
	const char* type = basic_type_entry( *p++ );
	
	if ( type == NULL )
	{
		throw demangle_failed();
	}
	
	out += type;
}

static inline unsigned ReadLength( const char*& p )
{
	return gear::parse_unsigned_decimal( &p );
}

static const char* ReadLName( const char*& p )
{
	unsigned length = ReadLength( p );
	
	const char* begin = p;
	
	p += length;
	
	return begin;
}

static void ReadInteger( plus::var_string& out, const char* begin, const char* end, unsigned x )
{
	if ( *begin != '-' )
	{
		char code[] = "'code'";
		
		code[4] =  x        & 0xff;
		code[3] = (x >>= 8) & 0xff;
		code[2] = (x >>= 8) & 0xff;
		code[1] = (x >>= 8) & 0xff;
		
		bool isCode =    iota::is_print( code[1] )
		              && iota::is_print( code[2] )
		              && iota::is_print( code[3] )
		              && iota::is_print( code[4] );
		
		if ( isCode )
		{
			out.append( code, sizeof code );
			
			return;
		}
	}
	
	out.append( begin, end );
}


class Unmangler
{
	public:
		virtual ~Unmangler()
		{
		}
		
		virtual const char* FindTemplateParameters( const char* p ) = 0;
		
		virtual bool TemplateParameterFollows( const char* p ) = 0;
		
		virtual bool TemplateParameterListEndsHere( const char* p ) = 0;
		
		
		void ReadTemplateParameter( plus::var_string& out, const char*& p );
		
		void ReadTemplateParameters( plus::var_string& out, const char*&p );
		
		void ExpandTemplates( plus::var_string& out, const plus::string& name );
		
		void ReadQualName( plus::var_string& out, const char*& p );
		
		void ReadQualifiedName( plus::var_string& out, const char*& p );
		
		void ReadFunctionType( plus::var_string& out, const char*& p );
		
		void ReadIndirectType( plus::var_string& out, const char*& p );
		
		void ReadQualifiedType( plus::var_string& out, const char*& p );
		
		void ReadType( plus::var_string& out, const char*& p );
		
		void ReadConversion( plus::var_string& out, const char*& p )
		{
			out += "operator ";
			
			ReadType( out, p );
		}
		
		void ReadSpecialName( plus::var_string& out, const char*& p );
		
		void ReadIdentifier( plus::var_string& out, const char*& p );
		
		void ReadEntityName( plus::var_string& out, const char*& p );
		
		void ReadSymbol( plus::var_string& out, const char*& p );
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
		const char* FindTemplateParameters( const char* p )  { return strchr( p, '<' ); }
		
		bool TemplateParameterFollows( const char* p )  { return *p == ','; }
		
		bool TemplateParameterListEndsHere( const char* p )  { return *p == '>'; }
};


bool MWC68K_Unmangler::TemplateParameterFollows( const char* p )
{
	if ( p[0] != '_' )  return false;
	
	if ( iota::is_digit( p[1] ) )  return true;
	
	if ( p[1] == 'Q'  &&  iota::is_digit( p[2] ) )  return true;
	
	if ( basic_type_entry( p[1] ) != NULL  &&  p[2] == '_' )  return true;
	
	return false;
}

bool MWC68K_Unmangler::TemplateParameterListEndsHere( const char* p )
{
	return p[0] == '_'  &&  ( p[1] == '_'  ||  p[1] == '\0' );
}

const char* MWC68K_Unmangler::FindTemplateParameters( const char* name )
{
	while ( const char* underscore = strchr( name, '_' ) )
	{
		if ( TemplateParameterFollows( underscore ) )
		{
			return underscore;
		}
		
		name = underscore + 1;
	}
	
	return NULL;
}


void Unmangler::ReadTemplateParameter( plus::var_string& out, const char*& p )
{
	if ( *p == '&' )
	{
		ReadSymbol( out, ++p );
		
		return;
	}
	
	const char* integer = p;
	
	const bool negative = *p == '-';
	
	if ( negative )
	{
		++p;
	}
	
	if ( iota::is_digit( *p ) )
	{
		unsigned x = gear::parse_unsigned_decimal( &p );
		
		if ( TemplateParameterListEndsHere( p ) || TemplateParameterFollows( p ) )
		{
			ReadInteger( out, integer, p, x );
			
			return;
		}
		
		p = integer;  // backtrack
	}
	
	ReadType( out, p );
}

void Unmangler::ReadTemplateParameters( plus::var_string& out, const char*&p )
{
	out += "< ";
	
	bool successive = false;
	
	while ( true )
	{
		if ( successive )
		{
			out += ", ";
		}
		
		successive = true;
		
		ReadTemplateParameter( out, ++p );
		
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
	
	out += " >";
}

void Unmangler::ExpandTemplates( plus::var_string& out, const plus::string& name )
{
	const char* params = FindTemplateParameters( name.c_str() );
	
	if ( params == NULL )
	{
		out += name;
		
		return;
	}
	
	const char* p = params;
	
	out.append( name.data(), params );
	
	ReadTemplateParameters( out, p );
}

void Unmangler::ReadQualName( plus::var_string& out, const char*& p )
{
	const char* name = ReadLName( p );
	
	ExpandTemplates( out, plus::string( name, p ) );
}

void Unmangler::ReadQualifiedName( plus::var_string& out, const char*& p )
{
	int count = *p++ - '0';
	
	ReadQualName( out, p );
	
	while ( --count )
	{
		out += "::";
		
		ReadQualName( out, p );
	}
}

void Unmangler::ReadFunctionType( plus::var_string& out, const char*& p )
{
	plus::var_string params;
	
	while ( *p != '\0' )
	{
		if ( *p == '>'  ||  *p == ',' )
		{
			break;
		}
		
		if ( *p == '_' )
		{
			ReadType( out, ++p );
			
			out += " ";
			
			break;
		}
		
		if ( !params.empty() )
		{
			params += ", ";
		}
		
		ReadType( params, p );
	}
	
	out += "( ";
	out += params;
	out += " )";
}

void Unmangler::ReadIndirectType( plus::var_string& out, const char*& p )
{
	plus::var_string result;
	
	switch ( *p )
	{
		case 'R':
			result = "&";
			++p;
			
			break;
		
		case 'P':
			result = "*";
			++p;
			
			break;
		
		case 'M':
			{
				result = "::(";
				
				ReadType( result, ++p );
				
				result += ")*";
			}
			
			break;
		
		default:
			break;
	}
	
	ReadType( out, p );
	
	out += result;
}

void Unmangler::ReadQualifiedType( plus::var_string& out, const char*& p )
{
	bool is_const = *p == 'C';
	
	if ( is_const )
	{
		++p;
	}
	
	bool is_volatile = *p == 'V';
	
	if ( is_volatile )
	{
		++p;
	}
	
	ReadType( out, p );
	
	if ( is_volatile )
	{
		out += " volatile";
	}
	
	if ( is_const )
	{
		out += " const";
	}
}

void Unmangler::ReadType( plus::var_string& out, const char*& p )
{
	if ( (*p >= 'a'  &&  *p <= 'z')  ||  *p == 'S'  ||  *p == 'U' )
	{
		ReadBasicType( out, p );
		
		return;
	}
	
	if ( *p == 'C'  ||  *p == 'V' )
	{
		ReadQualifiedType( out, p );
		
		return;
	}
	
	if ( *p == 'R'  ||  *p == 'P'  ||  *p == 'M' )
	{
		ReadIndirectType( out, p );
		
		return;
	}
	
	if ( iota::is_digit( *p ) )
	{
		ReadQualName( out, p );
		
		return;
	}
	
	if ( *p == 'Q' )
	{
		ReadQualifiedName( out, ++p );
		
		return;
	}
	
	if ( *p == 'A' )
	{
		throw demangle_failed();
	}
	
	if ( *p == 'F' )
	{
		ReadFunctionType( out, ++p );
		
		return;
	}
	
	throw demangle_failed();
}

struct NotSpecial {};

void Unmangler::ReadSpecialName( plus::var_string& out, const char*& p )
{
	const char* q = p;
	
	while ( *q == '_' )
	{
		++q;
	}
	
	const char* double_underscore = strstr( q, "__" );
	
	if ( double_underscore == NULL )
	{
		throw NotSpecial();
	}
	
	plus::string name( p, double_underscore );
	
	if ( name == "__ct" )  { p = double_underscore;/*out += "";*/ return; }
	if ( name == "__dt" )  { p = double_underscore;  out += "~";  return; }
	
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
			
			ReadConversion( out, p );
			
			return;
		}
		
		out += ReadOperator( p, double_underscore );
		
		return;
	}
	
	throw NotSpecial();
}

void Unmangler::ReadIdentifier( plus::var_string& out, const char*& p )
{
	const char* params = FindTemplateParameters( p );
	
	const char* double_underscore = strstr( p + 1, "__" );
	
	const char* id_end = double_underscore ? double_underscore
	                                       : p + strlen( p );
	
	if ( params != NULL  &&  params < id_end )
	{
		out.append( p, params );
		
		p = params;
		
		ReadTemplateParameters( out, p );
		
		return;
	}
	
	out.append( p, id_end );
	
	p = id_end;
}

void Unmangler::ReadEntityName( plus::var_string& out, const char*& p )
{
	if ( strcmp( p, "__end__catch" ) == 0 )
	{
		p += STRLEN( "__end__catch" );
		
		out += "__end__catch";
		
		return;
	}
	
	if ( strcmp( p, "__throw_bad_alloc__3stdFv" ) == 0 )
	{
		p += STRLEN( "__throw_bad_alloc__3stdFv" );
		
		out += "std::__throw_bad_alloc( void )";
		
		return;
	}
	
	if ( *p == '_' )
	{
		try
		{
			ReadSpecialName( out, p );
			
			return;
		}
		catch ( const NotSpecial& )
		{
			// This happens e.g. with _exit
		}
	}
	
	ReadIdentifier( out, p );
}

void Unmangler::ReadSymbol( plus::var_string& out, const char*& p )
{
	if ( p[0] == '.' )
	{
		++p;
	}
	
	plus::var_string function_name;
	
	ReadEntityName( function_name, p );
	
	if ( *p == '\0' )
	{
		out += function_name;
		
		return;
	}
	
	bool has_class_name = false;
	
	p += 2;
	
	if ( iota::is_digit( *p ) )
	{
		ReadQualName( out, p );
		
		has_class_name = true;
	}
	else if ( p[0] == 'Q' )
	{
		ReadQualifiedName( out, ++p );
		
		has_class_name = true;
	}
	
	if ( function_name.empty()  ||  function_name == "~" )
	{
		// The class name was the last thing appended
		function_name += LastName( out );
	}
	
	if ( has_class_name )
	{
		out += "::";
	}
	
	out += function_name;
	
	ReadType( out, p );
}


void demangle_MWC68K( plus::var_string& result, const plus::string& name )
{
	const char* p = name.c_str();
	
	MWC68K_Unmangler().ReadSymbol( result, p );
}

void demangle_MWCPPC( plus::var_string& result, const plus::string& name )
{
	if ( name[0] != '.' )
	{
		result = name;
		
		return;
	}
	
	const char* p = name.c_str();
	
	MWCPPC_Unmangler().ReadSymbol( result, p );
}

}
