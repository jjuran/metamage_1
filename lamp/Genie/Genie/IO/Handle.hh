/*
	Handle.hh
	---------
*/

#ifndef GENIE_IO_HANDLE_HH
#define GENIE_IO_HANDLE_HH

// nucleus
#ifndef NUCLEUS_SHARED_HH
#include "nucleus/shared.hh"
#endif

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Handle.hh"
#endif

// vfs
#include "vfs/filehandle.hh"


namespace Genie
{
	
	class Handle_IOHandle : public vfs::filehandle
	{
		private:
			nucleus::shared< Mac::Handle > itsHandle;
		
		public:
			Handle_IOHandle( const vfs::node&                       file,
			                 int                                    flags,
			                 const nucleus::shared< Mac::Handle >&  h );
			
			~Handle_IOHandle();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF();
			
			void SetEOF( off_t length );
	};
	
}

#endif

