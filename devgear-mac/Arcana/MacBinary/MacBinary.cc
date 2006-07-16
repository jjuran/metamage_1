/*	============
 *	MacBinary.cc
 *	============
 */

// MacBinary III has a purported "signature" of 'mBIN'.
// According to <http://www.lazerware.com/formats/macbinary/macbinary_iii.html>,
// a MacBinary III decoder is supposed to conclude, based only on the presence
// of this signature (which is at byte offset 102, not early enough to be
// considered a magic number, and which consists entirely of printable
// characters), that a file is a MacBinary III envelope.
// This is broken -- according to that test, THIS file is valid MacBinary III!

// Consequently, I'm operating under the assumption that MacBinary III files
// will continue to be valid MacBinary II files, and applying the MacBinary II
// validity tests even in the presence of the MacBinary III signature.
// This will only break under the following condition:
// A future version of MacBinary (say, MacBinary 4) creates files that are valid
// MacBinary III but invalid MacBinary II, which necessitates failing the
// MacBinary II validity tests.  However, given the forward compatibility means
// built into MacBinary II, this is *extremely* unlikely -- the secondary header
// field allows for considerable expansion, and in the event that's insufficient,
// the minimum version field is sufficient to ward off compliant decoders.
// Therefore, there is no technically sound reason to create files that are valid
// MacBinary III but invalid MacBinary II.  Concordantly, neither is there a
// technically sound reason *not* to apply the MacBinary II validity tests.

#include "MacBinary.hh"

// Standard C++
#include <algorithm>

// Nitrogen Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"


/*

Offset	Type	Description
------	----	-----------
000		Byte	Old version number; must be kept at zero for compatibility

001		Str63	Filename

065		OSType	File type code

069		OSType	File creator code

073		Byte	Original Finder flags 
				Bit 7 - isAlias
				Bit 6 - isInvisible
				Bit 5 - hasBundle
				Bit 4 - nameLocked
				Bit 3 - isStationery
				Bit 2 - hasCustomIcon
				Bit 1 - reserved
				Bit 0 - hasBeenInited

074		Byte	Zero fill

075		Point	Position of file's icon in window (vertical, then horizontal)

079		short	File's window or folder ID (reserved in Mac OS 8)

081		Byte	"Protected" flag (in low order bit)

082		Byte	Zero fill

083		UInt32	Length of data fork in bytes

087		UInt32	Length of resource fork in bytes

091		UInt32	Creation date

095		UInt32	Modification date.

099		UInt16	Length of Get Info comment

101		Byte	Finder Flags, bits 0-7. (Bits 8-15 are already in byte 73) 
				Bit 7 - hasNoInits 
				Bit 6 - isShared 
				Bit 5 - requiresSwitchLaunch 
				Bit 4 - ColorReserved 
				Bits 1-3 - color 
				Bit 0 - isOnDesk

102		OSType	MacBinary III signature ('mBIN')

106		Byte	script of file name (from the fdScript field of an fxInfo record)
107		Byte	extended Finder flags (from the fdXFlags field of an fxInfo record)

108		Byte[8]	Unused (must be zeroed by creators, must be ignored by readers)

116		UInt32	Total size of files prior to compression, zero for non-archives

120		UInt16	Length of a secondary header. If this is non-zero, skip this many bytes
                            (rounded up to the next multiple of 128). This is for future expansion
                            only, when sending files with MacBinary, this word should be zero.
122		Byte	Version number of MacBinary III that the uploading program is written
                            for (the version is 130 for MacBinary III)
123		Byte	Minimum MacBinary version needed to read this file (set this value at
                            129 for backwards compatibility with MacBinary II)

124		UInt16	CRC of previous 124 bytes

126		UInt16	reserved (zero)

*/


namespace Nitrogen
{
	
	#ifdef PBDTGetPath
	#undef PBDTGetPath
	#endif
	
	#ifdef PBDTGetComment
	#undef PBDTGetComment
	#endif
	
	#ifdef PBDTSetComment
	#undef PBDTSetComment
	#endif
	
	static void PBDTGetPath( FSVolumeRefNum vRefNum, DTPBRec& pb )
	{
		pb.ioVRefNum = vRefNum;
		pb.ioNamePtr = NULL;
		
		PBDTGetPath( pb );
	}
	
	static void PBDTGetComment( DTPBRec& pb )
	{
		ThrowOSStatus( ::PBDTGetCommentSync( &pb ) );
	}
	
	static void PBDTSetComment( DTPBRec& pb )
	{
		ThrowOSStatus( ::PBDTSetCommentSync( &pb ) );
	}
	
	static std::string DTGetComment( DTPBRec& pb )
	{
		const ByteCount kMaximumCommentLength = 200;
		const ByteCount kExperimentalCommentLength = 256;
		
		char comment[ kExperimentalCommentLength ];
		
		pb.ioDTBuffer = comment;
		pb.ioDTReqCount = kExperimentalCommentLength;
		
		ThrowOSStatus( ::PBDTGetCommentSync( &pb ) );
		
		ASSERT( pb.ioDTActCount >=   0 );
		ASSERT( pb.ioDTActCount <= 200 );
		
		return std::string( comment, pb.ioDTActCount );
	}
	
	static void DTSetComment( DTPBRec& pb, const std::string& comment )
	{
		pb.ioDTBuffer = const_cast< char* >( comment.data() );
		pb.ioDTReqCount = comment.size();
		
		PBDTSetComment( pb );
	}
	
	static DTPBRec& FSpDTGetPath( const FSSpec& file, DTPBRec& pb )
	{
		PBDTGetPath( file.vRefNum, pb );
		
		pb.ioNamePtr = const_cast< unsigned char* >( file.name );
		pb.ioDirID   = file.parID;
		
		return pb;
	}
	
	static std::string FSpDTGetComment( const FSSpec& file )
	{
		DTPBRec pb;
		
		return DTGetComment( FSpDTGetPath( file, pb ) );
	}
	
	static void FSpDTSetComment( const FSSpec& file, const std::string& comment )
	{
		DTPBRec pb;
		
		return DTSetComment( FSpDTGetPath( file, pb ), comment );
	}
	
}

namespace MacBinary
{
	
	namespace FS = FileSystem;
	
	enum
	{
		kVersionMacBinaryII  = 129,
		kVersionMacBinaryIII = 130,
		
		kCurrentMacBinaryVersion = kVersionMacBinaryIII
	};
	
	// High-level implementation
	
	void Encode( const FSSpec& source, const FSSpec& dest )
	{
		Encoder encoder( source );
		
		N::FSpCreate( dest, '????', 'BINA' );
		
		NN::Owned< N::FSFileRefNum > out = N::FSpOpenDF( dest, fsWrPerm );
		
		char data[ 4096 ];
		
		try
		{
			while ( true )
			{
				std::size_t bytes = encoder.Read( data, 4096 );
				
				FS::Write( out, data, bytes );
			}
		}
		catch ( FS::EndOfFile& ) {}
	}
	
	void Decode( const FSSpec& source, const N::FSDirSpec& destDir )
	{
		NN::Owned< N::FSFileRefNum > in = N::FSpOpenDF( source, fsRdPerm );
		
		Decoder decoder( destDir );
		
		char data[ 4096 ];
		
		try
		{
			while ( true )
			{
				std::size_t bytes = FS::Read( in, data, 4096 );
				
				decoder.Write( data, bytes );
			}
		}
		catch ( FS::EndOfFile& ) {}
	}
	
	// Low-level implementation
	
	static unsigned short CalcCRC( register const unsigned char *dataBuf, std::size_t size )
	{
		//#define CCITT_CRC_GEN	0x1021
		
		const unsigned long kMagicNumber = 0x1021;
		
		register const unsigned char* dataEnd = dataBuf + size;
		
		register unsigned long crc = 0;
		
		while ( dataBuf != dataEnd )
		{
			register unsigned long dataByte = *dataBuf++;
			dataByte <<= 8;
			
			register long i = 8;
			
			do
			{	
				register long bit = dataByte;
				dataByte += dataByte;
				bit ^= crc;
				crc += crc;
				
				if ( bit &= 0x8000 )
				{
					crc ^= kMagicNumber;
				}
			}
			while ( --i );
		}
		
		return crc;
	}
	
	enum HeaderField
	{
		kOldVersion,
		kFileName,
		kFileType,
		kFileCreator,
		kFinderFlags,
		kZeroByte74,
		kIconPosition,
		//kFileFolderID,  // reserved in Mac OS 8
		kFInfo,
		//kProtectedFlag,  // not used in sample code
		kZeroByte82,
		kDataForkLength,
		kResourceForkLength,
		kFileCreationDate,
		kFileModificationDate,
		kGetInfoCommentLength,
		kMacBinaryIIISignature,
		kExtendedFinderFlags,
		kTotalFileLength,
		kSecondaryHeaderLength,
		kCurrentVersion,
		kMinimumVersion,
		kCRC
	};
	
	template < HeaderField field > struct Field_Traits;
	
	struct Header
	{
		unsigned char data[ kMacBinaryHeaderLength ];
	
		//Header();
		
		template < HeaderField field >
		typename Field_Traits< field >::Value Get() const
		{
			return Field_Traits< field >::Get( *this );
		}
		
		template < HeaderField field >
		void Set( typename Field_Traits< field >::Value v )
		{
			Field_Traits< field >::Set( *this, v );
		}
		
		template < HeaderField field >
		void Set()
		{
			Field_Traits< field >::Set( *this );
		}
		
		template < HeaderField field >
		bool Check() const
		{
			return Field_Traits< field >::Check( *this );
		}
	};
	
	template < std::size_t offset >
	struct Zero_Field_Traits
	{
		typedef UInt8 Value;
		
		static bool Check( const Header& h )
		{
			return h.data[ offset ] == 0;
		}
	};
	
	template < class Type, std::size_t offset >
	struct POD_Field_Traits
	{
		typedef Type Value;
		
		static Value Get( const Header& h )
		{
			return reinterpret_cast< const Value& >( h.data[ offset ] );
		}
		
		static void Set( Header& h, Value v )
		{
			reinterpret_cast< Value& >( h.data[ offset ] ) = v;
		}
	};
	
	template < class Type, std::size_t offset >
	struct MisalignedPOD_Field_Traits
	{
		typedef Type Value;
		
		static Value Get( const Header& h )
		{
			Value result;
			
			std::copy( h.data + offset,
			           h.data + offset + sizeof (Value),
			           reinterpret_cast< unsigned char* >( &result ) );
			
			return result;
		}
		
		static void Set( Header& h, Value v )
		{
			const unsigned char* value = reinterpret_cast< const unsigned char* >( &v );
			
			std::copy( value,
			           value + sizeof (Value),
			           h.data + offset );
		}
	};
	
	template <> struct Field_Traits< kOldVersion > : Zero_Field_Traits< 0 > {};
	
	template <>
	struct Field_Traits< kFileName >
	{
		typedef ConstStr63Param Value;
		
		static const std::size_t offset = 1;
		
		static Value Get( const Header& h )  { return &h.data[ offset ]; }
		
		static void Set( Header& h, Value v )
		{
			std::copy( v,
			           v + 1 + v[ 0 ],
			           &h.data[ offset ] );
		}
	};
	
	template <> struct Field_Traits< kFileType    > : MisalignedPOD_Field_Traits< ::OSType, 65 > {};
	template <> struct Field_Traits< kFileCreator > : MisalignedPOD_Field_Traits< ::OSType, 69 > {};
	
	template <>
	struct Field_Traits< kFinderFlags >
	{
		typedef unsigned short Value;
		
		static const std::size_t highOffset = 73;
		static const std::size_t lowOffset = 101;
		
		typedef POD_Field_Traits< unsigned char, highOffset > High;
		typedef POD_Field_Traits< unsigned char, lowOffset  > Low;
		
		static Value Get( const Header& h )
		{
			return   High::Get( h ) << 8
			       | Low ::Get( h );
		}
		
		static void Set( Header& h, Value v )
		{
			High::Set( h, v >> 8 );
			Low ::Set( h, v & 0xFF );
		}
	};
	
	template <> struct Field_Traits< kZeroByte74 > : Zero_Field_Traits< 74 > {};
	
	template <> struct Field_Traits< kIconPosition > : MisalignedPOD_Field_Traits< Point, 75 > {};
	
	template <>
	struct Field_Traits< kFInfo >
	{
		typedef FInfo Value;
		
		static const std::size_t offset = 65;
		
		typedef MisalignedPOD_Field_Traits< FInfo, offset > POD;
		
		static Value Get( const Header& h )
		{
			FInfo result = POD::Get( h );
			
			result.fdFlags = Field_Traits< kFinderFlags >::Get( h );
			
			return result;
		}
		
		static void Set( Header& h, Value v )
		{
			Field_Traits< kFinderFlags >::Set( h, v.fdFlags );
			
			v.fdFlags &= 0xFF00;  // Zero out the low byte
			
			POD::Set( h, v );
		}
	};
	
	template <> struct Field_Traits< kZeroByte82 > : Zero_Field_Traits< 82 > {};
	
	template <> struct Field_Traits< kDataForkLength     > : MisalignedPOD_Field_Traits< UInt32, 83 > {};
	template <> struct Field_Traits< kResourceForkLength > : MisalignedPOD_Field_Traits< UInt32, 87 > {};
	
	template <> struct Field_Traits< kFileCreationDate     > : MisalignedPOD_Field_Traits< UInt32, 91 > {};
	template <> struct Field_Traits< kFileModificationDate > : MisalignedPOD_Field_Traits< UInt32, 95 > {};
	
	template <> struct Field_Traits< kGetInfoCommentLength > : MisalignedPOD_Field_Traits< UInt16, 99 > {};
	
	template <>
	struct Field_Traits< kMacBinaryIIISignature >
	{
		typedef MisalignedPOD_Field_Traits< ::OSType, 102 > Field;
		
		static const ::OSType signature = 'mBIN';
		
		static bool Check( const Header& h )  { return Field::Get( h ) == signature; }
		
		static void Set( Header& h )  { Field::Set( h, signature ); }
	};
	
	template <> struct Field_Traits< kExtendedFinderFlags > : POD_Field_Traits< UInt16, 106 > {};
	
	template <> struct Field_Traits< kTotalFileLength > : POD_Field_Traits< UInt32, 116 > {};
	
	template <> struct Field_Traits< kSecondaryHeaderLength > : POD_Field_Traits< UInt8, 116 > {};
	
	template <> struct Field_Traits< kCurrentVersion > : POD_Field_Traits< UInt8, 122 > {};
	template <> struct Field_Traits< kMinimumVersion > : POD_Field_Traits< UInt8, 123 > {};
	
	template <>
	struct Field_Traits< kCRC >
	{
		static const std::size_t offset     = 124;
		static const std::size_t dataLength = 124;
		
		typedef POD_Field_Traits< UInt16, offset > Field;
		
		static UInt16 CRC( const unsigned char* data )  { return CalcCRC( data, dataLength ); }
		
		static bool Check( const Header& h )  { return Field::Get( h ) == CRC( h.data ); }
		
		static void Set( Header& h )  { Field::Set( h, CRC( h.data ) ); }
	};
	
	
	/*
	Header::Header()
	{
		std::fill( data,
		           data + kMacBinaryHeaderLength,
		           '\0' );
	}
	*/
	
	static void ZeroHeader( Header& h )
	{
		std::fill( h.data,
		           h.data + kMacBinaryHeaderLength,
		           '\0' );
	}
	
	// 0 < k < blockSize
	// 
	// 0             -> 0
	// k             -> blockSize
	// blockSize     -> blockSize
	// blockSize + k -> blockSize * 2
	// etc.
	static std::size_t PaddedLength( std::size_t length, std::size_t blockSize )
	{
		return length + blockSize - ( (length + blockSize - 1) % blockSize + 1 );
	}
	
	static void MakeHeader( const HFileInfo& pb, Header& h )
	{
		ZeroHeader( h );
		
		h.Set< kMacBinaryIIISignature >();  // 'mBIN'
		
		h.Set< kCurrentVersion >( kVersionMacBinaryIII );
		h.Set< kMinimumVersion >( kVersionMacBinaryII  );
		
		ASSERT( pb.ioNamePtr != NULL );
		
		h.Set< kFileName >( pb.ioNamePtr );
		
		/*
		h.Set< kFileType >( pb.ioFlFndrInfo.fdType );
		h.Set< kFileCreator >( pb.ioFlFndrInfo.fdCreator );
		h.Set< kFinderFlags >( pb.ioFlFndrInfo.fdFlags );
		h.Set< kIconPosition >( pb.ioFlFndrInfo.fdLocation );
		// folder ID
		*/
		h.Set< kFInfo >( pb.ioFlFndrInfo );
		
		h.Set< kFileCreationDate     >( pb.ioFlCrDat );
		h.Set< kFileModificationDate >( pb.ioFlMdDat );
		
		h.Set< kDataForkLength     >( pb.ioFlLgLen  );
		h.Set< kResourceForkLength >( pb.ioFlRLgLen );
		
		h.Set< kExtendedFinderFlags >( reinterpret_cast< const ExtendedFileInfo& >( pb.ioFlXFndrInfo ).extendedFinderFlags );
		
		h.Set< kCRC >();
	}
	
	union HeaderBlock
	{
		Header h;
		Block block;
	};
	
	Encoder::Encoder( const FSSpec& file )
	:
		fDataFork    ( N::FSpOpenDF( file, fsRdPerm ) ),
		fResourceFork( N::FSpOpenRF( file, fsRdPerm ) )
	{
		CInfoPBRec pb;
		
		N::FSpGetCatInfo( file, pb );
		
		if ( pb.hFileInfo.ioFlLgLen == 0 )
		{
			FS::Close( fDataFork );
		}
		
		if ( pb.hFileInfo.ioFlRLgLen == 0 )
		{
			FS::Close( fResourceFork );
		}
		
		pb.hFileInfo.ioNamePtr = const_cast< unsigned char* >( file.name );
		
		HeaderBlock u;
		
		MakeHeader( pb.hFileInfo, u.h );
		
		fBlocks.push_back( u.block );
		
		try
		{
			fComment = N::FSpDTGetComment( file );
			fComment.resize( PaddedLength( fComment.size(), kMacBinaryBlockSize ) );
		}
		catch ( ... ) {}
	}
	
	static void ReadBlocks( NN::Owned< N::FSFileRefNum >&  fork,
	                        char*&                         data,
	                        std::size_t&                   blocksRemaining )
	{
		if ( !blocksRemaining || fork.Get() == N::FSFileRefNum() )
		{
			return;
		}
		
		try
		{
			std::size_t bytes = FS::Read( fork,
			                              data,
			                              blocksRemaining * kMacBinaryBlockSize );
			
			if ( bytes % kMacBinaryBlockSize != 0 )
			{
				std::size_t roundedBytes = PaddedLength( bytes, kMacBinaryBlockSize );
				
				std::fill( data + bytes,
				           data + roundedBytes,
				           '\0' );
				
				bytes = roundedBytes;
			}
			
			data += bytes;
			blocksRemaining -= bytes / kMacBinaryBlockSize;
		}
		catch ( N::EOFErr& )
		{
			FS::Close( fork );
		}
	}
	
	int Encoder::Read( char* data, std::size_t byteCount )
	{
		char* const start = data;
		
		if ( byteCount % kMacBinaryBlockSize != 0 )
		{
			throw N::ParamErr();
		}
		
		const std::size_t blockCount = byteCount / kMacBinaryBlockSize;
		
		std::size_t blocksToCopy = std::min( blockCount, fBlocks.size() );
		
		// This will be true after the while loop, but we calculate it here
		std::size_t blocksRemaining = blockCount - blocksToCopy;
		
		while ( blocksToCopy > 0 )
		{
			const char* block = fBlocks.front().data;
			
			std::copy( block, block + kMacBinaryBlockSize, data );
			
			fBlocks.pop_front();
			
			data += kMacBinaryBlockSize;
			
			--blocksToCopy;
		}
		
		ReadBlocks( fDataFork,     data, blocksRemaining );
		ReadBlocks( fResourceFork, data, blocksRemaining );
		
		if ( blocksRemaining && !fComment.empty() )
		{
			ByteCount bytes = std::min( blocksRemaining * kMacBinaryBlockSize, fComment.size() );
			
			std::copy( fComment.begin(),
			           fComment.begin() + bytes,
			           data );
			
			std::copy( fComment.begin() + bytes,
			           fComment.end(),
			           fComment.begin() );
			
			fComment.resize( fComment.size() - bytes );
			
			data += bytes;
			//blocksRemaining -= bytes / kMacBinaryBlockSize;
		}
		
		std::size_t bytesRead = data - start;
		
		if ( bytesRead == 0 )
		{
			throw FS::EndOfFile();
		}
		
		return bytesRead;
	}
	
	Decoder::Decoder( const N::FSDirSpec& destination )
	:
		fDestDir( destination ),
		fHeaderReceived( false ),
		fDataForkLength       ( 0 ),
		fResourceForkLength   ( 0 ),
		fSecondaryHeaderLength( 0 ),
		fCommentLength        ( 0 )
	{
	}
	
	// Contrary to <http://www.lazerware.com/formats/macbinary/macbinary_iii.html>,
	// we will not be satisfied with the presence of 'mBIN' at offset 102.
	
	static bool VerifyMacBinaryI( const Header& h )
	{
		bool zeroByte82   = h.Check< kZeroByte82 >();
		
		return zeroByte82;
	}
	
	static bool VerifyMacBinaryII( const Header& h )
	{
		return true;
	}
	
	static bool VerifyMacBinaryIII( const Header& h )
	{
		return true;
	}
	
	static Byte CheckHeader( const Header& h )
	{
		bool zeroByte0    = h.Check< kOldVersion            >();
		bool zeroByte74   = h.Check< kZeroByte74            >();
		bool macBinIIIsig = h.Check< kMacBinaryIIISignature >();
		bool crc          = h.Check< kCRC                   >();
		
		Byte currentVersion = h.Get< kCurrentVersion >();
		Byte minimumVersion = h.Get< kMinimumVersion >();
		
		if ( zeroByte0 && zeroByte74 )
		{
			if ( crc )
			{
				return macBinIIIsig ? VerifyMacBinaryIII( h ) ? 3 : 0
				                    : VerifyMacBinaryII ( h ) ? 2 : 0;
			}
			else if ( VerifyMacBinaryI( h ) )
			{
				return 1;
			}
		}
		
		return 0;  // Not MacBinary
	}
	
	void Decoder::DecodeHeader( const char* header )
	{
		const Header& h = reinterpret_cast< const Header& >( *header );
		
		Byte check = CheckHeader( h );
		
		if ( check == 0 )
		{
			throw InvalidMacBinaryHeader();
		}
		
		Byte minimumVersion = h.Get< kMinimumVersion >();
		
		if ( kCurrentMacBinaryVersion < minimumVersion )
		{
			throw IncompatibleMacBinaryHeader();
		}
		
		ConstStr63Param name = h.Get< kFileName >();
		
		//fFile = N::FSMakeFSSpec( fDestDir, name );
		fFile = fDestDir & name;
		
		N::FSpCreate( fFile, h.Get< kFileCreator >(),
		                     h.Get< kFileType    >() );
		
		CInfoPBRec pb;
		
		N::FSpGetCatInfo( fFile, pb );
		
		pb.hFileInfo.ioDirID = fFile.parID;
		
		pb.hFileInfo.ioFlFndrInfo = h.Get< kFInfo >();
		
		pb.hFileInfo.ioFlCrDat = h.Get< kFileCreationDate     >();
		//pb.hFileInfo.ioFlMdDat = h.Get< kFileModificationDate >();
		
		pb.hFileInfo.ioFlLgLen  = fDataForkLength     = h.Get< kDataForkLength     >();
		pb.hFileInfo.ioFlRLgLen = fResourceForkLength = h.Get< kResourceForkLength >();
		
		reinterpret_cast< ExtendedFileInfo& >( pb.hFileInfo.ioFlXFndrInfo ).extendedFinderFlags = h.Get< kExtendedFinderFlags >();
		
		N::ThrowOSStatus( ::PBSetCatInfoSync( &pb ) );
		
		fModificationDate = h.Get< kFileModificationDate >();
		
		fSecondaryHeaderLength = h.Get< kSecondaryHeaderLength >();
		fCommentLength         = h.Get< kGetInfoCommentLength  >();
		
		fDataFork     = N::FSpOpenDF( fFile, fsWrPerm );
		fResourceFork = N::FSpOpenRF( fFile, fsWrPerm );
		
		fHeaderReceived = true;
	}
	
	int Decoder::Write( const char* data, ByteCount byteCount )
	{
		const char* const start = data;
		const char* const end   = data + byteCount;
		
		if ( byteCount == 0 )
		{
			return 0;
		}
		else if ( byteCount % kMacBinaryBlockSize != 0 )
		{
			throw N::ParamErr();
		}
		
		if ( !fHeaderReceived )
		{
			DecodeHeader( data );
			
			data += kMacBinaryHeaderLength;
		}
		
		if ( fSecondaryHeaderLength  &&  data < end )
		{
			ByteCount headerBytes = std::min< ByteCount >( fSecondaryHeaderLength, end - data );
			
			fSecondaryHeaderLength -= headerBytes;
			
			data += PaddedLength( headerBytes, kMacBinaryBlockSize );
		}
		
		if ( fDataForkLength  &&  data < end )
		{
			ByteCount dataBytes = std::min< ByteCount >( fDataForkLength, end - data );
			
			fDataForkLength -= dataBytes;
			
			FS::Write( fDataFork, data, dataBytes );
			
			data += PaddedLength( dataBytes, kMacBinaryBlockSize );
		}
		
		if ( fResourceForkLength  &&  data < end )
		{
			ByteCount resourceBytes = std::min< ByteCount >( fResourceForkLength, end - data );
			
			fResourceForkLength -= resourceBytes;
			
			FS::Write( fResourceFork, data, resourceBytes );
			
			data += PaddedLength( resourceBytes, kMacBinaryBlockSize );
		}
		
		if ( fCommentLength  &&  data < end )
		{
			ByteCount commentBytes = std::min< ByteCount >( fCommentLength, end - data );
			
			fCommentLength -= commentBytes;
			
			fComment.append( data, commentBytes );
			
			data += PaddedLength( commentBytes, kMacBinaryBlockSize );
		}
		
		if (    !fSecondaryHeaderLength
		     && !fDataForkLength
		     && !fResourceForkLength
		     && !fCommentLength )
		{
			// We're done
			FS::Close( fDataFork     );
			FS::Close( fResourceFork );
			
			if ( !fComment.empty() )
			{
				N::FSpDTSetComment( fFile, fComment );
			}
			
			CInfoPBRec pb;
			
			N::FSpGetCatInfo( fFile, pb );
			
			pb.hFileInfo.ioDirID = fFile.parID;
			
			pb.hFileInfo.ioFlMdDat = fModificationDate;
			
			N::ThrowOSStatus( ::PBSetCatInfoSync( &pb ) );
		}
		
		// If we got here, there's excess data.  Do we throw?
		
		return data - start;
	}
	
}

