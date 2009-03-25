/*	=======
 *	POP3.hh
 *	=======
 */

#pragma once

#include <string>
#include <vector>

// Io
#include "Io/TextInput.hh"


namespace POP3
{
	
	namespace N = Nitrogen;
	
	/*
	enum ProtocolState {
		stateNone, 
		stateAuthorization, 
		stateAuthorizationUserOK, 
		stateTransaction
	};
	*/
	
	namespace Client
	{
		
		struct InvalidResponse {};
		struct NegativeResponse {};
		struct MessageTooLong {};
		
		struct StatResult
		{
			int messages;
			int bytes;
			
			StatResult( int msgCount, int byteCount )
			:
				messages( msgCount  ),
				bytes   ( byteCount )
			{}
		};
		
		bool CheckResponse( const std::string& response );
		
		template < class Stream >
		std::string GetResponse( Io::TextInputAdapter< Stream >& input )
		{
			std::string response = input.Read();
			
			//DebugReceived( response );
			
			CheckResponse( response );
			
			return response;
		}
		
		template < class Stream >
		class Session
		{
			private:
				Io::TextInputAdapter< Stream >  itsInput;
			
			public:
				Session( Io::Handle socket );
				~Session() {}
				
				void ReceiveResponse();
				void ReceiveResponseMultiLine( std::vector< std::string >& outLines );
				
				void ReadMultiLineData( std::vector< std::string >& outLines );
				
				bool User( const std::string& username );
				bool Password( const std::string& password );
				bool Apop(const std::string& username, const std::string& digest);
				StatResult Stat();
				int List(int msgNumber);
				void Top(int msgNumber, int numLines, std::vector< std::string >& outLines);
				void Retrieve(int msgNumber, std::vector< std::string >& outLines);
				void Delete(int msgNumber);
				void Quit();
		};
		
	}  // namespace Client
	
}  // namespace POP3

