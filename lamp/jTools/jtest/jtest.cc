/*	========
 *	jtest.cc
 *	========
 */

// Standard C++
#include <algorithm>
#include <functional>
#include <vector>

// Standard C/C++
#include <cctype>
#include <cstring>

// Standard C
#include <errno.h>
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Iota
#include "iota/decimal.hh"

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


namespace tool
{
	
	namespace NN = Nucleus;
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
	
	static IoOperator ReadOp( const char* s )
	{
		bool input = false;
		bool here = false;
		
		switch ( s[0] )
		{
			case '-':
				return s[1] == '-' ? kClosed : kNoOp;
			
			case '<':
				input = true;
				// fall through
			
			case '>':
				if      ( s[1] == s[0] )  here = true;
				else if ( s[1] != '='  )  return kNoOp;
				
				return IoOperator( (input ? kInput : kOutput)  |  (here ? kHereDoc : kOneLine) );
			
			default:
				return kNoOp;
		}
	}
	
	struct Redirection
	{
		int          fd;
		IoOperator   op;
		std::string  data;
		
		Redirection( int fd, IoOperator op, const std::string& data ) : fd( fd ), op( op ), data( data )  {}
	};
	
	
	static Redirection GetRedirectionFromLine( const std::string&  line,
	                                           text_input::feed&   feed,
	                                           p7::fd_reader       reader )
	{
		std::size_t end_of_fd = line.find_first_not_of( "0123456789" );
		
		int fd_to_redirect = std::atoi( line.substr( 0, end_of_fd - 0 ).c_str() );
		
		std::size_t start_of_op = line.find_first_not_of( " \t", end_of_fd );
		
		std::size_t end_of_op = start_of_op + 2;
		
		if ( line.size() < end_of_op )
		{
			std::fprintf( stderr, "Missing operator in line: %s\n", line.c_str() );
			
			throw p7::exit_failure;
		}
		
		IoOperator op = ReadOp( line.c_str() + start_of_op );
		
		if ( op == kNoOp )
		{
			std::fprintf( stderr, "Unrecognized operator in line: %s\n", line.c_str() );
			
			throw p7::exit_failure;
		}
		
		std::string param;
		
		if ( op != kClosed )
		{
			std::size_t start_of_param = line.find_first_not_of( " \t", end_of_op );
			
			char param0 = line[ start_of_param ];
			char delimiter = ' ';
			
			if ( param0 == '"'  ||  param0 == '\'' )
			{
				delimiter = param0;
				
				++start_of_param;
			}
			
			std::size_t end_of_param = line.find( delimiter, start_of_param );
			
			param = line.substr( start_of_param, end_of_param - start_of_param );
			
			if ( (op & kFormatMask) == kOneLine )
			{
				param += "\n";
			}
			else
			{
				std::string hereDoc;
				
				bool found_terminator = false;
				
				while ( const std::string* s = get_line_from_feed( feed, reader ) )
				{
					std::string nextLine( s->begin(), s->end() - 1 );
					
					if ( nextLine == param )
					{
						found_terminator = true;
						
						break;
					}
					
					hereDoc += nextLine + "\n";
				}
				
				const bool premature_EOF = !found_terminator;
				
				if ( premature_EOF )
				{
					std::fprintf( stderr, "Missing heredoc terminator '%s'\n", param.c_str() );
					
					throw p7::exit_failure;
				}
				
				std::swap( param, hereDoc );
			}
			
			//op &= kIOMask;
			op = IoOperator( op & kIOMask );
		}
		
		return Redirection( fd_to_redirect, op, param );
	}
	
	
	static std::string PrefixLines( const std::string text, const char* prefix = "# " )
	{
		std::string result;
		
		const char* p = text.c_str();
		
		while ( const char* q = std::strchr( p, '\n' ) )
		{
			result += prefix;
			
			result.append( p, ++q );
			
			p = q;
		}
		
		// *p == '\0' unless there's an unterminated trailing line
		
		return result;
	}
	
	static bool DiscrepantOutput( const Redirection& redir )
	{
		if ( redir.op != kOutput )  return false;
		
		std::string actual_output;
		
		char data[ 4096 ];
		
		while ( int bytes_read = read( redir.fd, data, 4096 ) )
		{
			if ( bytes_read == -1 )
			{
				bytes_read = 0;
				std::perror( "Error reading captured output" );
				
				return true;
			}
			
			actual_output.append( data, bytes_read );
		}
		
		
		close( redir.fd );
		
		bool match = actual_output == redir.data;
		
		if ( !match )
		{
			std::fprintf( stdout, "# EXPECTED:\n%s"
			                      "# RECEIVED:\n%s", PrefixLines( redir.data    ).c_str(),
			                                         PrefixLines( actual_output ).c_str() );
		}
		
		return !match;
	}
	
	
	struct Pipe
	{
		int itsFDs[2];
		
		Pipe()  {}
		
		Pipe( const int fds[2] )  { std::copy( fds, fds + 2, itsFDs ); }
	};
	
	
	class TestCase
	{
		private:
			std::string itsCommand;
			std::string itsToDoReason;
			int itsExpectedExitStatus;
			std::vector< Redirection > itsRedirections;
			unsigned itsCountOfPipesNeeded;
			std::vector< Pipe > itsPipes;
			std::vector< int > itsWriteEnds;
		
		public:
			TestCase() : itsExpectedExitStatus(), itsCountOfPipesNeeded()  {}
			
			void SetCommand( const std::string& command )  { itsCommand = command; }
			const std::string& GetToDoReason() const  { return itsToDoReason; }
			void SetToDoReason( const std::string& reason )  { itsToDoReason = reason; }
			void SetExitStatus( int status )  { itsExpectedExitStatus = status; }
			void AddRedirection( const Redirection& redir );
			
			void Redirect( Redirection& redir );
			
			bool Run();
		
		private:
			struct Redirecting
			{
				TestCase& test;
				
				Redirecting( TestCase& test ) : test( test )  {}
				void operator()( Redirection& redir )  { test.Redirect( redir ); }
			};
			
			Redirecting Redirector()  { return Redirecting( *this ); }
			
			void CheckForCompleteness();
			
			void CreatePipes();
			
			void ClosePipeWriters();
			
			bool DoesOutputMatch() const;
	};
	
	
	void TestCase::AddRedirection( const Redirection& redir )
	{
		itsRedirections.push_back( redir );
		
		if ( redir.op == kOutput )
		{
			++itsCountOfPipesNeeded;
		}
	}
	
	void TestCase::CheckForCompleteness()
	{
		if ( itsCommand.empty() )
		{
			std::fprintf( stderr, "Command missing\n" );
			
			throw p7::exit_failure;
		}
	}
	
	
	void TestCase::CreatePipes()
	{
		unsigned pipes = itsCountOfPipesNeeded;
		
		while ( pipes-- )
		{
			int fds[2];
			pipe( fds );
			itsPipes.push_back( fds );
		}
	}
	
	void TestCase::Redirect( Redirection& redir )
	{
		int pipe_fds[2];
		int* fds = pipe_fds;
		
		switch ( redir.op )
		{
			case kClosed:
				close( redir.fd );
				break;
			
			case kInputLine:
				pipe( fds );
				write( fds[1], redir.data.data(), redir.data.size() );
				close( fds[1] );
				dup2( fds[0], redir.fd );
				close( fds[0] );
				break;
			
			case kMatchOutputLine:
				fds = itsPipes.back().itsFDs;
				dup2( fds[1], redir.fd );
				close( fds[1] );
				redir.fd = fds[0];
				close( fds[0] );
				itsWriteEnds.push_back( fds[1] );
				itsPipes.pop_back();
				break;
			
			default:
				std::fprintf( stderr, "Error in redirection\n" );
				break;
		}
	}
	
	void TestCase::ClosePipeWriters()
	{
		std::for_each( itsWriteEnds.begin(),
			           itsWriteEnds.end(),
			           std::ptr_fun( close ) );
		
		itsWriteEnds.clear();
	}
	
	bool TestCase::DoesOutputMatch() const
	{
		std::vector< Redirection >::const_iterator it = std::find_if( itsRedirections.begin(),
		                                                              itsRedirections.end(),
		                                                              std::ptr_fun( DiscrepantOutput ) );
		
		bool output_matches = it == itsRedirections.end();
		
		return output_matches;
	}
	
	
	bool TestCase::Run()
	{
		CheckForCompleteness();
		
		CreatePipes();
		
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			std::for_each( itsRedirections.begin(),
			               itsRedirections.end(),
			               Redirector() );
			
			const char* argv[] = { "sh", "-c", "", NULL };
			
			argv[2] = itsCommand.c_str();
			
			p7::execv( "/bin/sh", argv );
		}
		
		ClosePipeWriters();
		
		p7::wait_t wait_status = p7::wait();
		
		bool output_matches = DoesOutputMatch();
		bool status_matches = NN::Convert< p7::exit_t >( wait_status ) == itsExpectedExitStatus;
		
		bool test_ok = status_matches && output_matches;
		
		return test_ok;
	}
	
	
	static void RunTest( TestCase& test )
	{
		static unsigned gLastNumber = 0;
		
		bool test_ok = test.Run();
		
		std::string result = test_ok ? "ok" : "not ok";
		
		result += " ";
		result += iota::inscribe_decimal( ++gLastNumber );
		
		const std::string& reason = test.GetToDoReason();
		
		if ( !reason.empty() )
		{
			result += " # TODO " + reason;
		}
		
		result += "\n";
		
		p7::write( p7::stdout_fileno, result );
	}
	
	#ifndef O_CLOEXEC
	#define O_CLOEXEC 0
	#endif
	
	int Main( int argc, iota::argv_t argv )
	{
		const char* jtest = argv[0];
		
		int fd = 0;  // Default to stdin
		
		if ( argc > 1  &&  !PathnameMeansStdIn( argv[1] ) )
		{
			fd = open( argv[1], O_RDONLY | O_CLOEXEC );
			
			if ( fd == -1 )
			{
				std::fprintf( stderr, "%s: %s: %s\n", jtest, argv[1], std::strerror( errno ) );
				
				return 1;
			}
			
			if ( !O_CLOEXEC )
			{
				p7::fcntl< p7::f_setfd >( p7::fd_t( fd ), p7::fd_cloexec );
			}
		}
		
		std::vector< TestCase > battery;
		TestCase test;
		
		text_input::feed feed;
		
		p7::fd_reader reader = p7::fd_t( fd );
		
		while ( const std::string* s = get_line_from_feed( feed, reader ) )
		{
			std::string line( s->begin(), s->end() - 1 );
			
			if ( line.empty() )  continue;
			
			if ( line[0] == '#' )
			{
				// comment
				continue;
			}
			
			if ( line[0] == '$' )
			{
				std::string command = line.substr( line.find_first_not_of( " \t", 1 ), line.npos );
				
				test.SetCommand( command );
				
				continue;
			}
			
			if ( line[0] == '?' )
			{
				int exit_status = std::atoi( line.substr( line.find_first_not_of( " \t", 1 ), line.npos ).c_str() );
				test.SetExitStatus( exit_status );
				continue;
			}
			
			if ( std::isdigit( line[0] ) )
			{
				test.AddRedirection( GetRedirectionFromLine( line, feed, reader ) );
				
				continue;
			}
			
			if ( line[0] == '%' )
			{
				battery.push_back( test );
				test = TestCase();
				continue;
			}
			
			if ( line.substr( 0, 4 ) == "TODO" )
			{
				test.SetToDoReason( line.substr( 5, line.npos ) );
				continue;
			}
			
			std::fprintf( stderr, "Unprocessed line: %s\n", line.c_str() );
		}
		
		battery.push_back( test );
		
		std::string header = "1..";
		header += iota::inscribe_decimal( battery.size() );
		header += "\n";
		
		p7::write( p7::stdout_fileno, header );
		
		std::for_each( battery.begin(), battery.end(), std::ptr_fun( RunTest ) );
		
		return 0;
	}

}

