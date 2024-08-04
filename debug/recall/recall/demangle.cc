/*	===========
 *	demangle.cc
 *	===========
 */

#include "recall/demangle.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/char_types.hh"
#include "iota/strings.hh"

// more-libc
#include "more/string.h"

// gear
#include "gear/find.hh"
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

static const Operator* find_operator( const char* code )
{
	const Operator* begin = gOperators;
	const Operator* end   = begin + sizeof gOperators / sizeof gOperators[0];
	
	for ( const Operator* it = begin;  it != end;  ++it )
	{
		if ( begins_with( code, it->code ) )
		{
			return it;
		}
	}
	
	return NULL;
}

static const char* ReadOperator( const char*& p, const char* end )
{
	if ( const Operator* it = find_operator( p ) )
	{
		p = end;
		
		return it->name;
	}
	
	return NULL;
}

template < class String >
static inline
unsigned last_name_offset( const String& qualified_name )
{
	const char* data = qualified_name.data();
	size_t      size = qualified_name.size();
	
	const char* found = gear::find_last_match( data, size, ":", 1 );
	
	return found ? found + 1 - data : 0;
}

static inline
const char* basic_type_entry( unsigned char c )
{
	const unsigned char i = c - 'a';
	
	if ( i > 'z' - 'a' )  return NULL;
	
	return gBasicTypes[ i ];
}

static inline
bool is_basic_type( char c )
{
	return basic_type_entry( c ) != NULL;
}

enum Stat
{
	Stat_failed,
	Stat_ok,
	Stat_special = -1,
};

static
Stat ReadBasicType( plus::var_string& out, const char*& p )
{
	const char* signage = "unsigned ";
	
	char c = *p++;
	
	if ( c < 'a' )
	{
		int skip = 'U' - c;  //  2 for 'S', 0 for 'U'
		
		signage += skip;
		
		out += signage;
		
		c = *p++;
	}
	
	const char* type = basic_type_entry( c );
	
	if ( type == NULL )
	{
		return Stat_failed;
	}
	
	out += type;
	
	return Stat_ok;
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
		
		char* p = code + 5;
		
		*--p = x;
		*--p = x >>= 8;
		*--p = x >>= 8;
		*--p = x >>= 8;
		
		bool isCode =    iota::is_print( *p++ )
		              && iota::is_print( *p++ )
		              && iota::is_print( *p++ )
		              && iota::is_print( *p++ );
		
		if ( isCode )
		{
			out.append( code, sizeof code - 1 );
			
			return;
		}
	}
	
	out.append( begin, end );
}


class Unmangler
{
	private:
		bool it_is_punctuated;  // PPC symbols use [<,>] for templates
	
	public:
		Unmangler( bool punctuated ) : it_is_punctuated( punctuated )
		{
		}
		
		const char* FindTemplateParameters( const char* p );
		
		bool TemplateParameterFollows( const char* p );
		
		bool TemplateParameterListEndsHere( const char* p );
		
		
		Stat ReadTemplateParameter( plus::var_string& out, const char*& p );
		
		Stat ReadTemplateParameters( plus::var_string& out, const char*& p );
		
		Stat ExpandTemplates( plus::var_string& out, const char* name );
		
		Stat ReadQualName( plus::var_string& out, const char*& p );
		
		Stat ReadQualifiedName( plus::var_string& out, const char*& p );
		
		Stat ReadFunctionType( plus::var_string& out, const char*& p );
		
		Stat ReadIndirectType( plus::var_string& out, const char*& p );
		
		Stat ReadQualifiedType( plus::var_string& out, const char*& p );
		
		Stat ReadType( plus::var_string& out, const char*& p );
		
		Stat ReadConversion( plus::var_string& out, const char*& p )
		{
			out += "operator ";
			
			return ReadType( out, p );
		}
		
		Stat ReadSpecialName( plus::var_string& out, const char*& p );
		
		Stat ReadIdentifier( plus::var_string& out, const char*& p );
		
		Stat ReadEntityName( plus::var_string& out, const char*& p );
		
		Stat ReadSymbol( plus::var_string& out, const char*& p );
};

class MWC68K_Unmangler : public Unmangler
{
	public:
		MWC68K_Unmangler() : Unmangler( false )
		{
		}
};

class MWCPPC_Unmangler : public Unmangler
{
	public:
		MWCPPC_Unmangler() : Unmangler( true )
		{
		}
};


bool Unmangler::TemplateParameterFollows( const char* p )
{
	if ( it_is_punctuated )
	{
		return *p == ',';
	}
	
	if ( p[0] != '_' )  return false;
	
	if ( iota::is_digit( p[1] ) )  return true;
	
	if ( p[1] == 'Q'  &&  iota::is_digit( p[2] ) )  return true;
	
	if ( is_basic_type( p[1] )  &&  p[2] == '_' )  return true;
	
	return false;
}

bool Unmangler::TemplateParameterListEndsHere( const char* p )
{
	if ( it_is_punctuated )
	{
		return *p == '>';
	}
	
	return p[0] == '_'  &&  ( p[1] == '_'  ||  p[1] == '\0' );
}

const char* Unmangler::FindTemplateParameters( const char* name )
{
	if ( it_is_punctuated )
	{
		return strchr( name, '<' );
	}
	
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


Stat Unmangler::ReadTemplateParameter( plus::var_string& out, const char*& p )
{
	if ( *p == '&' )
	{
		return ReadSymbol( out, ++p );
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
			
			return Stat_ok;
		}
		
		p = integer;  // backtrack
	}
	
	return ReadType( out, p );
}

Stat Unmangler::ReadTemplateParameters( plus::var_string& out, const char*&p )
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
		
		Stat stat = ReadTemplateParameter( out, ++p );
		
		if ( ! stat )
		{
			return stat;
		}
		
		if ( TemplateParameterListEndsHere( p ) )
		{
			++p;
			break;
		}
		
		if ( !TemplateParameterFollows( p ) )
		{
			return Stat_failed;
		}
	}
	
	out += " >";
	
	return Stat_ok;
}

Stat Unmangler::ExpandTemplates( plus::var_string& out, const char* name )
{
	const char* params = FindTemplateParameters( name );
	
	if ( params == NULL )
	{
		out += name;
		
		return Stat_ok;
	}
	
	const char* p = params;
	
	out.append( name, params );
	
	return ReadTemplateParameters( out, p );
}

Stat Unmangler::ReadQualName( plus::var_string& out, const char*& p )
{
	const char* name = ReadLName( p );
	
	size_t len = p - name;
	
	char* c_str = (char*) alloca( len + 1 );
	
	char* q = (char*) mempcpy( c_str, name, len );
	
	*q = '\0';
	
	return ExpandTemplates( out, c_str );
}

Stat Unmangler::ReadQualifiedName( plus::var_string& out, const char*& p )
{
	int count = *p++ - '0';
	
	Stat stat = ReadQualName( out, p );
	
	while ( stat  &&  --count )
	{
		out += "::";
		
		stat = ReadQualName( out, p );
	}
	
	return stat;
}

Stat Unmangler::ReadFunctionType( plus::var_string& out, const char*& p )
{
	plus::var_string params;
	
	Stat stat = Stat_ok;
	
	while ( stat  &&  *p != '\0' )
	{
		if ( *p == '>'  ||  *p == ',' )
		{
			break;
		}
		
		if ( *p == '_' )
		{
			stat = ReadType( out, ++p );
			
			out += " ";
			
			break;
		}
		
		if ( !params.empty() )
		{
			params += ", ";
		}
		
		stat = ReadType( params, p );
	}
	
	out += "( ";
	out += params;
	out += " )";
	
	return stat;
}

Stat Unmangler::ReadIndirectType( plus::var_string& out, const char*& p )
{
	plus::var_string result;
	
	Stat stat = Stat_ok;
	
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
				
				stat = ReadType( result, ++p );
				
				result += ")*";
			}
			
			break;
		
		default:
			break;
	}
	
	if ( stat )
	{
		stat = ReadType( out, p );
		
		out += result;
	}
	
	return stat;
}

Stat Unmangler::ReadQualifiedType( plus::var_string& out, const char*& p )
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
	
	Stat stat = ReadType( out, p );
	
	if ( is_volatile )
	{
		out += " volatile";
	}
	
	if ( is_const )
	{
		out += " const";
	}
	
	return stat;
}

Stat Unmangler::ReadType( plus::var_string& out, const char*& p )
{
	if ( (*p >= 'a'  &&  *p <= 'z')  ||  *p == 'S'  ||  *p == 'U' )
	{
		return ReadBasicType( out, p );
	}
	
	if ( *p == 'C'  ||  *p == 'V' )
	{
		return ReadQualifiedType( out, p );
	}
	
	if ( *p == 'R'  ||  *p == 'P'  ||  *p == 'M' )
	{
		return ReadIndirectType( out, p );
	}
	
	if ( iota::is_digit( *p ) )
	{
		return ReadQualName( out, p );
	}
	
	if ( *p == 'Q' )
	{
		return ReadQualifiedName( out, ++p );
	}
	
	if ( *p == 'F' )
	{
		return ReadFunctionType( out, ++p );
	}
	
	if ( *p == 'A' )
	{
		// currently unsupported
	}
	
	return Stat_failed;
}

Stat Unmangler::ReadSpecialName( plus::var_string& out, const char*& p )
{
	const char* q = p;
	
	while ( *q == '_' )
	{
		++q;
	}
	
	const char* double_underscore = strstr( q, "__" );
	
	if ( double_underscore == NULL )
	{
		return Stat_special;
	}
	
	plus::string name( p, double_underscore );
	
	if ( name == "__ct" )  { p = double_underscore;/*out += "";*/ return Stat_ok; }
	if ( name == "__dt" )  { p = double_underscore;  out += "~";  return Stat_ok; }
	
	if ( name == "_vtbl"     )  return Stat_failed;
	if ( name == "_rttivtbl" )  return Stat_failed;
	if ( name == "_vbtbl"    )  return Stat_failed;
	if ( name == "__rtti"    )  return Stat_failed;
	if ( name == "__ti"      )  return Stat_failed;
	if ( name == "___ti"     )  return Stat_failed;
	
	if ( p[1] == '_' )
	{
		p += 2;
		
		if ( p[0] == 'o'  &&  p[1] == 'p' )
		{
			p += 2;
			
			return ReadConversion( out, p );
		}
		
		if ( const char* op = ReadOperator( p, double_underscore ) )
		{
			out += op;
			
			return Stat_ok;
		}
		
		return Stat_failed;
	}
	
	return Stat_special;
}

Stat Unmangler::ReadIdentifier( plus::var_string& out, const char*& p )
{
	const char* params = FindTemplateParameters( p );
	
	const char* double_underscore = strstr( p + 1, "__" );
	
	const char* id_end = double_underscore ? double_underscore
	                                       : p + strlen( p );
	
	if ( params != NULL  &&  params < id_end )
	{
		out.append( p, params );
		
		p = params;
		
		return ReadTemplateParameters( out, p );
	}
	
	out.append( p, id_end );
	
	p = id_end;
	
	return Stat_ok;
}

Stat Unmangler::ReadEntityName( plus::var_string& out, const char*& p )
{
	if ( strcmp( p, "__end__catch" ) == 0 )
	{
		p += STRLEN( "__end__catch" );
		
		out += "__end__catch";
		
		return Stat_ok;
	}
	
	if ( strcmp( p, "__throw_bad_alloc__3stdFv" ) == 0 )
	{
		p += STRLEN( "__throw_bad_alloc__3stdFv" );
		
		out += "std::__throw_bad_alloc( void )";
		
		return Stat_ok;
	}
	
	if ( *p == '_' )
	{
		Stat stat = ReadSpecialName( out, p );
		
		if ( stat >= 0 )
		{
			return stat;
		}
		
		// This happens e.g. with _exit
	}
	
	return ReadIdentifier( out, p );
}

Stat Unmangler::ReadSymbol( plus::var_string& out, const char*& p )
{
	if ( p[0] == '.' )
	{
		++p;
	}
	
	plus::var_string function_name;
	
	Stat stat = ReadEntityName( function_name, p );
	
	if ( ! stat )
	{
		return stat;
	}
	
	if ( *p == '\0' )
	{
		out += function_name;
		
		return Stat_ok;
	}
	
	bool has_class_name = false;
	
	p += 2;
	
	if ( iota::is_digit( *p ) )
	{
		stat = ReadQualName( out, p );
		
		has_class_name = true;
	}
	else if ( p[0] == 'Q' )
	{
		stat = ReadQualifiedName( out, ++p );
		
		has_class_name = true;
	}
	
	if ( ! stat )
	{
		return stat;
	}
	
	if ( function_name.empty()  ||  function_name == "~" )
	{
		// The class name was the last thing appended
		
		size_t offset = last_name_offset( out );
		
		function_name.append( out.data() + offset, out.size() - offset );
	}
	
	if ( has_class_name )
	{
		out += "::";
	}
	
	out += function_name;
	
	return ReadType( out, p );
}


void demangle_MWC68K( plus::var_string& result, const char* name )
{
	Stat stat = MWC68K_Unmangler().ReadSymbol( result, name );
	
	if ( ! stat )
	{
		result = name;
	}
}

void demangle_MWCPPC( plus::var_string& result, const char* name )
{
	if ( name[0] != '.' )
	{
		result = name;
		
		return;
	}
	
	Stat stat = MWCPPC_Unmangler().ReadSymbol( result, name );
	
	if ( ! stat )
	{
		result = name;
	}
}

}
