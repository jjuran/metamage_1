/*
	stream.hh
	---------
*/

#ifndef VFS_STREAM_HH
#define VFS_STREAM_HH

// plus
#include "plus/conduit.hh"

// vfs
#include "vfs/filehandle_ptr.hh"


namespace vfs
{
	
	class stream : public plus::conduit
	{
		private:
			typedef void (*try_again_f)( bool );
			
			std::list< filehandle_ptr > its_fds;
			
			// non-copyable
			stream           ( const stream& );
			stream& operator=( const stream& );
		
		public:
			stream()
			{
			}
			
			bool empty() const  { return its_fds.empty(); }
			
			void send_fd( filehandle& fh );
			
			filehandle_ptr recv_fd( bool nonblocking, try_again_f try_again );
	};
	
}

#endif

