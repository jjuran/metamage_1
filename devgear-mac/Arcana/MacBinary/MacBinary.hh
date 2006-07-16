/*	============
 *	MacBinary.hh
 *	============
 */

#ifndef MACBINARY_HH
#define MACBINARY_HH

// Standard C++
#include <list>
#include <vector>

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
	class MacBinaryIIPlusNotSupported {};  // The file has a valid MacBinary II+ header
	class TooManyEndBlocks            {};  // Directory end block received without start
	
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
			struct Frame
			{
				N::FSDirSpec  destDir;
				FSSpec        file;
				UInt32        modificationDate;
				std::string   comment;
				
				Frame()  {}
				
				Frame( N::FSDirSpec  destDir,
				       FSSpec        file,
				       UInt32        modificationDate,
				       std::string   comment )
				: destDir( destDir ),
				  file( file ),
				  modificationDate( modificationDate ),
				  comment( comment )
				{
				}
			};
			
			Frame frame;
			std::vector< Frame > frameStack;
			
			bool isFolder;
			bool fHeaderReceived;
			bool trailerReceived;
			
			ByteCount fDataForkLength;
			ByteCount fResourceForkLength;
			UInt16    fSecondaryHeaderLength;
			UInt16    fCommentLength;
			
			NN::Owned< N::FSFileRefNum > fDataFork;
			NN::Owned< N::FSFileRefNum > fResourceFork;
			
		public:
			Decoder( const N::FSDirSpec& destination );
			
			void DecodeHeader( const char* header );
			
			int Write( const char* data, ByteCount byteCount );
	};
	
}

#endif

