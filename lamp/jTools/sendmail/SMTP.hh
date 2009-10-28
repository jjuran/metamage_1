/*	=======
 *	SMTP.hh
 *	=======
 */

#ifndef ARCANA_SMTP_HH
#define ARCANA_SMTP_HH

// Standard C++
#include <string>
#include <vector>

// poseven
#include "poseven/FileDescriptor.hh"
#include "poseven/functions/write.hh"

// Io
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
		
		bool CheckResponse( const std::string& response );
		
		ResponseCode GetResponse( Io::TextInputAdapter< poseven::fd_t >& input );
		
		
		ResponseCode VerifySuccess( ResponseCode code );
		
		inline void SendCommand( poseven::fd_t stream, const std::string& command )
		{
			poseven::write( stream, command + "\r\n" );
		}
		
		class Session
		{
			private:
				Io::TextInputAdapter< poseven::fd_t >  itsInput;
			
			public:
				Session( poseven::fd_t stream ) : itsInput( stream )
				{
					VerifySuccess( GetResponse( itsInput ) );
				}
				
				~Session()  {}
				
				void DoCommand( const std::string& command )
				{
					SendCommand( itsInput.GetStream(), command );
					VerifySuccess( GetResponse( itsInput ) );
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
				
				void EndData()
				{
					VerifySuccess( GetResponse( itsInput ) );
				}
				
				void Quit()
				{
					DoCommand( Commands::Quit() );
				}
		};
		
	}  // namespace Client
	
}  // namespace SMTP

#endif

