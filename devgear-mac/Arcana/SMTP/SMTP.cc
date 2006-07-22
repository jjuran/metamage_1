/*	=======
 *	SMTP.cc
 *	=======
 */

#include "SMTP.hh"

#include <algorithm>
#include <string>


namespace SMTP
{
	
	namespace Client
	{
		
		ResponseCode::ResponseCode( const std::string& response )
		{
			if ( response.size() < size )
			{
				throw InvalidResponse();
			}
			
			std::copy( response.begin(), response.begin() + size, chars );
		}
		
		ResponseCode GetResponse( Io::TextInputAdapter& input )
		{
			while ( true )
			{
				std::string response = input.Read();
				ResponseCode code = response;
				
				if ( response[ 3 ] == ' ' )
				{
					return code;
				}
				else if ( response[ 3 ] == '-' )
				{
					continue;
				}
				else
				{
					throw InvalidResponse();
				}
			}
		}
		
		ResponseCode VerifySuccess( ResponseCode code )
		{
			if ( code.chars[ 0 ] == '4' )
			{
				throw Error();
			}
			
			if ( code.chars[ 0 ] == '5' )
			{
				throw Failure();
			}
			
			return code;
		}
		
		
		Session::Session( Io::Handle socket )
		:
			io   ( socket ),
			input( socket )
		{
			VerifySuccess( GetResponse( input ) );
		}
		
	}  // namespace Client
	
}  // namespace SMTP

