/*
	Handle.hh
	---------
*/

#ifndef GENIE_IO_HANDLE_HH
#define GENIE_IO_HANDLE_HH

// nucleus
#include "nucleus/shared.hh"

// Nitrogen
#include "Mac/Memory/Types/Handle.hh"

// Genie
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	class Handle_IOHandle : public VirtualFileHandle< RegularFileHandle >
	{
		private:
			nucleus::shared< Mac::Handle > itsHandle;
		
		protected:
			const nucleus::shared< Mac::Handle >& GetHandle() const  { return itsHandle; }
		
		public:
			Handle_IOHandle( const FSTreePtr&                       file,
			                 OpenFlags                              flags,
			                 const nucleus::shared< Mac::Handle >&  h )
			:
				VirtualFileHandle< RegularFileHandle >( file, flags ),
				itsHandle( h )
			{
			}
			
			~Handle_IOHandle();
			
			boost::shared_ptr< IOHandle > Clone();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF();
			
			void SetEOF( off_t length );
	};
	
}

#endif

