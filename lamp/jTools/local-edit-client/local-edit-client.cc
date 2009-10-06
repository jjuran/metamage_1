/*	====================
 *	local-edit-client.cc
 *	====================
 */

// Standard C++
#include <string>

// POSIX
#include <fcntl.h>
#include <sys/socket.h>

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/Pathnames.hh"
#include "poseven/extras/pump.hh"
#include "poseven/functions/ftruncate.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/perror.hh"
#include "poseven/functions/pwrite.hh"
#include "poseven/functions/shutdown.hh"
#include "poseven/functions/write.hh"

// Arcana
#include "HTTP.hh"
#include "MD5.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace o = orion;
	
	
	static MD5::Result MD5DigestFile( p7::fd_t input )
	{
		MD5::Engine md5;
		
		const std::size_t blockSize = 4096;
		
		char buffer[ blockSize ];
		
		while ( std::size_t bytes_read = p7::read( input, buffer, blockSize ) )
		{
			if ( bytes_read == blockSize )
			{
				for ( const char* p = buffer;  p < buffer + blockSize;  p += 64 )
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
				
				return md5.GetResult();
			}
		}
		
		md5.Finish( buffer, 0 );
		
		return md5.GetResult();
	}
	
	static void EncodeBase64Triplet( const unsigned char* triplet, unsigned char* buffer )
	{
		const char* code = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		
		const unsigned char mask = 0x3f;
		
		buffer[0] = code[                            (triplet[0] >> 2) ];
		buffer[1] = code[ mask & (triplet[0] << 4) | (triplet[1] >> 4) ];
		buffer[2] = code[ mask & (triplet[1] << 2) | (triplet[2] >> 6) ];
		buffer[3] = code[ mask & (triplet[2] << 0)                     ];
	}
	
	static std::string EncodeBase64( const unsigned char* begin, const unsigned char* end )
	{
		std::string result;
		
		result.reserve( (end - begin + 2) * 4 / 3 );
		
		unsigned char buffer[4];
		
		while ( begin < end - 2 )
		{
			EncodeBase64Triplet( begin, buffer );
			
			result.append( (char*) buffer, 4 );
			
			begin += 3;
		}
		
		if ( begin < end )
		{
			unsigned char final[3] = { 0, 0, 0 };
			
			std::copy( begin, end, final );
			
			EncodeBase64Triplet( final, buffer );
			
			if ( end - begin == 1 )
			{
				buffer[2] = '=';
			}
			
			buffer[3] = '=';
			
			result.append( (char*) buffer, 4 );
		}
		
		return result;
	}
	
	static void CopyFileContents( p7::fd_t in, p7::fd_t out )
	{
		// Truncate the destinaton so we don't get leftover garbage
		p7::ftruncate( out, 0 );
		
		// Start at the beginning of each file
		off_t from_offset = 0;
		off_t to_offset   = 0;
		
		// Read/write until EOF
		p7::pump( in, &from_offset, out, &to_offset );
	}
	
	static void CommitFileEdits( p7::fd_t  edited_file_stream,
	                             p7::fd_t  target_file_stream )
	{
		CopyFileContents( edited_file_stream, target_file_stream );
	}
	
	static void CommitFileEditsWithBackup( p7::fd_t  edited_file_stream,
	                                       p7::fd_t  target_file_stream,
	                                       p7::fd_t  backup_file_stream )
	{
		if ( backup_file_stream != -1 )
		{
			CopyFileContents( target_file_stream, backup_file_stream );
		}
		
		CommitFileEdits( edited_file_stream, target_file_stream );
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		bool dumpHeader = false;
		
		char const *const defaultOutput = "/dev/fd/1";
		
		const char* outputFile = defaultOutput;
		
		o::bind_option_to_variable( "-i", dumpHeader );
		o::bind_option_to_variable( "-o", outputFile  );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		const size_t n_args = o::free_argument_count();
		
		const char* target_pathname = n_args > 0 ? free_args[0] : "/dev/null";
		
		NN::Owned< p7::fd_t > target_file_stream = p7::open( target_pathname, p7::o_rdwr | p7::o_creat );
		
		MD5::Result digest = MD5DigestFile( target_file_stream );
		
		std::string old_digest_b64 = EncodeBase64( digest.data, digest.data + 16 );
		
		const p7::fd_t socket_in  = p7::fd_t( 6 );
		const p7::fd_t socket_out = p7::fd_t( 7 );
		
		const char* method = "APPLY";
		
		const char* urlPath = "/cgi-bin/local-edit-server";
		
		std::string contentLengthHeader;
		
		try
		{
			contentLengthHeader = HTTP::GetContentLengthLine( target_file_stream );
		}
		catch ( ... )
		{
		}
		
		std::string message_header =   HTTP::RequestLine( method, urlPath )
		                             //+ HTTP::HeaderLine( "Host", hostname )
		                             + HTTP::HeaderFieldLine( "X-Edit-Title", io::get_filename( target_pathname ) )
		                             + HTTP::HeaderFieldLine( "Content-MD5", old_digest_b64 )
		                             + contentLengthHeader
		                             + "\r\n";
		
		p7::write( socket_out, message_header );
		
		off_t offset = 0;
		
		p7::pump( target_file_stream, &offset, socket_out );
		
		p7::shutdown( socket_out, p7::shut_wr );
		
		HTTP::ResponseReceiver response;
		
		response.ReceiveHeader( socket_in );
		
		if ( dumpHeader )
		{
			p7::write( p7::stdout_fileno, response.GetMessageStream() );
		}
		
		unsigned result_code = response.GetResultCode();
		
		if ( result_code == 200 )
		{
			NN::Owned< p7::fd_t > edited_file_stream = p7::open( outputFile, p7::o_rdwr | p7::o_trunc | p7::o_creat, p7::_400 );
			
			const std::string& partial_content = response.GetPartialContent();
			
			p7::pwrite( edited_file_stream, partial_content, 0 );
			
			offset = partial_content.length();
			
			p7::pump( socket_in, edited_file_stream, &offset );
			
			digest = MD5DigestFile( edited_file_stream );
			
			std::string new_digest_b64 = EncodeBase64( digest.data, digest.data + 16 );
			
			std::string received_digest_b64 = response.GetHeaderField( "Content-MD5", "" );
			
			if ( new_digest_b64 != received_digest_b64 )
			{
				p7::write( p7::stderr_fileno, STR_LEN( "MD5 digest mismatch\n" ) );
				
				unlink( outputFile );
				
				return EXIT_FAILURE;
			}
			
			// FIXME:  test for a regular file
			if ( outputFile != defaultOutput )
			{
				p7::write( p7::stdout_fileno, STR_LEN( "Hit return to confirm or Control-D to cancel: " ) );
				
				while ( true )
				{
					char c;
					
					int bytes_read = read( p7::stdin_fileno, &c, sizeof c );
					
					if ( bytes_read == -1 )
					{
						p7::perror( "local-edit-client: read" );
						
						// I'm not sure what the scenario is here.
						// (EINTR on handled signal?  EIO on disconnected terminal?)
						// Leave tmp file for recovery.
						return EXIT_FAILURE;
					}
					
					if ( bytes_read == 0 )
					{
						p7::write( p7::stdout_fileno, STR_LEN( "\n" "canceled\n" ) );
						
						break;
					}
					
					if ( c == '\n' )
					{
						CommitFileEdits( edited_file_stream, target_file_stream );
						
						break;
					}
				}
				
				p7::close( edited_file_stream );
				
				unlink( outputFile );
			}
			
		}
		else if ( result_code == 304 )
		{
			// Not modified
		}
		else
		{
			p7::write( p7::stderr_fileno, response.GetResult() + "\n" );
			
			return EXIT_FAILURE;
		}
		
		p7::shutdown( socket_in, p7::shut_rd );
		
		return 0;
	}

}

