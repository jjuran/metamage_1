/*
	MacBinary.hh
	------------
*/

#ifndef MACBINARY_HH
#define MACBINARY_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C++
#include <vector>

// mac-types
#include "mac_types/VRefNum_DirID.hh"

// mac-file-utils
#include "mac_file/FSIORefNum.hh"

// plus
#include "plus/var_string.hh"


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
			
			FSIORefNum itsDataFork;
			FSIORefNum itsResourceFork;
			
			void DecodeHeader( const char* header );
			
			void CloseFiles();
			
			// non-copyable
			Decoder           ( const Decoder& );
			Decoder& operator=( const Decoder& );
			
		public:
			Decoder( const VRefNum_DirID& destination );
			
			~Decoder()  { CloseFiles(); }
			
			int Write( const char* data, ByteCount byteCount );
	};
	
}

#endif
