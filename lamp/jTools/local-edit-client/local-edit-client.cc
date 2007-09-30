/*	====================
 *	local-edit-client.cc
 *	====================
 */

// Standard C/C++
#include <cerrno>
#include <cstdlib>

// Standard C++
#include <string>

// POSIX
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

// POSeven
#include "POSeven/FileDescriptor.hh"
#include "POSeven/Open.hh"

// Arcana
#include "HTTP.hh"
#include "MD5.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace NN = Nucleus;
namespace p7 = poseven;
namespace O = Orion;


static MD5::Result MD5DigestFile( p7::fd_t input )
{
	MD5::Engine md5;
	
	char buffer[ 4096 ];
	
	try
	{
		while ( std::size_t bytes_read = p7::read( input, buffer, 4096 ) )
		{
			if ( bytes_read == 4096 )
			{
				for ( const char* p = buffer;  p < buffer + 4096;  p += 64 )
				{
					md5.DoBlock( p );
				}
			}
			else
			{
				const char* end_of_blocks = buffer + bytes_read - bytes_read % 64;
				
				for ( const char* p = buffer;  p < end_of_blocks;  p += 64 )
				{
					md5.DoBlock( p );
				}
				
				md5.Finish( end_of_blocks, bytes_read % 64 * 8 );
			}
		}
	}
	catch ( const io::end_of_input& )
	{
	}
	
	return md5.GetResult();
}

static void EncodeBase64Triplet( unsigned triplet, unsigned char buffer[4] )
{
	const char* code = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	
	buffer[0] = code[ (triplet >> 18)            ];
	buffer[1] = code[ (triplet >> 12) & 0x00003f ];
	buffer[2] = code[ (triplet >>  6) & 0x00003f ];
	buffer[3] = code[ (triplet >>  0) & 0x00003f ];
}

static std::string EncodeBase64( const unsigned char* begin, const unsigned char* end )
{
	std::string result;
	
	result.reserve( (end - begin + 2) * 4 / 3 );
	
	unsigned char buffer[4];
	
	while ( begin < end - 2 )
	{
		unsigned triplet = (begin[0] << 16 )
		                 | (begin[1] <<  8 )
		                 | (begin[2] <<  0 );
		
		EncodeBase64Triplet( triplet, buffer );
		
		result.append( (char*) buffer, 4 );
		
		begin += 3;
	}
	
	if ( begin < end )
	{
		unsigned triplet = begin[0] << 16;
		
		if ( end - begin == 2 )
		{
			triplet |= begin[1] << 8;
		}
		
		EncodeBase64Triplet( triplet, buffer );
		
		if ( end - begin == 1 )
		{
			buffer[2] = '=';
		}
		
		buffer[3] = '=';
		
		result.append( (char*) buffer, 4 );
	}
	
	return result;
}

int O::Main( int argc, argv_t argv )
{
	bool dumpHeaders = false;
	
	const char* defaultOutput = "/dev/fd/1";
	
	const char* outputFile = defaultOutput;
	
	O::BindOption( "-i", dumpHeaders );
	O::BindOption( "-o", outputFile  );
	
	O::GetOptions( argc, argv );
	
	char const *const *freeArgs = O::FreeArguments();
	
	std::size_t argCount = O::FreeArgumentCount();
	
	const char* filename = argCount > 0 ? freeArgs[0] : "/dev/null";
	
	NN::Owned< p7::fd_t > message_body = p7::open( filename, O_RDONLY );
	
	MD5::Result digest = MD5DigestFile( message_body );
	
	std::string old_digest_b64 = EncodeBase64( digest.data, digest.data + 16 );
	
	//p7::lseek( message_body, 0, 0 );
	lseek( message_body, 0, 0 );
	
	const p7::fd_t socket_in  = p7::fd_t( 6 );
	const p7::fd_t socket_out = p7::fd_t( 7 );
	
	const char* method = "APPLY";
	
	const char* urlPath = "/cgi-bin/local-edit-server";
	
	std::string contentLengthHeader;
	
	try
	{
		contentLengthHeader = HTTP::GetContentLengthHeaderLine( message_body );
	}
	catch ( ... )
	{
	}
	
	std::string message_header =   HTTP::RequestLine( method, urlPath )
	                             //+ HTTP::HeaderLine( "Host", hostname )
	                             + HTTP::HeaderLine( "Content-MD5", old_digest_b64 )
	                             + contentLengthHeader
	                             + "\r\n";
	
	HTTP::SendMessage( socket_out, message_header, message_body );
	
	shutdown( socket_out, SHUT_WR );
	
	HTTP::ResponseReceiver response;
	
	response.ReceiveHeaders( socket_in );
	
	if ( dumpHeaders )
	{
		const std::string& message = response.GetMessageStream();
		
		p7::write( p7::stdout_fileno, message.data(), message.size() );
	}
	
	unsigned result_code = response.GetResultCode();
	
	if ( result_code == 200 )
	{
		NN::Owned< p7::fd_t > output = p7::open( outputFile, O_WRONLY | O_TRUNC | O_CREAT );
		
		const std::string& partial_content = response.GetPartialContent();
		
		if ( !partial_content.empty() )
		{
			p7::write( output, partial_content.data(), partial_content.size() );
		}
		
		HTTP::SendMessageBody( output, socket_in );
		
		output = p7::open( outputFile, O_RDONLY );
		
		digest = MD5DigestFile( output );
		
		std::string new_digest_b64 = EncodeBase64( digest.data, digest.data + 16 );
		
		std::string received_digest_b64 = response.GetHeader( "Content-MD5", "" );
		
		if ( new_digest_b64 != received_digest_b64 )
		{
			std::fprintf( stderr, "MD5 digest mismatch\n" );
			
			unlink( outputFile );
			
			return EXIT_FAILURE;
		}
	}
	else if ( result_code == 304 )
	{
		// Not modified
	}
	else
	{
		std::fprintf( stderr, "%s\n", response.GetResult().c_str() );
		
		return EXIT_FAILURE;
	}
	
	shutdown( socket_in, SHUT_RD );
	
	return 0;
}

