/*	===============
 *	SimpleDevice.hh
 *	===============
 */

#ifndef GENIE_IO_SIMPLEDEVICE_HH
#define GENIE_IO_SIMPLEDEVICE_HH

// POSIX
#include <fcntl.h>

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"

 // Genie
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	typedef ssize_t (*Reader)( char      *, std::size_t );
	typedef ssize_t (*Writer)( char const*, std::size_t );
	
	struct DeviceIOSpec
	{
		const char* name;
		Reader reader;
		Writer writer;
	};
	
	class SimpleDeviceHandle : public StreamHandle
	{
		private:
			const DeviceIOSpec& io;
		
		public:
			SimpleDeviceHandle( const DeviceIOSpec& io ) : StreamHandle( O_RDWR ), io( io )
			{
			}
			
			FSTreePtr GetFile();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
			
			memory_mapping_ptr Map( size_t length, int prot, int flags, off_t offset );
	};
	
	vfs::filehandle_ptr GetSimpleDeviceHandle( const vfs::node& file );
	
}

#endif

