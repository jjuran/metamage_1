/*
	MacBinary.hh
	------------
*/

#ifndef MACBINARY_HH
#define MACBINARY_HH

// Standard C++
#include <vector>

// nucleus
#include "nucleus/owned.hh"
#include "nucleus/string.hh"

// Nitrogen
#include "Mac/Files/Types/FSDirSpec.hh"
#include "Mac/Files/Types/FSFileRefNum.hh"


namespace MacBinary
{
	
	typedef void (*BlockWriter)( int, const void*, std::size_t );
	
	void Encode( const FSSpec& file, BlockWriter blockWrite, int output );
	
	static const std::size_t kMacBinaryBlockSize    = 128;
	static const std::size_t kMacBinaryHeaderLength = 128;
	
	class InvalidMacBinaryHeader      {};  // The stream lacks a valid MacBinary header.
	class IncompatibleMacBinaryHeader {};  // The header says we're too old to decode it.
	class MacBinaryIIPlusNotSupported {};  // The file has a valid MacBinary II+ header
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
				Mac::FSDirSpec           destDir;
				FSSpec                   file;
				UInt32                   modificationDate;
				nucleus::mutable_string  comment;
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
			Decoder( const Mac::FSDirSpec& destination );
			
			int Write( const char* data, ByteCount byteCount );
	};
	
}

#endif

