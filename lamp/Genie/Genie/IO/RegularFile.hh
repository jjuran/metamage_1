/*	==============
 *	RegularFile.hh
 *	==============
 */

#ifndef GENIE_IO_REGULARFILE_HH
#define GENIE_IO_REGULARFILE_HH

 // Genie
 #include "Genie/IO/Stream.hh"


namespace Genie
{
	
	class RegularFileHandle : public StreamHandle
	{
		public:
			RegularFileHandle( int                                flags,
			                   const vfs::filehandle_method_set*  methods = NULL );
			
			RegularFileHandle( const vfs::node_ptr&               file,
			                   int                                flags,
			                   const vfs::filehandle_method_set*  methods = NULL );
			
			virtual ~RegularFileHandle();
			
			virtual IOPtr Clone() = 0;
			
			virtual unsigned int SysPoll()  { return kPollRead | kPollWrite; }
			
			virtual ssize_t Append( const char* buffer, size_t n_bytes );
			
			ssize_t SysRead( char* buffer, size_t n_bytes );
			
			ssize_t SysWrite( const char* buffer, size_t n_bytes );
			
			ssize_t Write( const char* buffer, std::size_t byteCount );
			
			memory_mapping_ptr Map( size_t length, int prot, int flags, off_t offset );
	};
	
}

#endif

