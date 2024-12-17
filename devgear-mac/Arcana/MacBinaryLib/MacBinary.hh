/*
	MacBinary.hh
	------------
*/

#ifndef MACBINARY_HH
#define MACBINARY_HH

// Standard C++
#include <vector>

// mac-types
#include "mac_types/VRefNum_DirID.hh"

// mac-file-utils
#include "mac_file/FSIORefNum.hh"

// plus
#include "plus/var_string.hh"

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#include "Mac/Files/Types/FSFileRefNum.hh"
#endif


namespace MacBinary
{
	
	using mac::file::FSIORefNum;
	using mac::types::VRefNum_DirID;
	
	typedef void (*BlockWriter)( int, const void*, std::size_t );
	
	void Encode( const FSSpec& file, BlockWriter blockWrite, int output );
	
	static const std::size_t kMacBinaryBlockSize    = 128;
	static const std::size_t kMacBinaryHeaderLength = 128;
	
	class InvalidMacBinaryHeader      {};  // The stream lacks a valid MacBinary header.
	class IncompatibleMacBinaryHeader {};  // The header says we're too old to decode it.
	class TooManyEndBlocks            {};  // Directory end block received without start
	
	struct Block
	{
		char data[ kMacBinaryBlockSize ];
	};
	
	class Decoder
	{
		private:
			struct Frame
			{
				VRefNum_DirID     destDir;
				FSSpec            file;
				UInt32            modificationDate;
				plus::var_string  comment;
			};
			
			Frame itsFrame;
			std::vector< Frame > itsFrameStack;
			
			bool itIsFolder;
			bool itsHeaderWasReceived;
			bool itsTrailerWasReceived;
			
			ByteCount itsDataForkLength;
			ByteCount itsResourceForkLength;
			UInt16    itsSecondaryHeaderLength;
			UInt16    itsCommentLength;
			
			nucleus::owned< Mac::FSFileRefNum > itsDataFork;
			nucleus::owned< Mac::FSFileRefNum > itsResourceFork;
			
			void DecodeHeader( const char* header );
			
		public:
			Decoder( const VRefNum_DirID& destination );
			
			int Write( const char* data, ByteCount byteCount );
	};
	
}

#endif
