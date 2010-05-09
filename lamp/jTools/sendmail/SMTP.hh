/*	=======
 *	SMTP.hh
 *	=======
 */

#ifndef ARCANA_SMTP_HH
#define ARCANA_SMTP_HH

// Standard C++
#include <vector>

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/functions/write.hh"


namespace SMTP
{
	
	namespace Client
	{
		
		namespace Commands
		{
			
			inline plus::string Hello( const plus::string& hostname )
			{
				return "HELO " + hostname;
			}
			
			inline plus::string MailFrom( const plus::string& from )
			{
				return "MAIL FROM:" + from;
			}
			
			inline plus::string RecipientTo( const plus::string& to )
			{
				return "RCPT TO:" + to;
			}
			
			inline plus::string BeginData()
			{
				return "DATA";
			}
			
			inline plus::string Quit()
			{
				return "QUIT";
			}
			
		}
		
		struct Exception  {};
		struct InvalidResponse : Exception  {};
		struct Error           : Exception  {};
		struct Failure         : Exception  {};
		
		
		inline void SendCommand( poseven::fd_t stream, const plus::string& command )
		{
			poseven::write( stream, command + "\r\n" );
		}
		
		class Session
		{
			private:
				text_input::feed    its_feed;
				poseven::fd_reader  its_input_reader;
				poseven::fd_t       its_connection;
				
				void verify_response();
			
			public:
				Session( poseven::fd_t fd )
				:
					its_connection  ( fd ),
					its_input_reader( fd )
				{
					verify_response();
				}
				
				~Session()  {}
				
				void DoCommand( const plus::string& command )
				{
					SendCommand( its_connection, command );
					
					verify_response();
				}
				
				void Hello( const plus::string& hostname )
				{
					DoCommand( Commands::Hello( hostname ) );
				}
				
				void MailFrom( const plus::string& from )
				{
					DoCommand( Commands::MailFrom( from ) );
				}
				
				void RecipientTo( const plus::string& to )
				{
					DoCommand( Commands::RecipientTo( to ) );
				}
				
				void BeginData()
				{
					DoCommand( Commands::BeginData() );
				}
				
				void EndData()
				{
					verify_response();
				}
				
				void Quit()
				{
					DoCommand( Commands::Quit() );
				}
		};
		
	}  // namespace Client
	
}  // namespace SMTP

#endif

