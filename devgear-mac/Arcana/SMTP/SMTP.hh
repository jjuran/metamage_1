/*	=======
 *	SMTP.hh
 *	=======
 */

#pragma once

// Standard C++
#include <string>
#include <vector>

// Io
#include "Io/Handle.hh"
#include "Io/TextInput.hh"


namespace SMTP
{
	
	namespace Client
	{
		
		namespace Commands
		{
			
			inline std::string Hello( const std::string& hostname )
			{
				return std::string( "HELO " ) + hostname;
			}
			
			inline std::string MailFrom( const std::string& from )
			{
				return std::string( "MAIL FROM:" ) + from;
			}
			
			inline std::string RecipientTo( const std::string& to )
			{
				return std::string( "RCPT TO:" ) + to;
			}
			
			inline std::string BeginData()
			{
				return "DATA";
			}
			
			inline std::string Quit()
			{
				return "QUIT";
			}
			
		}
		
		struct Exception  {};
		struct InvalidResponse : Exception  {};
		struct Error           : Exception  {};
		struct Failure         : Exception  {};
		
		
		struct ResponseCode
		{
			enum { size = 4 };
			
			ResponseCode( const std::string& response );
			
			char chars[ size ];
		};
		
		ResponseCode GetResponse( Io::TextInputAdapter< Io::IOHandle >& input );
		
		ResponseCode VerifySuccess( ResponseCode code );
		
		inline void SendCommand( Io::IOHandle io, const std::string& command )
		{
			io.Write( command.data(), command.size() );
			io.Write( "\r\n", 2 );  // FIXME
		}
		
		class Session
		{
			private:
				Io::IOHandle io;
				Io::TextInputAdapter< Io::IOHandle > input;
			
			public:
				Session( Io::Handle socket );
				~Session()  {}
				
				void DoCommand( const std::string& command )
				{
					SendCommand( io, command );
					VerifySuccess( GetResponse( input ) );
				}
				
				void Hello( const std::string& hostname )
				{
					DoCommand( Commands::Hello( hostname ) );
				}
				
				void MailFrom( const std::string& from )
				{
					DoCommand( Commands::MailFrom( from ) );
				}
				
				void RecipientTo( const std::string& to )
				{
					DoCommand( Commands::RecipientTo( to ) );
				}
				
				void BeginData()
				{
					DoCommand( Commands::BeginData() );
				}
				
				void WriteRaw( const char* data, unsigned int byteCount );
				
				void EndData()
				{
					VerifySuccess( GetResponse( input ) );
				}
				
				void Quit()
				{
					DoCommand( Commands::Quit() );
				}
		};
		
	}  // namespace Client
	
}  // namespace SMTP

