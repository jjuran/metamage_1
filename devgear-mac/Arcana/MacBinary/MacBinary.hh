/*	============
 *	MacBinary.hh
 *	============
 */

#ifndef MACBINARY_HH
#define MACBINARY_HH

// Standard C++
#include <list>

// Nitrogen Carbon support
#include "Nitrogen/Files.h"


namespace MacBinary
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	static const std::size_t kMacBinaryBlockSize    = 128;
	static const std::size_t kMacBinaryHeaderLength = 128;
	
	class InvalidMacBinaryHeader      {};  // The stream lacks a valid MacBinary header.
	class IncompatibleMacBinaryHeader {};  // The header says we're too old to decode it.
	
	// High-level interface
	
	bool Check( const FSSpec& file );
	
	void Encode( const FSSpec& source, const FSSpec& dest );
	void Decode( const FSSpec& source, const N::FSDirSpec& destDir );
	
	// Low-level interface
	
	struct Block
	{
		char data[ kMacBinaryBlockSize ];
	};
	
	class Encoder
	{
		private:
			std::list< Block > fBlocks;
			
			std::string fComment;
			
			NN::Owned< N::FSFileRefNum > fDataFork;
			NN::Owned< N::FSFileRefNum > fResourceFork;
		
		public:
			Encoder( const FSSpec& file );
			
			int Read( char* data, std::size_t byteCount );
	};
	
	class Decoder
	{
		private:
			N::FSDirSpec  fDestDir;
			FSSpec        fFile;
			
			bool fHeaderReceived;
			
			ByteCount fDataForkLength;
			ByteCount fResourceForkLength;
			UInt16    fSecondaryHeaderLength;
			UInt16    fCommentLength;
			
			UInt32 fModificationDate;
			
			NN::Owned< N::FSFileRefNum > fDataFork;
			NN::Owned< N::FSFileRefNum > fResourceFork;
			
			std::string fComment;
		
		public:
			Decoder( const N::FSDirSpec& destination );
			
			void DecodeHeader( const char* header );
			
			int Write( const char* data, ByteCount byteCount );
	};
	
}

#endif

