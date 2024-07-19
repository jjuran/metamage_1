/*	========
 *	jtest.cc
 *	========
 */

// Standard C++
#include <vector>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// must
#include "must/pipe.h"
#include "must/write.h"

// iota
#include "iota/char_types.hh"
#include "iota/strings.hh"
#include "iota/swap.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/var_string.hh"
#include "plus/string/concat.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/functions/execv.hh"
#include "poseven/functions/fcntl.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/wait.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


#define PROGRAM  "jtest"

#define NO_CLOSURE  "using '--' to close an fd is unimplemented"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	static bool PathnameMeansStdIn( const char* pathname )
	{
		return    pathname[0] == '-'
		       && pathname[1] == '\0';
	}
	
	enum IoOperator
	{
		kNoOp = -1,
		
		kIOMask = 0x01,
			kInput  = 0,
			kOutput = 1,
		
		kFormatMask = 0x02,
			kOneLine = 0,
			kHereDoc = 2,
		
		kInputLine       = kInput  | kOneLine,  // 0  >=
		kInputHere       = kInput  | kHereDoc,  // 1  >>
		kMatchOutputLine = kOutput | kOneLine,  // 2  <=
		kMatchOutputHere = kOutput | kHereDoc,  // 3  <<
		
		kClosed = 4                             // 4  --
	};
	
	struct Output
	{
		plus::string expected;
		
		int readable_fd;
		int closable_fd;
		int redirect_fd;
		
		Output()  {}
		
		Output( const plus::string& s, int x, int y, int z )
		:
			expected( s ),
			readable_fd( x ),
			closable_fd( y ),
			redirect_fd( z )
		{
		}
	};
	
	static IoOperator ReadOp( const char* s )
	{
		bool input = false;
		bool here = false;
		
		switch ( s[0] )
		{
			case '<':
				input = true;
				// fall through
			
			case '>':
				if      ( s[1] == s[0] )  here = true;
				else if ( s[1] != '='  )  return kNoOp;
				
				return IoOperator( (input ? kInput : kOutput)  |  (here ? kHereDoc : kOneLine) );
			
			case '-':
				if ( s[1] == '-' )
				{
					/*
						This feature doesn't appear to have
						ever been used, much less tested.
					*/
					
					fprintf( stderr, "%s", PROGRAM ": " NO_CLOSURE "\n" );
					
//					return kClosed;
				}
				// fall through
			
			default:
				return kNoOp;
		}
	}
	
	struct Redirection
	{
		int           fd;
		IoOperator    op;
		plus::string  data;
		
		Redirection( int fd, IoOperator op, const plus::string& data )
		:
			fd( fd ),
			op( op ),
			data( data )
		{
		}
	};
	
	
	static Redirection GetRedirectionFromLine( const plus::string&  line,
	                                           text_input::feed&    feed,
	                                           p7::fd_reader        reader )
	{
		size_t end_of_fd = line.find_first_not_of( "0123456789" );
		
		int fd_to_redirect = gear::parse_unsigned_decimal( line.substr( 0, end_of_fd - 0 ).c_str() );
		
		size_t start_of_op = line.find_first_not_of( " \t", end_of_fd );
		
		size_t end_of_op = start_of_op + 2;
		
		if ( line.size() < end_of_op )
		{
			fprintf( stderr, "Missing operator in line: %s\n", line.c_str() );
			
			throw p7::exit_failure;
		}
		
		IoOperator op = ReadOp( line.c_str() + start_of_op );
		
		if ( op == kNoOp )
		{
			fprintf( stderr, "Unrecognized operator in line: %s\n", line.c_str() );
			
			throw p7::exit_failure;
		}
		
		plus::var_string param;
		
		if ( op != kClosed )
		{
			size_t start_of_param = line.find_first_not_of( " \t", end_of_op );
			
			char param0 = line[ start_of_param ];
			char delimiter = ' ';
			
			if ( param0 == '"'  ||  param0 == '\'' )
			{
				delimiter = param0;
				
				++start_of_param;
			}
			
			size_t end_of_param = line.find( delimiter, start_of_param );
			
			param = line.substr( start_of_param, end_of_param - start_of_param );
			
			if ( (op & kFormatMask) == kOneLine )
			{
				param += "\n";
			}
			else
			{
				plus::var_string hereDoc;
				
				bool found_terminator = false;
				
				while ( const plus::string* s = get_line_bare_from_feed( feed, reader ) )
				{
					plus::string nextLine( *s );
					
					if ( nextLine == param )
					{
						found_terminator = true;
						
						break;
					}
					
					hereDoc += nextLine;
					hereDoc += "\n";
				}
				
				const bool premature_EOF = !found_terminator;
				
				if ( premature_EOF )
				{
					fprintf( stderr, "Missing heredoc terminator '%s'\n", param.c_str() );
					
					throw p7::exit_failure;
				}
				
				using iota::swap;
				
				swap( param, hereDoc );
			}
			
			//op &= kIOMask;
			op = IoOperator( op & kIOMask );
		}
		
		return Redirection( fd_to_redirect, op, param );
	}
	
	
	static plus::string PrefixLines( const plus::string& text, const char* prefix = "# " )
	{
		plus::var_string result;
		
		const char* p = text.c_str();
		
		while ( const char* q = strchr( p, '\n' ) )
		{
			result += prefix;
			
			result.append( p, ++q );
			
			p = q;
		}
		
		// *p == '\0' unless there's an unterminated trailing line
		
		return result;
	}
	
	static
	bool DiscrepantOutput( const Output& output )
	{
		int fd = output.readable_fd;
		
		const plus::string& expected = output.expected;
		
		plus::var_string actual_output;
		
		char data[ 4096 ];
		
		while ( ssize_t bytes_read = read( fd, data, 4096 ) )
		{
			if ( bytes_read < 0 )
			{
				bytes_read = 0;
				perror( "Error reading captured output" );
				
				return true;
			}
			
			actual_output.append( data, bytes_read );
		}
		
		
		close( fd );
		
		bool match = actual_output == expected;
		
		if ( !match )
		{
			fprintf( stdout, "# EXPECTED:\n%s"
			                 "# RECEIVED:\n%s", PrefixLines( expected      ).c_str(),
			                                    PrefixLines( actual_output ).c_str() );
		}
		
		return !match;
	}
	
	
	class TestCase
	{
		private:
			plus::string itsCommand;
			plus::string itsToDoReason;
			int itsExpectedExitStatus;
			std::vector< Redirection > itsRedirections;
			std::vector< int > its_input_fds;
			std::vector< Output > its_outputs;
		
		public:
			TestCase() : itsExpectedExitStatus()  {}
			
			void SetCommand( const plus::string& command )  { itsCommand = command; }
			const plus::string& GetToDoReason() const  { return itsToDoReason; }
			void SetToDoReason( const plus::string& reason )  { itsToDoReason = reason; }
			void SetExitStatus( int status )  { itsExpectedExitStatus = status; }
			void AddRedirection( const Redirection& redir );
			void Redirect( Redirection& redir );
			
			bool Run();
		
		private:
			void CheckForCompleteness();
			
			void ClosePipeWriters();
			
			bool DoesOutputMatch() const;
	};
	
	
	void TestCase::AddRedirection( const Redirection& redir )
	{
		itsRedirections.push_back( redir );
	}
	
	void TestCase::CheckForCompleteness()
	{
		if ( itsCommand.empty() )
		{
			fprintf( stderr, "Command missing\n" );
			
			throw p7::exit_failure;
		}
	}
	
	void TestCase::ClosePipeWriters()
	{
		for ( int i = 0;  i < its_outputs.size();  ++i )
		{
			int fd = its_outputs[ i ].closable_fd;
			
			close( fd );
		}
	}
	
	void TestCase::Redirect( Redirection& redir )
	{
		int pipe_fds[2];
		
		must_pipe( pipe_fds );
		
		if ( redir.op == kInput )
		{
			must_write( pipe_fds[ 1 ], redir.data.data(), redir.data.size() );
			
			dup2( pipe_fds[ 0 ], redir.fd );
			
			close( pipe_fds[ 0 ] );
			close( pipe_fds[ 1 ] );
			
			its_input_fds.push_back( redir.fd );
		}
		else
		{
			Output output( redir.data, pipe_fds[ 0 ], pipe_fds[ 1 ], redir.fd );
			
			its_outputs.push_back( output );
		}
	}
	
	bool TestCase::DoesOutputMatch() const
	{
		for ( int i = 0;  i < its_outputs.size();  ++i )
		{
			if ( DiscrepantOutput( its_outputs[ i ] ) )
			{
				return false;
			}
		}
		
		return true;
	}
	
	
	bool TestCase::Run()
	{
		CheckForCompleteness();
		
		for ( int i = 0;  i < itsRedirections.size();  ++i )
		{
			Redirect( itsRedirections[ i ] );
		}
		
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			for ( int i = 0;  i < its_outputs.size();  ++i )
			{
				int old_fd = its_outputs[ i ].closable_fd;
				int new_fd = its_outputs[ i ].redirect_fd;
				
				dup2( old_fd, new_fd );
				
				close( old_fd );
			}
			
			const char* argv[] = { "sh", "-c", "", NULL };
			
			argv[2] = itsCommand.c_str();
			
			p7::execv( "/bin/sh", argv );
		}
		
		ClosePipeWriters();
		
		p7::wait_t wait_status = p7::wait();
		
		bool output_matches = DoesOutputMatch();
		bool status_matches = n::convert< p7::exit_t >( wait_status ) == itsExpectedExitStatus;
		
		bool test_ok = status_matches && output_matches;
		
		return test_ok;
	}
	
	
	static void RunTest( TestCase& test )
	{
		static unsigned gLastNumber = 0;
		
		bool test_ok = test.Run();
		
		plus::var_string result = test_ok ? "ok" : "not ok";
		
		result += " ";
		result += gear::inscribe_decimal( ++gLastNumber );
		
		const plus::string& reason = test.GetToDoReason();
		
		if ( !reason.empty() )
		{
			result += " # TODO ";
			result += reason;
		}
		
		result += "\n";
		
		p7::write( p7::stdout_fileno, result );
	}
	
	#ifndef O_CLOEXEC
	#define O_CLOEXEC 0
	#endif
	
	int Main( int argc, char** argv )
	{
		const char* jtest = argv[0];
		
		int fd = 0;  // Default to stdin
		
		if ( argc > 1  &&  !PathnameMeansStdIn( argv[1] ) )
		{
			fd = open( argv[1], O_RDONLY | O_CLOEXEC );
			
			if ( fd < 0 )
			{
				fprintf( stderr, "%s: %s: %s\n", jtest, argv[1], strerror( errno ) );
				
				return 1;
			}
			
			if ( !O_CLOEXEC )
			{
				p7::fcntl< p7::f_setfd >( p7::fd_t( fd ), p7::fd_cloexec );
			}
		}
		
		bool succeeding = false;  // as in following, not as in winning
		
		std::vector< TestCase > battery;
		TestCase test;
		
		text_input::feed feed;
		
		p7::fd_reader reader = p7::fd_t( fd );
		
		while ( const plus::string* s = get_line_bare_from_feed( feed, reader ) )
		{
			plus::string line( *s );
			
			if ( line.empty() )  continue;
			
			char c = line[ 0 ];
			
			if ( c == '#'  ||  c == '%' )
			{
				// comment
				continue;
			}
			
			if ( c == '$' )
			{
				if ( succeeding )
				{
					battery.push_back( test );
					
					test = TestCase();
				}
				
				succeeding = true;
				
				plus::string command = line.substr( line.find_first_not_of( " \t", 1 ) );
				
				test.SetCommand( command );
				
				continue;
			}
			
			if ( c == '?' )
			{
				int exit_status = gear::parse_unsigned_decimal( line.substr( line.find_first_not_of( " \t", 1 ) ).c_str() );
				
				test.SetExitStatus( exit_status );
				continue;
			}
			
			if ( iota::is_digit( c ) )
			{
				test.AddRedirection( GetRedirectionFromLine( line, feed, reader ) );
				
				continue;
			}
			
			const char* line_c_str = line.c_str();
			
			if ( memcmp( line_c_str, STR_LEN( "TODO" ) ) == 0 )
			{
				test.SetToDoReason( line.substr( 5 ) );
				continue;
			}
			
			fprintf( stderr, "Unprocessed line: %s\n", line_c_str );
		}
		
		battery.push_back( test );
		
		plus::var_string header = "1..";
		
		header += gear::inscribe_decimal( battery.size() );
		header += "\n";
		
		p7::write( p7::stdout_fileno, header );
		
		typedef std::vector< TestCase >::iterator Iter;
		
		for ( Iter it = battery.begin();  it != battery.end();  ++it )
		{
			RunTest( *it );
		}
		
		return 0;
	}

}
