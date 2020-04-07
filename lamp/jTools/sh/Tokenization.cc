// ===============
// Tokenization.cc
// ===============

#include "Tokenization.hh"

// iota
#include "iota/char_types.hh"

// gear
#include "gear/parse_decimal.hh"


namespace ShellShock
{
	
	static bool IsAShellMetacharacter( char c )
	{
		switch ( c )
		{
			case '|':
			case '&':
			case ';':
			case '<':
			case '>':
			case '(':
			case ')':
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				return true;
				break;
			
			default:
				break;
		}
		
		return false;
	}
	
	static ControlInfo IsAControlOperator( const char* p )
	{
		unsigned char len = 0;
		ControlOperator op = kControlNone;
		
		switch ( *p )
		{
			case '|':
				len = ( p[ 0 ] == p[ 1 ] ) ? 2 : 1;
				
				op = ( len == 1 ) ? kControlPipe
				                  : kControlOr;
				break;
			
			case '&':
				len = ( p[ 0 ] == p[ 1 ] ) ? 2 : 1;
				
				op = ( len == 1 ) ? kControlBackground
				                  : kControlAnd;
				break;
			
			case ';':
				len = ( p[ 0 ] == p[ 1 ] ) ? 2 : 1;
				
				op = ( len == 1 ) ? kControlSequential
				                  : kControlEndCase;
				break;
			
			case '\r':
			case '\n':
				len = 1;
				op = kControlSequential;
				break;
			
			case '(':
				len = 1;
				op = kControlOpenSubshell;
				break;
			
			case ')':
				len = 1;
				op = kControlCloseSubshell;
				break;
		}
		
		return ControlInfo( op, len );
	}
	
	static RedirectInfoWithLength IsARedirectionOperator( const char* p )
	{
		int fd = -1;
		
		const char* q = p;
		
		if ( iota::is_digit( *q ) )
		{
			fd = gear::parse_unsigned_decimal( &q );
		}
		
		unsigned char len = 0;
		RedirectionOperator op = kRedirectNone;
		
		switch ( q[ 0 ] )
		{
			case '<':
				len = 2;
				
				switch ( q[ 1 ] )
				{
					case '<':
						if ( q[2] == '-' )
						{
							len = 3;
							op = kRedirectInputHereStrippingTabs;
						}
						else
						{
							op = kRedirectInputHere;
						}
						break;
					
					case '&':
						op = kRedirectInputDuplicate;
						break;
					
					case '>':
						op = kRedirectInputAndOutput;
						break;
					
					default:
						len = 1;
						op = kRedirectInput;
				}
				
				if ( fd == -1  &&  op != kRedirectInputAndOutput )
				{
					fd = 0;
				}
				
				break;
			
			case '>':
				len = 2;
				switch ( q[ 1 ] )
				{
					case '|':
						op = kRedirectOutputClobbering;
						break;
					
					case '>':
						op = kRedirectOutputAppending;
						break;
					
					case '&':
						op = kRedirectOutputDuplicate;
						break;
					
					default:
						len = 1;
						op = kRedirectOutput;
					}
					
					if ( fd == -1 )
					{
						fd = 1;
					}
				break;
			
			case '&':
				if ( q[ 1 ] == '>' )
				{
					len = 2;
					op = kRedirectOutputAndError;
				}
				break;
		}
		
		return RedirectInfoWithLength( RedirectInfo( op, fd ), (q - p) + len );
	}
	
	static const char* SkipWhitespace( const char* p )
	{
		while ( *p == ' '  ||  *p == '\t' )
		{
			p++;
		}
		
		return p;
	}
	
	
	static inline bool IsNewLine( char c )
	{
		return c == '\r'  ||  c == '\n';
	}
	
	static const char* SkipPastNewLine( const char* p )
	{
		while ( *p != '\0'  &&  !IsNewLine( *p ) )
		{
			p++;
		}
		
		while ( IsNewLine( *p ) )
		{
			p++;
		}
		
		return p;
	}
	
	static const char* SkipPastNextSingleQuote( const char* str )
	{
		const char* p = str;
		p++;
		
		while ( *p != '\0' && *p != '\'' )
		{
			p++;
		}
		
		if ( *p != '\0' )
		{
			p++;
		}
		
		return p;
	}
	
	static const char* SkipPastNextDoubleQuote( const char* str )
	{
		const char* p = str;
		p++;
		
		while ( *p != '\0' && *p != '"' )
		{
			if ( *p == '\\' )
			{
				p++;
				
				if ( *p != '\0' )
				{
					p++;
				}
			}
			else
			{
				p++;
			}
		}
		
		if ( *p != '\0' )
		{
			p++;
		}
		
		return p;
	}
	
	static const char* SkipOverToken( const char* p )
	{
		while ( *p != '\0' && !IsAShellMetacharacter( *p ) )
		{
			switch (*p)
			{
				case '\\':
					p++;
					
					if ( *p != '\0' )
					{
						p++;
					}
					break;
				
				case '\'':
					p = SkipPastNextSingleQuote( p );
					break;
				
				case '"':
					p = SkipPastNextDoubleQuote( p );
					break;
				
				default:
					p++;
					break;
			}
		}
		
		return p;
	}
	
	List Tokenization( const plus::string& cmdLine )
	{
		const char *p, *q, *r;
		short n = 0;
		
		const char* cmd = cmdLine.c_str();  // a conversion gives us the actual pointer
		
		// skip leading space
		p = SkipWhitespace( cmd );
		
		if ( *p == '\0' )
		{
			return List();
		}
		
		List resultList( 1,
		                 Circuit( std::vector< Pipeline >( 1,
		                                                   Pipeline( std::vector< Command >( 1 ) ) ) ) );
		
		for ( r = q = p;  *r != '\0';  q = r, ++n )
		{
			// Check for a comment.  If there is one, stop processing.
			if ( *r == '#' )
			{
				r = SkipPastNewLine( r );
				continue;
			}
			
			bool isAnOperator = false;
			// Check for redirection operator first so that &> doesn't match &
			RedirectInfoWithLength redirectInfo = IsARedirectionOperator( r );
			
			if ( redirectInfo.op != kRedirectNone )
			{
				r += redirectInfo.len;
				const char* paramBegin = SkipWhitespace( r );
				r = SkipOverToken( paramBegin );
				
				plus::string param( paramBegin, r );
				
				resultList.back().pipelines.back()
				                 .commands.back()
				                 .redirections.push_back( Redirection( redirectInfo, param ) );
				
				isAnOperator = true;
			}
			else
			{
				ControlInfo controlInfo = IsAControlOperator( r );
				
				if ( controlInfo.op != kControlNone )
				{
					r += controlInfo.len;
					
					switch ( controlInfo.op )
					{
						case kControlPipe:
							resultList.back().pipelines.back().commands.push_back( Command() );
								break;
						
						case kControlOr:
						case kControlAnd:
							resultList.back().pipelines.push_back( Pipeline( std::vector< Command >( 1 ),
							                                                 controlInfo.op ) );
							break;
						
						case kControlBackground:
						case kControlSequential:
							resultList.back().op = controlInfo.op;
							resultList.push_back( Circuit( std::vector< Pipeline >( 1,
							                                                        Pipeline( std::vector< Command >( 1 ) ) ) ) );
							break;
						
						default:
							// Unsupported operator
							break;
					}
					
					isAnOperator = true;
				}
			}
			
			if ( !isAnOperator )
			{
				// scan over arg
				r = SkipOverToken( r );
				// add arg to vector
				resultList.back().pipelines.back()
				                 .commands.back()
				                 .args.push_back( plus::string( q, r - q ) );
			}
			
			// skip intervening space
			r = SkipWhitespace( r );
		}
		
		if ( resultList.back().pipelines.back().commands.back().args.empty() )
		{
			resultList.pop_back();
		}
		
		return resultList;
	}
	
}
