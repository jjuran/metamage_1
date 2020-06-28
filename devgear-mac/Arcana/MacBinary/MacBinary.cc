/*
	MacBinary.cc
	------------
*/

/*
	Valid MacBinary III archives carry a purported "signature" of 'mBIN'.
	According to <http://www.lazerware.com/formats/macbinary/macbinary_iii.html>,
	a MacBinary III decoder is supposed to conclude, based only on the presence
	of this signature (which is at byte offset 102, not early enough to be
	considered a magic number, and which consists entirely of printable
	characters), that a file is a MacBinary III envelope.
	This is broken -- according to that test, THIS file is valid MacBinary III!
	
	Consequently, I'm operating under the assumption that MacBinary III files
	will continue to be valid MacBinary II files, and applying the MacBinary II
	validity tests even in the presence of the MacBinary III signature.
	This will only break under the following condition:
	A future version of MacBinary (say, MacBinary 4) creates files that are valid
	MacBinary III but invalid MacBinary II, which necessitates failing the
	MacBinary II validity tests.  However, given the forward compatibility means
	built into MacBinary II, this is *extremely* unlikely -- the secondary header
	field allows for considerable expansion, and in the event that's insufficient,
	the minimum version field is sufficient to ward off compliant decoders.
	Therefore, there is no technically sound reason to create files that are valid
	MacBinary III but invalid MacBinary II.  Concordantly, neither is there a
	technically sound reason *not* to apply the MacBinary II validity tests.
*/

#include "MacBinary.hh"

// Standard C
#include <string.h>

// mac-file-utils
#include "mac_file/directory.hh"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/MacMemory.hh"

// FSContents
#include "FSContents.h"


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


namespace MacBinary
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	using namespace io::path_descent_operators;
	
	using N::fsRdPerm;
	using N::fsWrPerm;
	
	
	static inline
	ByteCount min( ByteCount a, ByteCount b )
	{
		return b < a ? b : a;
	}
	
	template < class Type > struct ByteSwap_Traits
	{
		static Type Swap( Type value )
		{
			return value;
		}
	};
	
#if TARGET_RT_LITTLE_ENDIAN
	
	template <> struct ByteSwap_Traits< ::UInt16 >
	{
		static ::UInt16 Swap( ::UInt16 value )
		{
			return ::CFSwapInt16( value );
		}
	};
	
	template <> struct ByteSwap_Traits< ::UInt32 >
	{
		static ::UInt32 Swap( ::UInt32 value )
		{
			return ::CFSwapInt32( value );
		}
	};
	
	template <> struct ByteSwap_Traits< ::Point >
	{
		static ::Point Swap( ::Point point )
		{
			point.h = ::CFSwapInt16( point.h );
			point.v = ::CFSwapInt16( point.v );
			
			return point;
		}
	};
	
	template <> struct ByteSwap_Traits< ::FInfo >
	{
		static ::FInfo Swap( ::FInfo info )
		{
			info.fdType    = ::CFSwapInt32( info.fdType    );
			info.fdCreator = ::CFSwapInt32( info.fdCreator );
			info.fdFlags   = ::CFSwapInt16( info.fdFlags   );
			
			info.fdLocation = ByteSwap_Traits< ::Point >::Swap( info.fdLocation );
			
			return info;
		}
	};
	
#endif
	
	static
	unsigned short CalcCRC( register const unsigned char *dataBuf, size_t size )
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
	
	
	const UInt8 kVersionMacBinaryII  = 129;
	const UInt8 kVersionMacBinaryIII = 130;
	
	const UInt8 kCurrentMacBinaryVersion = kVersionMacBinaryIII;
	
	
	enum HeaderField
	{
		kOldVersion,
		kOldVersionForMacBinaryIIPlus,
		kFileName,
		kFileType,
		kFileCreator,
		kFinderFlags,
		kZeroByte74,
		//kIconPosition,
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
	
	template < UInt8 value, size_t offset >
	struct Byte_Field_Traits
	{
		typedef UInt8 Value;
		
		static bool Check( const Header& h )
		{
			return h.data[ offset ] == value;
		}
		
		static void Set( Header& h )
		{
			h.data[ offset ] = value;
		}
	};
	
	template < size_t offset >
	struct Zero_Field_Traits : Byte_Field_Traits< 0, offset > {};
	
	template < class Type, size_t offset >
	struct POD_Field_Traits
	{
		typedef Type Value;
		
		static Value Get( const Header& h )
		{
			return ByteSwap_Traits< Type >::Swap( reinterpret_cast< const Value& >( h.data[ offset ] ) );
		}
		
		static void Set( Header& h, Value v )
		{
			reinterpret_cast< Value& >( h.data[ offset ] ) = ByteSwap_Traits< Type >::Swap( v );
		}
	};
	
	template < class Type, size_t offset >
	struct MisalignedPOD_Field_Traits
	{
		typedef Type Value;
		
		static Value Get( const Header& h )
		{
			Value result;
			
			memcpy( &result, h.data + offset, sizeof (Value) );
			
			return ByteSwap_Traits< Type >::Swap( result );
		}
		
		static void Set( Header& h, Value v )
		{
			v = ByteSwap_Traits< Type >::Swap( v );
			
			memcpy( h.data + offset, &v, sizeof (Value) );
		}
	};
	
	template <> struct Field_Traits< kOldVersion > : Zero_Field_Traits< 0 > {};
	
	template <> struct Field_Traits< kOldVersionForMacBinaryIIPlus > : Byte_Field_Traits< 1, 0 > {};
	
	template <>
	struct Field_Traits< kFileName >
	{
		typedef ConstStr63Param Value;
		
		static const size_t offset = 1;
		
		static bool Check( const Header& h )
		{
			return h.data[ offset ] != 0;  // Non-empty filename
		}
		
		static Value Get( const Header& h )  { return &h.data[ offset ]; }
		
		static void Set( Header& h, Value v )
		{
			memcpy( &h.data[ offset ], v, 1 + v[ 0 ] );
		}
	};
	
	template <> struct Field_Traits< kFileType    > : MisalignedPOD_Field_Traits< ::OSType, 65 > {};
	template <> struct Field_Traits< kFileCreator > : MisalignedPOD_Field_Traits< ::OSType, 69 > {};
	
	template <>
	struct Field_Traits< kFinderFlags >
	{
		typedef unsigned short Value;
		
		static const size_t highOffset = 73;
		static const size_t lowOffset = 101;
		
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
			Low ::Set( h, v      );
		}
	};
	
	template <> struct Field_Traits< kZeroByte74 > : Zero_Field_Traits< 74 > {};
	
	//template <> struct Field_Traits< kIconPosition > : MisalignedPOD_Field_Traits< ::Point, 75 > {};
	
	template <>
	struct Field_Traits< kFInfo >
	{
		typedef FInfo Value;
		
		static const size_t offset = 65;
		
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
		static const size_t offset     = 124;
		static const size_t dataLength = 124;
		
		typedef POD_Field_Traits< UInt16, offset > Field;
		
		static UInt16 CRC( const unsigned char* data )  { return CalcCRC( data, dataLength ); }
		
		static bool Check( const Header& h )  { return Field::Get( h ) == CRC( h.data ); }
		
		static void Set( Header& h )  { Field::Set( h, CRC( h.data ) ); }
	};
	
	
	// 0 < k < blockSize
	// 
	// 0             -> 0
	// k             -> blockSize
	// blockSize     -> blockSize
	// blockSize + k -> blockSize * 2
	// etc.
	static
	size_t PaddedLength( size_t length, size_t blockSize )
	{
		return length + blockSize - ( (length + blockSize - 1) % blockSize + 1 );
	}
	
	static inline
	void ZeroHeader( Header& h )
	{
		memset( h.data, '\0', kMacBinaryHeaderLength );
	}
	
	static void MakePartialHeaderForItem( const HFileInfo& pb, Header& h )
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
		
		h.Set< kExtendedFinderFlags >( reinterpret_cast< const ExtendedFileInfo& >( pb.ioFlXFndrInfo ).extendedFinderFlags );
	}
	
	static void MakeFileHeader( const HFileInfo& pb, Header& h )
	{
		MakePartialHeaderForItem( pb, h );
		
		h.Set< kOldVersion >();
		
		h.Set< kDataForkLength     >( pb.ioFlLgLen  );
		h.Set< kResourceForkLength >( pb.ioFlRLgLen );
		
		h.Set< kCRC >();
	}
	
	static void MakeFolderHeader( const HFileInfo& pb, Header& h )
	{
		MakePartialHeaderForItem( pb, h );
		
		h.Set< kOldVersionForMacBinaryIIPlus >();
		
		h.Set< kFileType    >( 'fold'     );
		h.Set< kFileCreator >( 0xFFFFFFFF );
		
		h.Set< kCRC >();
	}
	
	union HeaderBlock
	{
		Header h;
		Block block;
	};
	
	static
	void ReadWrite( N::FSFileRefNum file, BlockWriter blockWrite, int output, size_t byteCount )
	{
		size_t paddedCount = PaddedLength( byteCount, kMacBinaryBlockSize );
		
		n::owned< N::Handle > tempMem = N::TempNewHandle( paddedCount );
		
		char* buffer = *tempMem.get().Get();
		
		UInt32 bytesRead = N::FSRead( file, byteCount, buffer, N::ThrowEOF_Never() );
		
		memset( buffer + bytesRead, '\0', paddedCount - bytesRead );
		
		if ( !TARGET_API_MAC_OSX )
		{
			N::HLock( tempMem );
		}
		
		blockWrite( output, &buffer[0], paddedCount );
	}
	
	static void EncodeFile( const FSSpec& file, HFileInfo& hFileInfo, BlockWriter blockWrite, int output )
	{
		nucleus::mutable_string comment;
		
		try
		{
			comment = N::FSpDTGetComment( file );
			
			comment.resize( PaddedLength( comment.size(), kMacBinaryBlockSize ) );
		}
		catch ( ... )
		{
		}
		
		const size_t comment_size = comment.size();
		
		HeaderBlock u;
		
		MakeFileHeader( hFileInfo, u.h );
		
		u.h.Set< kGetInfoCommentLength >( comment_size );
		
		blockWrite( output, u.block.data, sizeof u.block );
		
		if ( hFileInfo.ioFlLgLen > 0 )
		{
			ReadWrite( N::HOpen( file, fsRdPerm ), blockWrite, output,  hFileInfo.ioFlLgLen );
		}
		
		if ( hFileInfo.ioFlRLgLen > 0 )
		{
			ReadWrite( N::HOpenRF( file, fsRdPerm ), blockWrite, output,  hFileInfo.ioFlRLgLen );
		}
		
		if ( comment_size > 0 )
		{
			blockWrite( output, comment.data(), comment_size );
		}
	}
	
	static void EncodeFolder( CInfoPBRec& cInfo, BlockWriter blockWrite, int output )
	{
		HeaderBlock u;
		
		MakeFolderHeader( cInfo.hFileInfo, u.h );
		
		blockWrite( output, u.block.data, sizeof u.block );
		
		N::FSDirSpec dir = n::make< N::FSDirSpec >( cInfo );
		
		N::FSSpecContents_Container contents = N::FSContents( dir );
		
		typedef N::FSSpecContents_Container::const_iterator const_iterator;
		
		for ( const_iterator it = contents.begin();  it != contents.end();  ++it )
		{
			Encode( dir / *it, blockWrite, output );
		}
		
		u.h.Set< kFileCreator >( 0xFFFFFFFE );
		u.h.Set< kCRC >();
		
		blockWrite( output, u.block.data, sizeof u.block );
	}
	
	void Encode( const FSSpec& file, BlockWriter blockWrite, int output )
	{
		CInfoPBRec cInfo;
		
		N::FSpGetCatInfo( file, cInfo );
		
		cInfo.hFileInfo.ioNamePtr = const_cast< unsigned char* >( file.name );
		
		bool isDir = io::item_is_directory( cInfo );
		
		if ( isDir )
		{
			EncodeFolder( cInfo, blockWrite, output );
		}
		else
		{
			EncodeFile( file, cInfo.hFileInfo, blockWrite, output );
		}
	}
	
	static bool VerifyMacBinaryI( const Header& h )
	{
		bool zeroByte82   = h.Check< kZeroByte82 >();
		
		return zeroByte82;
	}
	
	static bool VerifyMacBinaryII( const Header& )
	{
		return true;
	}
	
	static bool VerifyMacBinaryIII( const Header& )
	{
		return true;
	}
	
	// Contrary to <http://www.lazerware.com/formats/macbinary/macbinary_iii.html>,
	// we will not be satisfied with the presence of 'mBIN' at offset 102.
	
	static Byte CheckHeader( const Header& h )
	{
		bool zeroByte0    = h.Check< kOldVersion            >();
		bool hasFilename  = h.Check< kFileName              >();
		bool zeroByte74   = h.Check< kZeroByte74            >();
		bool macBinIIIsig = h.Check< kMacBinaryIIISignature >();
		bool crc          = h.Check< kCRC                   >();
		
		if ( !hasFilename )
		{
			return 0;
		}
		
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
		
		// Not a MacBinary-encoded file, check for a directory tree
		
		bool versionOne = h.Check< kOldVersionForMacBinaryIIPlus >();
		
		::OSType type    = h.Get< kFileType    >();
		::OSType creator = h.Get< kFileCreator >();
		
		if ( versionOne  &&  crc  &&  type == 'fold'  &&  (creator & 0xFFFFFFFE) == 0xFFFFFFFE )
		{
			return (Byte) creator;
		}
		
		return 0;
	}
	
	Decoder::Decoder( const VRefNum_DirID& destination )
	:
		//fDestDir( destination ),
		itIsFolder( false ),
		itsHeaderWasReceived( false ),
		itsDataForkLength       ( 0 ),
		itsResourceForkLength   ( 0 ),
		itsSecondaryHeaderLength( 0 ),
		itsCommentLength        ( 0 )
	{
		itsFrame.destDir = destination;
	}
	
	void Decoder::DecodeHeader( const char* header )
	{
		OSErr err;
		
		const Header& h = reinterpret_cast< const Header& >( *header );
		
		Byte check = CheckHeader( h );
		
		if ( check == 0 )
		{
			throw InvalidMacBinaryHeader();
		}
		
		itIsFolder = check >= 0xFE;
		
		itsTrailerWasReceived = check == 0xFE;
		
		if ( itsTrailerWasReceived )
		{
			return;
		}
		
		Byte minimumVersion = h.Get< kMinimumVersion >();
		
		if ( kCurrentMacBinaryVersion < minimumVersion )
		{
			throw IncompatibleMacBinaryHeader();
		}
		
		ConstStr63Param name = h.Get< kFileName >();
		
		const VRefNum_DirID& dir = itsFrame.destDir;
		
		itsFrame.file.vRefNum = itsFrame.destDir.vRefNum;
		itsFrame.file.parID   = itsFrame.destDir.dirID;
		
		memcpy( itsFrame.file.name, name, sizeof (Str63) );
		
		if ( itIsFolder )
		{
			long newDirID;
			
			err = DirCreate( dir.vRefNum, dir.dirID, name, &newDirID );
		}
		else
		{
			const OSType creator = h.Get< kFileCreator >();
			const OSType type    = h.Get< kFileType    >();
			
			err = HCreate( dir.vRefNum, dir.dirID, name, creator, type );
		}
		
		Mac::ThrowOSStatus( err );
		
		CInfoPBRec pb;
		
		N::FSpGetCatInfo( itsFrame.file, pb );
		
		pb.hFileInfo.ioNamePtr = itsFrame.file.name;
		pb.hFileInfo.ioDirID   = itsFrame.file.parID;
		
		pb.hFileInfo.ioFlFndrInfo = h.Get< kFInfo >();
		
		pb.hFileInfo.ioFlCrDat = h.Get< kFileCreationDate     >();
		pb.hFileInfo.ioFlMdDat = h.Get< kFileModificationDate >();
		
		if ( !itIsFolder )
		{
			pb.hFileInfo.ioFlLgLen  = itsDataForkLength     = h.Get< kDataForkLength     >();
			pb.hFileInfo.ioFlRLgLen = itsResourceForkLength = h.Get< kResourceForkLength >();
		}
		
		reinterpret_cast< ExtendedFileInfo& >( pb.hFileInfo.ioFlXFndrInfo ).extendedFinderFlags = h.Get< kExtendedFinderFlags >();
		
		N::PBSetCatInfoSync( pb );
		
		itsFrame.modificationDate = h.Get< kFileModificationDate >();
		
		itsSecondaryHeaderLength = h.Get< kSecondaryHeaderLength >();
		itsCommentLength         = h.Get< kGetInfoCommentLength  >();
		
		if ( !itIsFolder )
		{
			itsDataFork     = N::HOpen  ( itsFrame.file, fsWrPerm );
			itsResourceFork = N::HOpenRF( itsFrame.file, fsWrPerm );
		}
		
		itsHeaderWasReceived = true;
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
			Mac::ThrowOSStatus( paramErr );
		}
		
	Loop:
		
		if ( !itsHeaderWasReceived )
		{
			DecodeHeader( data );
			
			data += kMacBinaryHeaderLength;
		}
		
		if ( itsSecondaryHeaderLength  &&  data < end )
		{
			ByteCount headerBytes = min( itsSecondaryHeaderLength, end - data );
			
			itsSecondaryHeaderLength -= headerBytes;
			
			data += PaddedLength( headerBytes, kMacBinaryBlockSize );
		}
		
		if ( itsDataForkLength  &&  data < end )
		{
			ByteCount dataBytes = min( itsDataForkLength, end - data );
			
			itsDataForkLength -= dataBytes;
			
			io::write( itsDataFork, data, dataBytes );
			
			data += PaddedLength( dataBytes, kMacBinaryBlockSize );
		}
		
		if ( itsResourceForkLength  &&  data < end )
		{
			ByteCount resourceBytes = min( itsResourceForkLength, end - data );
			
			itsResourceForkLength -= resourceBytes;
			
			io::write( itsResourceFork, data, resourceBytes );
			
			data += PaddedLength( resourceBytes, kMacBinaryBlockSize );
		}
		
		if ( itsCommentLength  &&  data < end )
		{
			ByteCount commentBytes = min( itsCommentLength, end - data );
			
			itsCommentLength -= commentBytes;
			
			itsFrame.comment.append( data, commentBytes );
			
			data += PaddedLength( commentBytes, kMacBinaryBlockSize );
		}
		
		if (    !itsSecondaryHeaderLength
		     && !itsDataForkLength
		     && !itsResourceForkLength
		     && !itsCommentLength )
		{
			// We're done
			
			itsHeaderWasReceived = false;
			
			if ( itIsFolder && !itsTrailerWasReceived )
			{
				itsFrameStack.push_back( itsFrame );
				
				itsFrame.destDir = mac::file::directory( itsFrame.file );
				
				itsFrame.comment.clear();
			}
			else
			{
				if ( itIsFolder )
				{
					if ( itsFrameStack.empty() )
					{
						throw TooManyEndBlocks();
					}
					
					itsFrame = itsFrameStack.back();
					itsFrameStack.pop_back();
				}
				else
				{
					N::FSClose( itsDataFork     );
					N::FSClose( itsResourceFork );
				}
				
				if ( !itsFrame.comment.empty() )
				{
					N::FSpDTSetComment( itsFrame.file, itsFrame.comment );
				}
				
				CInfoPBRec pb;
				
				N::FSpGetCatInfo( itsFrame.file, pb );
				
				pb.hFileInfo.ioNamePtr = itsFrame.file.name;
				pb.hFileInfo.ioDirID   = itsFrame.file.parID;
				
				// Writing to the file bumps the mod date, so set it back
				pb.hFileInfo.ioFlMdDat = itsFrame.modificationDate;
				
				// Clear flags
				UInt16 flagsToClear = kIsOnDesk;
				
				if ( itsFrameStack.empty() )
				{
					// Let Finder init the top-level item
					flagsToClear |= kHasBeenInited;
				}
				
				pb.hFileInfo.ioFlFndrInfo.fdFlags &= ~flagsToClear;
				
				if ( itsFrameStack.empty() )
				{
					// Root folder / file
					static const Point emptyPoint = { 0, 0 };
					
					pb.hFileInfo.ioFlFndrInfo.fdLocation = emptyPoint;
					pb.hFileInfo.ioFlFndrInfo.fdFldr     = 0;
				}
				
				N::PBSetCatInfoSync( pb );
			}
		}
		
		if ( data < end )
		{
			goto Loop;
		}
		
		return data - start;
	}
	
}
