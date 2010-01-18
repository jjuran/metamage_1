/*	=======
 *	POP3.cc
 *	=======
 */

#include "POP3.hh"

#include <cstdlib>
#include <cstring>

#include <string>
#include <vector>

// Nucleus
#include "Nucleus/Convert.h"

// Io
#include "Io/Stream.hh"


namespace POP3
{
	
	namespace NN = Nucleus;
	
	static inline std::string Censored( const std::string& password )
	{
		return std::string( password.size(), '*' );
	}
	
	namespace Client
	{
		
		static void DebugSent( const std::string& data )
		{
			return;
			
			//Io::Out << "--> " << data << "\n";
		}
		
		static void DebugReceived( const std::string& data )
		{
			return;
			
			//Io::Out << "<-- " << data << "\n";
		}
		
		enum
		{
			maxResponseLength = 512,
			maxMessageLength = 512 * 1024,  // 512K
			maxMessageLineCount = 10240
		};
		
		bool CheckResponse( const std::string& response )
		{
			if ( response.size() >= 3 )
			{
				// Minimum response == "+OK"
				if ( response[ 0 ] == '+'  &&  response.substr( 1, 2 ) == "OK" )
				{
					return true;
				}
				
				if ( response[ 0 ] == '-'  &&  response.substr( 1, 3 ) == "ERR" )
				{
					return false;
				}
			}
			
			throw InvalidResponse();
		}
		
		static bool TestResponse( const std::string& response )
		{
			if ( response[ 0 ] == '+' )  return true;
			if ( response[ 0 ] == '-' )  return false;
			
			throw InvalidResponse();
		}
		
		namespace Commands
		{
			
			struct Debugging {};
			
			typedef std::string CommandName;
			
			class CommandOnly
			{
				private:
					const CommandName& name;
				
				public:
					CommandOnly( const CommandName& name ) : name( name )  {}
					
					std::string Generate( Debugging d = Debugging() ) const  { return name; }
			};
			
			class CommandWithString
			{
				private:
					const CommandName& name;
					const std::string& arg;
				
				public:
					CommandWithString( const CommandName& name,
					                   const std::string& arg )
					:
						name( name ),
						arg ( arg  )
					{}
					
					std::string Generate( Debugging d = Debugging() ) const
					{
						return name + " " + arg;
					}
			};
			
			class CommandWithSecretString
			{
				private:
					const CommandName& name;
					const std::string& arg;
				
				public:
					CommandWithSecretString( const CommandName& name,
					                         const std::string& arg )
					:
						name( name ),
						arg ( arg  )
					{}
					
					std::string Generate() const
					{
						return name + " " + arg;
					}
					
					std::string Generate( Debugging ) const
					{
						return name + " " + Censored( arg );
					}
			};
			
			class CommandWithTwoStrings
			{
				private:
					const CommandName& name;
					const std::string& arg1;
					const std::string& arg2;
				
				public:
					CommandWithTwoStrings( const CommandName& name,
					             const std::string& arg1,
					             const std::string& arg2 )
					:
						name( name ),
						arg1( arg1 ),
						arg2( arg2 )
					{}
					
					std::string Generate( Debugging d = Debugging() ) const
					{
						return name + " " + arg1
						            + " " + arg2;
					}
			};
			
			class CommandWithInteger
			{
				private:
					const CommandName& name;
					int arg;
				
				public:
					CommandWithInteger( const CommandName& name,
					                    int arg )
					:
						name( name ),
						arg ( arg  )
					{}
					
					std::string Generate( Debugging d = Debugging() ) const
					{
						return name + " " + NN::Convert< std::string >( arg );
					}
			};
			
			class CommandWithTwoIntegers
			{
				private:
					const CommandName& name;
					int arg1;
					int arg2;
				
				public:
					CommandWithTwoIntegers( const CommandName& name,
					                        int arg1,
					                        int arg2 )
					:
						name( name ),
						arg1( arg1 ),
						arg2( arg2 )
					{}
					
					std::string Generate( Debugging d = Debugging() ) const
					{
						return name + " " + NN::Convert< std::string >( arg1 )
						            + " " + NN::Convert< std::string >( arg2 );
					}
			};
			
			static char const *const User = "USER";
			static char const *const Pass = "PASS";
			static char const *const Apop = "APOP";
			static char const *const Stat = "STAT";
			static char const *const List = "LIST";
			static char const *const Top  = "TOP";
			static char const *const Retr = "RETR";
			static char const *const Dele = "DELE";
			static char const *const Quit = "QUIT";
			
			static char const *const Password = Pass;
			static char const *const Retrieve = Retr;
			static char const *const Delete = Dele;
			
		}  // namespace Commands
		
		Session::Session( Io::Handle socket )
		:
			io   ( socket ),
			input( socket )
		{
			std::string greeting = GetResponse( input );
		}
		
		void Session::ReceiveResponse()
		{
			std::string response = GetResponse( input );
			
			if ( TestResponse( response ) )
			{
				// do nothing
			}
			else
			{
				throw NegativeResponse();
			}
		}
		
		void Session::ReceiveResponseMultiLine( std::vector< std::string >& outLines )
		{
			std::string response = GetResponse( input );
			
			if ( TestResponse( response ) )
			{
				ReadMultiLineData( outLines );
			}
			else
			{
				throw NegativeResponse();
			}
		}
		
		void Session::ReadMultiLineData( std::vector< std::string >& outLines )
		{
			int msgSize = 0;
			
			while ( true /* myInput.Ready() */)
			{
				std::string line = input.Read();
				
				if ( line.size() > 0 )
				{
					if ( line[ 0 ] == '.' )
					{
						if ( line.size() == 1 )
						{
							break;
						}
						else
						{
							line = line.substr( 1, std::string::npos );
						}
					}
				}
				
				msgSize += line.size() + 2;
				
				if ( msgSize > maxMessageLength )
				{
					throw MessageTooLong();
				}
				
				outLines.push_back( line );
				
				if ( outLines.size() > maxMessageLineCount )
				{
					throw MessageTooLong();
				}
			}
		}
		
		template < class Cmd >
		void SendPOPCommand( const Io::Handle& io, const Cmd& command )
		{
			DebugSent( command.Generate( Commands::Debugging() ) );
			
			Io::S( io ) << command.Generate() + "\r\n";
		}
		
		bool Session::User( const std::string& username )
		{
			SendPOPCommand( io, Commands::CommandWithString( Commands::User, username ) );
			
			std::string response = GetResponse( input );
			return TestResponse( response );
		}
		
		bool Session::Password( const std::string& password )
		{
			SendPOPCommand( io, Commands::CommandWithSecretString( Commands::Password, password ) );
			
			std::string response = GetResponse( input );
			return TestResponse( response );
		}
		
		bool Session::Apop( const std::string& username, const std::string& digest )
		{
			SendPOPCommand( io, Commands::CommandWithTwoStrings( Commands::Apop, username, digest ) );
			
			std::string response = GetResponse( input );
			return TestResponse( response );
		}
		
		static StatResult ParseStatResponse( const std::string& response )
		{
			typedef std::string::size_type Pos;
			
			Pos msgCountPos = std::strlen( "+OK " );  // Skip a bit, brother
			Pos msgCountEnd = response.find(' ', msgCountPos);
			
			if ( msgCountEnd == std::string::npos )
			{
				throw InvalidResponse();
			}
			
			int msgCount = std::atoi( response.substr( msgCountPos, msgCountEnd ).c_str() );
			Pos byteCountPos = msgCountEnd + 1;
			Pos byteCountEnd = response.find( ' ', byteCountPos );
			int byteCount = std::atoi( response.substr( byteCountPos, byteCountEnd ).c_str() );
			
			return StatResult( msgCount, byteCount );
		}
		
		StatResult Session::Stat()
		{
			SendPOPCommand( io, Commands::CommandOnly( Commands::Stat ) );
			
			std::string response = GetResponse( input );
			
			if ( TestResponse( response ) )
			{
				return ParseStatResponse( response );
			}
			
			throw NegativeResponse();
		}
		
		static int ParseListResponse( const std::string& response )
		{
			return ParseStatResponse( response ).bytes;
		}
		
		int Session::List( int msgNumber )
		{
			SendPOPCommand( io, Commands::CommandWithInteger( Commands::List, msgNumber ) );
			
			std::string response = GetResponse( input );
			
			if ( TestResponse( response ) )
			{
				int msgBytes = ParseListResponse( response );
				return msgBytes;
			}
			
			throw NegativeResponse();
		}
		
		void Session::Top( int msgNumber, int numLines, std::vector< std::string >& outLines )
		{
			SendPOPCommand( io, Commands::CommandWithTwoIntegers( Commands::Top, msgNumber, numLines ) );
			
			ReceiveResponseMultiLine( outLines );
		}
		
		void Session::Retrieve( int msgNumber, std::vector< std::string >& outLines )
		{
			SendPOPCommand( io, Commands::CommandWithInteger( Commands::Retrieve, msgNumber ) );
			
			ReceiveResponseMultiLine( outLines );
		}
		
		void Session::Delete( int msgNumber )
		{
			SendPOPCommand( io, Commands::CommandWithInteger( Commands::Delete, msgNumber ) );
			
			ReceiveResponse();
		}
		
		void Session::Quit()
		{
			SendPOPCommand( io, Commands::CommandOnly( Commands::Quit ) );
			
			ReceiveResponse();
		}
		
	}  // namespace Client
	
}  // namespace POP3

