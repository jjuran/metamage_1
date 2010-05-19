// Nitrogen/Files.hh
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_FILES_HH
#define NITROGEN_FILES_HH

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"
#include "nucleus/index_until_error_sequence.hh"
#include "nucleus/initialize.hh"

#ifndef __FILES__
#include <Files.h>
#endif

// iota
#include "iota/string_traits.hh"

// Nitrogen
#include "Mac/Files/Functions/FlushVol.hh"
#include "Mac/Files/Functions/UnmountVol.hh"
#include "Mac/Files/Types/FSDirID.hh"
#include "Mac/Files/Types/FSIOPerm.hh"
#include "Mac/Files/Types/FSVolumeRefNum.hh"

#ifndef NITROGEN_CFSTRING_HH
#include "Nitrogen/CFString.hh"
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
#endif
#ifndef NITROGEN_TEXTCOMMON_HH
#include "Nitrogen/TextCommon.hh"
#endif

#include "Nitrogen/Devices.hh"
#include "Nitrogen/UPP.hh"

#ifndef __LAMP__
#include <vector>
#endif


#undef PBGetCatInfo

inline OSErr PBGetCatInfo( CInfoPBRec* pb, Boolean async )
{
	return async ? PBGetCatInfoAsync( pb )
	             : PBGetCatInfoSync ( pb );
}


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( FileManager );
	
	
	struct FileSignature
	{
		OSType creator;
		OSType type;
		
		FileSignature() : creator(), type()
		{
		}
		
		FileSignature( OSType creator,
		               OSType type ) : creator( creator ),
		                               type   ( type    )
		{
		}
		
		FileSignature( const FInfo& fInfo ) : creator( OSType( fInfo.fdCreator ) ),
		                                      type   ( OSType( fInfo.fdType    ) )
		{
		}
	};
	
	
	using Mac::FSDirID;
	using Mac::fsRtParID;
	using Mac::fsRtDirID;
	
	enum FSNodeFlags
	{
		kFSNodeFlags_Max = nucleus::enumeration_traits< UInt16 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSNodeFlags )
	
	enum FSNodeID
	{
		kFSNodeID_Max = nucleus::enumeration_traits< UInt32 >::max
	};
	
	using Mac::FSVolumeRefNum;
	
	enum FSSharingFlags
	{
		kFSSharingFlags_Max = nucleus::enumeration_traits< UInt8 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSSharingFlags )
	
	typedef FSSharingFlags FSIOFileAttributes;
	typedef FSSharingFlags FSIOFlAttrib;
	
	enum FSUserPrivileges
	{
		kFSUserPrivileges_Max = nucleus::enumeration_traits< UInt8 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSUserPrivileges )
	
	typedef FSUserPrivileges FSIOACUser;
	
	enum FSIteratorFlags
	{
		kFSIteratorFlags_Max = nucleus::enumeration_traits< ::FSIteratorFlags >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSIteratorFlags )
	
	enum FSFileRefNum
	{
		kFSFileRefNum_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	enum FSForkRefNum
	{
		kFSForkRefNum_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	using Mac::FSIOPerm;
	using Mac::FSIOPermssn;
	using Mac::FSIOPermissions;
	
	using Mac::fsCurPerm;
	using Mac::fsRdPerm;
	using Mac::fsWrPerm;
	using Mac::fsRdWrPerm;
	using Mac::fsRdWrShPerm;
	using Mac::fsRdDenyPerm;
	using Mac::fsWrDenyPerm;
	
	enum FSIOPosMode
	{
		fsAtMark    = ::fsAtMark,
		fsFromStart = ::fsFromStart,
		fsFromLEOF  = ::fsFromLEOF,
		fsFromMark  = ::fsFromMark,
		
		kFSIOPosMode_Max = nucleus::enumeration_traits< UInt16 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSIOPosMode )
	
	typedef FSIOPosMode FSIOPositioningMode;
	
	enum FSAllocationFlags
	{
		kFSAllocationFlags_Max = nucleus::enumeration_traits< ::FSAllocationFlags >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSAllocationFlags )
	
	enum FSForkIterator
	{
		kFSForkIterator_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	enum FSVolumeInfoFlags
	{
		kFSVolumeInfoFlags_Max = nucleus::enumeration_traits< UInt16 >::max
	};
	
	typedef FSVolumeInfoFlags FSIOVAtrb;
	typedef FSVolumeInfoFlags FSIOVolumeAttributes;
	
	NUCLEUS_DEFINE_FLAG_OPS( FSVolumeInfoFlags )
	
	enum FSVolumeIndex
	{
		kFSVolumeIndex_Max = nucleus::enumeration_traits< ::ItemCount >::max
	};
	
	enum FSFileSystemID
	{
		kFSFileSystemID_Max = nucleus::enumeration_traits< UInt16 >::max
	};
	
	typedef FSFileSystemID IOFSID;
	
	enum DriverReferenceNumber
	{
		kDriverReferenceNumber_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	typedef DriverReferenceNumber DRefNum;
	typedef DriverReferenceNumber DriverRefNum;
	
	enum HFSCatalogNodeID
	{
		kHFSCatalogNodeID_Max = nucleus::enumeration_traits< UInt32 >::max
	};
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	struct IOCompletionUPP_Details
	{
		typedef ::IOCompletionUPP UPPType;
		
		// This is the stack-based function signature
		typedef pascal void (*ProcPtr)( ::ParamBlockRec* pb );
		
		template < ProcPtr procPtr >
		static pascal void Glue()
		{
			Call_With_A0_Glue< ProcPtr, procPtr >();
		}
	};
	
	typedef GlueUPP< IOCompletionUPP_Details > IOCompletionUPP;
	
#else
	
	struct IOCompletionUPP_Details : Basic_UPP_Details< ::IOCompletionUPP,
	                                                    ::IOCompletionProcPtr,
	                                                    ::NewIOCompletionUPP,
	                                                    ::DisposeIOCompletionUPP,
	                                                    ::InvokeIOCompletionUPP >
	{
	};
	
	typedef UPP< IOCompletionUPP_Details > IOCompletionUPP;
	
#endif
	
	using ::HFSUniStr255;
	using ::CInfoPBRec;
	using ::DirInfo;
	using ::FSSpec;
	using ::FSRef;
	
}

namespace nucleus
  {
   template <> struct converter< owned<CFStringRef>, HFSUniStr255 >
     {
      typedef HFSUniStr255          argument_type;
      typedef owned< CFStringRef >  result_type;
      
      owned<CFStringRef> operator()( const HFSUniStr255& in ) const
        {
         return Nitrogen::CFStringCreateWithCharacters( in.unicode, in.length );
        }
     };
   
   template <> struct converter< HFSUniStr255, CFStringRef >
     {
      typedef CFStringRef   argument_type;
      typedef HFSUniStr255  result_type;
      
      HFSUniStr255 operator()( CFStringRef in ) const
        {
         CFIndex length = Nitrogen::CFStringGetLength( in );
         if ( length > 255 )
            throw Nitrogen::StringTooLong();
         HFSUniStr255 result;
         result.length = UInt16( length );
         Nitrogen::CFStringGetCharacters( in, CFRangeMake( 0, length ), result.unicode );
         return result;
        }
     };

   template <> struct converter< Nitrogen::UniString, HFSUniStr255 >
     {
      typedef HFSUniStr255         argument_type;
      typedef Nitrogen::UniString  result_type;
      
      Nitrogen::UniString operator()( const HFSUniStr255& in ) const
        {
         return Nitrogen::UniString( in.unicode, in.unicode+in.length );
        }
     };
  
   template <> struct converter< HFSUniStr255, Nitrogen::UniString >
     {
      typedef Nitrogen::UniString  argument_type;
      typedef HFSUniStr255         result_type;
      
      HFSUniStr255 operator()( const Nitrogen::UniString& in ) const
        {
         if ( in.size() > 255 )
            throw Nitrogen::StringTooLong();
         HFSUniStr255 result;
         result.length = UInt16( in.size() );
         std::copy( in.begin(), in.end(), result.unicode );
         return result;
        }
     };
	
	template <> struct disposer< Nitrogen::FSFileRefNum >
	{
		typedef Nitrogen::FSFileRefNum  argument_type;
		typedef void                    result_type;
		
		void operator()( Nitrogen::FSFileRefNum file ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::FileManager );
			
			::Nitrogen::HandleDestructionOSStatus( ::FSClose( file ) );
		}
	};
  }

namespace Nitrogen
  {
	
	// Trivial PBFoo() wrappers
	
	void PBHOpenDFSync ( HParamBlockRec& pb );
	void PBHOpenDFAsync( HParamBlockRec& pb );
	void PBHOpenRFSync ( HParamBlockRec& pb );
	void PBHOpenRFAsync( HParamBlockRec& pb );
	
	using Mac::UnmountVol;
	using Mac::FlushVol;
	
	// HSetVol
	
	void FSClose( nucleus::owned< FSFileRefNum > fileRefNum );
	
	template < class EOF_Policy >
	SInt32 FSRead( FSFileRefNum  file,
	               SInt32        requestCount,
	               void *        buffer,
	               EOF_Policy    policy )
	{
		NUCLEUS_REQUIRE_ERRORS( FileManager );
		
		SInt32 actualCount = requestCount;
		
		OSErr err = ::FSRead( file, &actualCount, buffer );
		
		ThrowReadOSStatus( err, actualCount, policy );
		
		return actualCount;
	}
	
	template < class Element, std::size_t count, class Policy >
	inline SInt32 FSRead( FSFileRefNum  file,
	                      Element       (&buffer)[count],
	                      Policy        policy )
	{
		return FSRead( file, count * sizeof (Element), buffer, policy );
	}
	
	
	SInt32 FSWrite( FSFileRefNum  file,
	                SInt32        requestCount,
	                const void *  buffer );
	
	template < class Element, std::size_t count >
	inline SInt32 FSWrite( FSFileRefNum   file,
	                       const Element  (&buffer)[count] )
	{
		return FSWrite( file, count * sizeof (Element), buffer );
	}
	
	
	SInt32 Allocate( FSFileRefNum  fileRefNum,
	                 SInt32        requestCount );
	
	SInt32 GetEOF( FSFileRefNum fileRefNum );
	
	void SetEOF( FSFileRefNum  fileRefNum,
	             SInt32        positionOffset );
	
	SInt32 GetFPos( FSFileRefNum fileRefNum );
	
	void SetFPos( FSFileRefNum  fileRefNum,
	              FSIOPosMode   positionMode,
	              SInt32        positionOffset );
	
	// GetVRefNum
	
	struct FSDirSpec
	{
		FSVolumeRefNum  vRefNum;
		FSDirID         dirID;
		
		FSDirSpec() : vRefNum(), dirID()
		{
		}
	};
	
	inline bool operator==( const FSDirSpec& a, const FSDirSpec& b )
	{
		return a.vRefNum == b.vRefNum
		    && a.dirID   == b.dirID;
	}
	
	inline bool operator!=( const FSDirSpec& a, const FSDirSpec& b )
	{
		return !( a == b );
	}
	
  }

namespace nucleus
{
	
	template <>
	struct maker< Nitrogen::FSDirSpec >
	{
		Nitrogen::FSDirSpec operator()( Nitrogen::FSVolumeRefNum  vRefNum,
		                                Nitrogen::FSDirID         dirID ) const
		{
			Nitrogen::FSDirSpec result;
			
			result.vRefNum = vRefNum;
			result.dirID   = dirID;
			
			return result;
		}
		
		Nitrogen::FSDirSpec operator()( const ::DirInfo& dirInfo ) const
		{
			return operator()( Nitrogen::FSVolumeRefNum( dirInfo.ioVRefNum ),
			                   Nitrogen::FSDirID       ( dirInfo.ioDrDirID ) );
		}
		
		Nitrogen::FSDirSpec operator()( const ::CInfoPBRec& cInfo ) const
		{
			return operator()( cInfo.dirInfo );
		}
	};
	
	template <>
	struct initializer< CInfoPBRec >
	{
		CInfoPBRec& operator()( CInfoPBRec&               pb,
		                        Nitrogen::FSVolumeRefNum  vRefNum,
		                        Nitrogen::FSDirID         dirID,
		                        StringPtr                 name,
		                        SInt16                    index );
	};
	
}

namespace Nitrogen
  {
	
	// PBHSetVolSync
	
	// ...
	
	struct FNF_Throws
	{
		typedef void Result;
		
		static void HandleOSStatus( OSStatus err )
		{
			ThrowOSStatus( err );
		}
	};
	
	struct FNF_Returns
	{
		typedef bool Result;
		
		static bool HandleOSStatus( OSStatus err )
		{
			if ( err == fnfErr )
			{
				return false;
			}
			
			ThrowOSStatus( err );
			
			return true;
		}
	};
	
	
	void PBGetCatInfoSync ( CInfoPBRec& pb, FNF_Throws  policy );
	bool PBGetCatInfoSync ( CInfoPBRec& pb, FNF_Returns policy );
	void PBGetCatInfoAsync( CInfoPBRec& pb, FNF_Throws  policy );
	bool PBGetCatInfoAsync( CInfoPBRec& pb, FNF_Returns policy );
	
	// throw FNFErr by default
	inline void PBGetCatInfoSync( CInfoPBRec& pb )
	{
		PBGetCatInfoSync( pb, FNF_Throws() );
	}
	
	inline void PBGetCatInfoAsync( CInfoPBRec& pb )
	{
		PBGetCatInfoAsync( pb, FNF_Throws() );
	}
	
	
	void PBSetCatInfoSync ( CInfoPBRec& pb );
	void PBSetCatInfoAsync( CInfoPBRec& pb );
	
	// ...
	
	
	// Synchronous, throws FNFErr
	inline void FSpGetCatInfo( const FSSpec&  item,
	                           CInfoPBRec&    pb,
	                           FNF_Throws     policy = FNF_Throws() )
	{
		Str255 name = item.name;
		
		nucleus::initialize< CInfoPBRec >( pb,
		                                   FSVolumeRefNum( item.vRefNum ),
		                                   FSDirID       ( item.parID   ),
		                                   name,
		                                   0 );
		
		PBGetCatInfoSync( pb, policy );
	}
	
	// Synchronous, returns false on fnfErr
	inline bool FSpGetCatInfo( const FSSpec&  item,
	                           CInfoPBRec&    pb,
	                           FNF_Returns    policy )
	{
		Str255 name = item.name;
		
		nucleus::initialize< CInfoPBRec >( pb,
		                                   FSVolumeRefNum( item.vRefNum ),
		                                   FSDirID       ( item.parID   ),
		                                   name,
		                                   0 );
		
		return PBGetCatInfoSync( pb, policy );
	}
	
	FSDirSpec FSpMake_FSDirSpec( const FSSpec& dir );
	
	// ...
	
	void PBHGetVolParmsSync( HParamBlockRec& paramBlock );
	
	GetVolParmsInfoBuffer PBHGetVolParmsSync( FSVolumeRefNum vRefNum );
	
	// Desktop Manager
	// ---------------
	
	void PBDTGetPath( DTPBRec& pb );
	void PBDTGetPath( FSVolumeRefNum vRefNum, DTPBRec& pb );
	
	DTPBRec& FSpDTGetPath( const FSSpec& file, DTPBRec& pb );
	
	// PBDTCloseDown
	// PBDTAddIconSync
	// PBDTGetIconSync
	// PBDTGetIconInfoSync
	// PBDTAddAPPLSync
	// PBDTRemoveAPPLSync
	
	void PBDTGetAPPLSync( DTPBRec& pb );
	
	FSSpec DTGetAPPL( OSType signature, FSVolumeRefNum vRefNum );
	
	void PBDTSetCommentSync( DTPBRec& pb );
	
	void DTSetComment( DTPBRec& pb, const char* comment, unsigned length );
	
	template < class String >
	void DTSetComment( DTPBRec& pb, const String& comment )
	{
		DTSetComment( pb,
		              iota::get_string_data( comment ),
		              iota::get_string_size( comment ) );
	}
	
	void FSpDTSetComment( const FSSpec& file, const char* comment, unsigned length );
	
	template < class String >
	void FSpDTSetComment( const FSSpec& file, const String& comment )
	{
		FSpDTSetComment( file,
		                 iota::get_string_data( comment ),
		                 iota::get_string_size( comment ) );
	}
	
	// PBDTRemoveCommentSync
	
	void PBDTGetCommentSync( DTPBRec& pb );
	
	nucleus::string DTGetComment( DTPBRec& pb );
	
	nucleus::string FSpDTGetComment( const FSSpec& file );
	
	// PBDTFlushSync
	// PBDTResetSync
	// PBDTGetInfoSync
	// PBDTOpenInform
	// PBDTDeleteSync
	
	FSSpec FSMakeFSSpec( FSVolumeRefNum    vRefNum,
	                     FSDirID           dirID,
	                     ConstStr255Param  name = NULL );
	
	inline FSSpec FSMakeFSSpec( const FSDirSpec&  dir,
	                            ConstStr255Param  name = NULL )
	{
		return FSMakeFSSpec( dir.vRefNum, dir.dirID, name );
	}
	
	FSSpec FSMakeFSSpec( const FSRef& ref );
	
  }

namespace nucleus
  {
	template <> struct converter< FSSpec, Nitrogen::FSDirSpec >
	{
		typedef Nitrogen::FSDirSpec  argument_type;
		typedef FSSpec               result_type;
		
		FSSpec operator()( const Nitrogen::FSDirSpec& dir ) const
		{
			return Nitrogen::FSMakeFSSpec( dir );
		}
	};
	
	template <> struct converter< Nitrogen::FSDirSpec, FSSpec >
	{
		typedef FSSpec               argument_type;
		typedef Nitrogen::FSDirSpec  result_type;
		
		Nitrogen::FSDirSpec operator()( const FSSpec& dir ) const
		{
			return Nitrogen::FSpMake_FSDirSpec( dir );
		}
	};
  }

namespace Nitrogen
  {
	
	FSSpec FSMakeFSSpec( ConstStr255Param pathname );
	
	bool FSCompareFSSpecs( const FSSpec& a, const FSSpec& b );
	
	
	nucleus::owned< FSFileRefNum > FSpOpenDF( const FSSpec&   spec,
	                                          FSIOPermssn     permissions );
	
	
	nucleus::owned< FSFileRefNum > FSpOpenRF( const FSSpec&   spec,
	                                          FSIOPermssn     permissions );
	
	
	FSSpec FSpCreate( const FSSpec&  file, 
	                  OSType         creator, 
	                  OSType         type, 
	                  ScriptCode     scriptTag = smSystemScript );
	
	inline FSSpec FSpCreate( const FSSpec&         file,
	                         const FileSignature&  signature,
	                         ScriptCode            scriptTag = smSystemScript )
	{
		return FSpCreate( file, signature.creator, signature.type, scriptTag );
	}
	
	FSDirSpec FSpDirCreate( const FSSpec&  dir, 
	                        ScriptCode     scriptTag = smSystemScript );
	
	void FSpDelete( const FSSpec& item );
	
	FInfo FSpGetFInfo( const FSSpec& file );
	
	void FSpSetFInfo( const FSSpec& file, const FInfo& info );
	
	void FSpSetFLock( const FSSpec& file );
	
	void FSpRstFLock( const FSSpec& file );
	
	void FSpRename( const FSSpec& item, ConstStr255Param newName );
	
	template < unsigned char n >
	inline void FSpRename( const FSSpec& item, const Str< n >& newName )
	{
		FSpRename( item, newName + 0 );
	}
	
	template < class String >
	inline void FSpRename( const FSSpec& item, const String& newName )
	{
		FSpRename( item, Str255( iota::get_string_data( newName ),
		                         iota::get_string_size( newName ) ) );
	}
	
	// dest is the directory to move source *into*, not the actual new location of source.
	void FSpCatMove( const FSSpec& source, const FSSpec& dest );
	
	inline void FSpCatMove( const FSSpec& source, const FSDirSpec& dest )
	{
		FSpCatMove( source, FSMakeFSSpec( dest ) );
	}
	
	inline void FSpCatMove( const FSSpec& source, FSDirID dest )
	{
		FSpCatMove( source, FSMakeFSSpec( FSVolumeRefNum( source.vRefNum ), dest ) );
	}
	
	void FSpExchangeFiles( const FSSpec& a, const FSSpec& b );
	
	// ...
	
   // 5414
   FSRef FSpMakeFSRef( const FSSpec& );
  }

namespace nucleus
  {
   template <> struct converter< FSRef, FSSpec >
     {
       typedef FSSpec  argument_type;
       typedef FSRef   result_type;
       
       FSRef operator()( const FSSpec& spec ) const
        {
         return Nitrogen::FSpMakeFSRef( spec );
        }
     };
  }

namespace Nitrogen
  {
   FSRef FSMakeFSRefUnicode( const FSRef&   parentRef,
                             UniCharCount   nameLength,
                             const UniChar *name,
                             TextEncoding   textEncodingHint );

   inline FSRef FSMakeFSRefUnicode( const FSRef&        parentRef,
                                    const HFSUniStr255& name,
                                    TextEncoding        textEncodingHint )
     {
      return FSMakeFSRefUnicode( parentRef, name.length, name.unicode, textEncodingHint );
     }

   FSRef FSMakeFSRefUnicode( const FSRef&     parentRef,
                             const UniString& name,
                             TextEncoding     textEncodingHint );
   
   bool FSCompareFSRefs( const FSRef& ref1, const FSRef& ref2 );


   struct FSRefSpec
     {
      FSRef fsRef;
      FSSpec fsSpec;
      
      operator const FSRef&() const   { return fsRef; }
      operator const FSSpec&() const  { return fsSpec; }
     };

   struct FSRefSpecDirID: public FSRefSpec
     {
      FSDirID dirID;

      operator FSDirID() const          { return dirID; }
     };

   class FileSystemDisposer
     {
      public:
         void operator()( const FSRef& ) const;
         void operator()( const FSSpec& ) const;
         void operator()( const FSRefSpec& r ) const          { operator()( r.fsRef ); }
     };
   
  }

namespace Nucleus
  {
   inline bool operator==( const ::FSRef& ref1, const ::FSRef& ref2 )   { return Nitrogen::FSCompareFSRefs( ref1, ref2 ); }
   inline bool operator!=( const ::FSRef& ref1, const ::FSRef& ref2 )   { return !( ref1 == ref2 ); }
  }

namespace Nitrogen
  {


   typedef FSRefSpec FSCreateFileUnicode_Result;
   
   // 5588
   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&         parentRef,
                                                   UniCharCount         nameLength,
                                                   const UniChar *      name,
                                                   FSCatalogInfoBitmap  whichInfo,
                                                   const FSCatalogInfo& catalogInfo );

   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&   parentRef,
                                                   UniCharCount   nameLength,
                                                   const UniChar *name );

   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&         parentRef,
                                                   const UniString&     name,
                                                   FSCatalogInfoBitmap  whichInfo,
                                                   const FSCatalogInfo& catalogInfo );

   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&     parentRef,
                                                   const UniString& name );
   
   typedef FSRefSpecDirID FSCreateDirectoryUnicode_Result;
   
   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&         parentRef,
                                                             UniCharCount         nameLength,
                                                             const UniChar *      name,
                                                             FSCatalogInfoBitmap  whichInfo,
                                                             const FSCatalogInfo& catalogInfo );

   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&   parentRef,
                                                             UniCharCount   nameLength,
                                                             const UniChar *name );

   inline FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&        parentRef,
                                                                    const HFSUniStr255& name )
     {
      return FSCreateDirectoryUnicode( parentRef, name.length, name.unicode );
     }
   
   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&         parentRef,
                                                             const UniString&     name,
                                                             FSCatalogInfoBitmap  whichInfo,
                                                             const FSCatalogInfo& catalogInfo );

   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&     parentRef,
                                                             const UniString& name );

   void FSDeleteObject( const FSRef& ref );

   void FSMoveObject( FSRef& ref, const FSRef& destDirectory );

   void FSExchangeObjects( const FSRef& ref, const FSRef& destRef );
   
   void FSRenameUnicode( FSRef&         ref,
                         UniCharCount   nameLength,
                         const UniChar *name,
                         TextEncoding   textEncodingHint );

   void FSRenameUnicode( FSRef&            ref,
                         const UniString&  name,
                         TextEncoding      textEncodingHint );

   struct FSGetCatalogInfo_Result
     {
      FSCatalogInfo catalogInfo;
      HFSUniStr255  outName;
      FSSpec        fsSpec;
      FSRef         parentRef;
     };
   
   struct ForkSizes
     {
      UInt64 logicalSize;
      UInt64 physicalSize;
     };
   
   union FinderInfo
     {
      FileInfo   fileInfo;
      FolderInfo folderInfo;
      UInt8      finderInfo[16];
      
      FinderInfo()                                     { fileInfo = FileInfo(); }
      FinderInfo( const FileInfo& info )               { fileInfo = info; }
      FinderInfo( const FolderInfo& info )             { folderInfo = info; }
         
      FinderInfo& operator=( const FileInfo& info )    { fileInfo = info;   return *this; } 
      FinderInfo& operator=( const FolderInfo& info )  { folderInfo = info; return *this; }
      
      operator const FileInfo&() const                 { return fileInfo; }
      operator const FolderInfo&() const               { return folderInfo; }
      
      operator FileInfo&()                             { return fileInfo; }
      operator FolderInfo&()                           { return folderInfo; }
     };

   union ExtFinderInfo
     {
      ExtendedFileInfo   extFileInfo;
      ExtendedFolderInfo extFolderInfo;
      UInt8              extFinderInfo[16];

      ExtFinderInfo()                                             { extFileInfo = ExtendedFileInfo(); }
      ExtFinderInfo( const ExtendedFileInfo& info )               { extFileInfo = info; }
      ExtFinderInfo( const ExtendedFolderInfo& info )             { extFolderInfo = info; }
         
      ExtFinderInfo& operator=( const ExtendedFileInfo& info )    { extFileInfo = info;   return *this; } 
      ExtFinderInfo& operator=( const ExtendedFolderInfo& info )  { extFolderInfo = info; return *this; }
      
      operator const ExtendedFileInfo&() const                    { return extFileInfo; }
      operator const ExtendedFolderInfo&() const                  { return extFolderInfo; }
      
      operator ExtendedFileInfo&()                                { return extFileInfo; }
      operator ExtendedFolderInfo&()                              { return extFolderInfo; }
     };
   
   struct Permissions
     {
      UInt32 permissions[4];
     };
   
#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template < class DesiredType, class MemberType, MemberType FSCatalogInfo::*member >
   struct Basic_FSCatalogInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSCatalogInfo& info )                { return info.*member; }
      static void Set( FSCatalogInfo& info, const Type& value )   { info.*member = value; }
     };

   template < class DesiredType, class MemberType, MemberType FSCatalogInfo::*member >
   struct Const_FSCatalogInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSCatalogInfo& info )                { return info.*member; }
     };

   
   template < UInt64 FSCatalogInfo::*logical, UInt64 FSCatalogInfo::*physical >
   struct ForkSizes_FSCatalogInfoBit_Traits
     {
      typedef ForkSizes Type;
      
      static Type Get( const FSCatalogInfo& info )
        {
         ForkSizes result;
         result.logicalSize  = info.*logical;
         result.physicalSize = info.*physical;
         return result;
        }
     };

   template < class DesiredType,
              class MemberType,
              std::size_t size,
              MemberType (FSCatalogInfo::*infoMember)[size],
              MemberType (DesiredType::*desiredMember)[size] >
   struct Array_FSCatalogInfoBit_Traits
     {
      typedef DesiredType Type;
      
      static Type Get( const FSCatalogInfo& info )
        {
         DesiredType result;
         for ( std::size_t i = 0; i < size; ++i )
            (result.*desiredMember)[i] = (info.*infoMember)[i];
         return result;
        }
      
      static void Set( FSCatalogInfo& info, const Type& value )
        {
         for ( std::size_t i = 0; i < size; ++i )
            (info.*infoMember)[i] = (value.*desiredMember)[i];
        }
     };
   
   
   template < ::FSCatalogInfoBitmap bit > struct FSCatalogInfoBit_Traits;
   
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoTextEncoding > : Basic_FSCatalogInfoBit_Traits< TextEncoding,     ::TextEncoding,   &FSCatalogInfo::textEncodingHint > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoNodeFlags    > : Basic_FSCatalogInfoBit_Traits< FSNodeFlags,      ::UInt16,         &FSCatalogInfo::nodeFlags        > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoVolume       > : Const_FSCatalogInfoBit_Traits< FSVolumeRefNum,   ::FSVolumeRefNum, &FSCatalogInfo::volume           > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoParentDirID  > : Const_FSCatalogInfoBit_Traits< FSDirID,          ::UInt32,         &FSCatalogInfo::parentDirID      > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoNodeID       > : Const_FSCatalogInfoBit_Traits< FSNodeID,         ::UInt32,         &FSCatalogInfo::nodeID           > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoCreateDate   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::createDate       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoContentMod   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::contentModDate   > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoAttrMod      > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::attributeModDate > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoAccessDate   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::accessDate       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoBackupDate   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::backupDate       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoValence      > : Const_FSCatalogInfoBit_Traits< UInt32,           ::UInt32,         &FSCatalogInfo::valence          > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoSharingFlags > : Const_FSCatalogInfoBit_Traits< FSSharingFlags,   ::UInt8,          &FSCatalogInfo::sharingFlags     > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoUserPrivs    > : Const_FSCatalogInfoBit_Traits< FSUserPrivileges, ::UInt8,          &FSCatalogInfo::userPrivileges   > {};

   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoDataSizes > : ForkSizes_FSCatalogInfoBit_Traits< &FSCatalogInfo::dataLogicalSize, &FSCatalogInfo::dataPhysicalSize > {}; 
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoRsrcSizes > : ForkSizes_FSCatalogInfoBit_Traits< &FSCatalogInfo::rsrcLogicalSize, &FSCatalogInfo::rsrcPhysicalSize > {};

   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoPermissions  > : Array_FSCatalogInfoBit_Traits< Permissions,   ::UInt32, 4, &FSCatalogInfo::permissions,   &Permissions::permissions     > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoFinderInfo   > : Array_FSCatalogInfoBit_Traits< FinderInfo,    ::UInt8, 16, &FSCatalogInfo::finderInfo,    &FinderInfo::finderInfo       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoFinderXInfo  > : Array_FSCatalogInfoBit_Traits< ExtFinderInfo, ::UInt8, 16, &FSCatalogInfo::extFinderInfo, &ExtFinderInfo::extFinderInfo > {};
#endif

   void FSGetCatalogInfo( const FSRef&        ref,
                          FSCatalogInfoBitmap whichInfo,
                          FSCatalogInfo *     catalogInfo,
                          HFSUniStr255 *      outName,
                          FSSpec *            fsSpec,
                          FSRef *             parentRef );
   
   FSGetCatalogInfo_Result FSGetCatalogInfo( const FSRef& ref, FSCatalogInfoBitmap whichInfo );
   
#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template< ::FSCatalogInfoBitmap bit >
   struct FSGetCatalogInfo_Traits
     {
      typedef typename FSCatalogInfoBit_Traits<bit>::Type Result;
     };
   
   template< ::FSCatalogInfoBitmap bit >
   inline typename FSGetCatalogInfo_Traits<bit>::Result FSGetCatalogInfo( const FSRef& ref )
     {
      FSCatalogInfo info;
      FSGetCatalogInfo( ref, bit, &info, 0, 0, 0 );
      return FSCatalogInfoBit_Traits<bit>::Get( info );
     }
#endif

   void FSSetCatalogInfo( const FSRef&         ref,
                          FSCatalogInfoBitmap  whichInfo,
                          const FSCatalogInfo& catalogInfo );
   
  }

namespace nucleus
{   
   template <> struct converter< FSSpec, FSRef >
     {
      typedef FSRef   argument_type;
      typedef FSSpec  result_type;
      
      FSSpec operator()( const FSRef& ref ) const
        {
         return Nitrogen::FSMakeFSSpec( ref );
        }
     };
	
	template <> struct converter< Nitrogen::FSDirSpec, FSRef >
	{
		typedef FSRef                argument_type;
		typedef Nitrogen::FSDirSpec  result_type;
		
		Nitrogen::FSDirSpec operator()( const FSRef& ref ) const
		{
			return Nitrogen::FSpMake_FSDirSpec( Nitrogen::FSMakeFSSpec( ref ) );
		}
	};
}

namespace Nitrogen
  {

#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template< ::FSCatalogInfoBitmap bit >
   struct FSSetCatalogInfo_Traits
     {
      typedef const typename FSCatalogInfoBit_Traits<bit>::Type& ParameterType;
     };

   template< ::FSCatalogInfoBitmap bit >
   inline void FSSetCatalogInfo( const FSRef& ref, typename FSSetCatalogInfo_Traits<bit>::ParameterType value )
     {
      FSCatalogInfo info; 
      FSCatalogInfoBit_Traits<bit>::Set( info, value );
      FSSetCatalogInfo( ref, bit, info );
     }
#endif

   using ::FSIterator;
  }

namespace nucleus
  {
   template <> struct disposer< FSIterator >
     {
      typedef FSIterator  argument_type;
      typedef void        result_type;
      
      void operator()( FSIterator iterator ) const
        {
         NUCLEUS_REQUIRE_ERRORS( Nitrogen::FileManager );
         
         ::Nitrogen::HandleDestructionOSStatus( ::FSCloseIterator( iterator ) );
        }
     };
  }

namespace Nitrogen
  {

   nucleus::owned<FSIterator> FSOpenIterator( const FSRef& container, FSIteratorFlags iteratorFlags );
    
   void FSCloseIterator( nucleus::owned<FSIterator> );

#if 0
   template < class Specifics >
   class FSIteratedContainer: private Specifics
     {
      private:
          FSRef root;
         
         // not implemented:
            FSIteratedContainer& operator=( const FSIteratedContainer& );

      public:
         typedef typename Specifics::value_type value_type;
         typedef typename Specifics::size_type size_type;
         typedef typename Specifics::difference_type difference_type;

         typedef value_type& reference;
         typedef const value_type& const_reference;
         
         class const_iterator
           {
            friend class FSIteratedContainer;
 
            public:
               typedef IndexUntilFailureContainer::difference_type difference_type;
               typedef IndexUntilFailureContainer::value_type value_type;
               typedef const value_type *pointer;
               typedef const value_type& reference;
               typedef std::input_iterator_tag iterator_category;
           
            private:
               Shared<FSIterator> fsIterator;
               value_type value;
               
               void GetValue()
                 {
                  FSGetCatalogInfoBulk_Result infoResult;
                  do
                     infoResult = Specifics::GetValue( fsIterator, &value );
                  while ( infoResult.actualObjects == 0 && !infoResult.noMoreItems );
                  if ( infoResult.noMoreItems )
                     fsIterator = Shared<FSIterator>();
                 }
               
               const_iterator( const FSRef& root, FSIteratorFlags flags )
                 : fsIterator( FSOpenIterator( root, flags ) )
                 {
                  GetValue();
                 }
               
            public:
               const_iterator()                          {}
               
               const_iterator& operator++()              { GetValue(); return *this; }
               const_iterator operator++(int)            { GetValue(); return *this; }
               
               reference operator*() const               { return value; }
               pointer operator->() const                { return &value; }
               
               friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.fsIterator == b.fsIterator; }
               friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
           };
      
         FSIteratedContainer( const Specifics& base )
           : Specifics( base )
           {}
         
         const_iterator begin() const                    { return const_iterator( *this, Specifics::begin_position() ); }
         const_iterator end() const                      { return const_iterator(); }
     };
#endif
   
   struct FSGetCatalogInfoBulk_Result
     {
      ItemCount actualObjects;
      bool      noMoreItems;
     };
   
   class FSContainterChanged {};
   
   FSGetCatalogInfoBulk_Result FSGetCatalogInfoBulk( FSIterator          iterator,
                                                     ItemCount           maximumObjects,
                                                     FSCatalogInfoBitmap whichInfo,
                                                     FSCatalogInfo *     catalogInfos,
                                                     FSRef *             refs = 0,
                                                     FSSpec *            specs = 0,
                                                     HFSUniStr255 *      names = 0 );
   
#ifndef __LAMP__
   
   void FSGetCatalogInfoBulk( FSIterator                  iterator,
                              FSCatalogInfoBitmap         whichInfo,
                              std::vector<FSCatalogInfo> *catalogInfos,
                              std::vector<FSRef>         *refs = 0,
                              std::vector<FSSpec>        *specs = 0,
                              std::vector<HFSUniStr255>  *names = 0 );

#endif

#if 0  
   template < ::FSCatalogInfoBitmap whichInfo >
   class FSCatalogInfo_ContainerSpecifics
     {
      public:
         typedef FSCatalogInfo value_type;
         typedef FSCatalogIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef ErrFSNoMoreItems end_of_enumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetCatalogInfo( position, whichInfo );
           }
     };

   template < ::FSCatalogInfoBitmap whichInfo > class FSCatalogInfo_Container;
   template < ::FSCatalogInfoBitmap whichInfo > inline FSCatalogInfo_Container<whichInfo> FSCatalogInfos();
   
   template < ::FSCatalogInfoBitmap whichInfo >
   class FSCatalogInfo_Container: public IndexUntilFailureContainer< FSCatalogInfo_ContainerSpecifics<whichInfo> >
     {
      friend FSCatalogInfo_Container<whichInfo> FSCatalogInfos<whichInfo>();

      private:
         FSCatalogInfo_Container()
           : IndexUntilFailureContainer< FSCatalogInfo_ContainerSpecifics<whichInfo> >( FSCatalogInfo_ContainerSpecifics<whichInfo>() )
           {}
     };
   
   template < ::FSCatalogInfoBitmap whichInfo >
   inline FSCatalogInfo_Container<whichInfo> FSCatalogInfos()
     {
      return FSCatalogInfo_Container<whichInfo>();
     }
   
   
   template < ::FSCatalogInfoBitmap whichInfo >
   CatalogInfo_Container<whichInfo> CatalogInfos( const FSRef& container );
   CatalogRef_Container  CatalogRefs( const FSRef& container );
   CatalogSpec_Container CatalogSpecs( const FSRef& container );
   CatalogName_Container CatalogNames( const FSRef& container );
#endif

   typedef FSGetCatalogInfoBulk_Result FSCatalogSearch_Result;

   FSCatalogSearch_Result FSCatalogSearch( FSIterator            iterator,
                                           const FSSearchParams& searchCriteria,
                                           ItemCount             maximumObjects,
                                           FSCatalogInfoBitmap   whichInfo,
                                           FSCatalogInfo *       catalogInfos,
                                           FSRef *               refs,
                                           FSSpec *              specs,
                                           HFSUniStr255 *        names );

   class FSForkRef
     {
      private:
         FSRef file;
         UniString name;
      
      public:
         FSForkRef()
           : file(),
             name()
           {}
         
         FSForkRef( const FSRef& theFile, const UniString& theName )
           : file( theFile ),
             name( theName )
           {}
         
         FSForkRef( const FSRef& f, UniCharCount nameLength, const UniChar *theName )
           : file( f ),
             name( theName, theName+nameLength )
           {}
         
         const FSRef& File() const            { return file; }
         const UniString& Name() const    { return name; }
     };
   
   using Nucleus::operator==;
   
   inline bool operator==( const FSForkRef& a, const FSForkRef& b )
     {
      //using Nucleus::operator==;
      return a.Name() == b.Name() && a.File() == b.File();
     }
   
   inline bool operator!=( const FSForkRef& a, const FSForkRef& b )
     {
      return !( a == b );
     }
   
  }

namespace nucleus
  {
   template <> struct disposer< Nitrogen::FSForkRef >
     {
      typedef Nitrogen::FSForkRef  argument_type;
      typedef void                 result_type;
      
      void operator()( const Nitrogen::FSForkRef& fork ) const
        {
         NUCLEUS_REQUIRE_ERRORS( Nitrogen::FileManager );
         
         ::Nitrogen::HandleDestructionOSStatus( ::FSDeleteFork( &fork.File(),
                                                                fork.Name().size(),
                                                                fork.Name().data() ) );
        }
     };
  }

namespace Nitrogen
  {

   FSForkRef FSCreateFork( const FSRef&   ref,
                           UniCharCount   forkNameLength,
                           const UniChar *forkName );

   FSForkRef FSCreateFork( const FSRef&     ref,
                           const UniString& forkName );

   void FSDeleteFork( const FSForkRef& fork );
   
   struct FSIterateForks_Result
     {
      HFSUniStr255 forkName;
      UInt64       forkSize;
      UInt64       forkPhysicalSize;
     };
   
   FSIterateForks_Result FSIterateForks( const FSRef&    ref,
                                         CatPositionRec& forkIterator );
   
  }

namespace nucleus
  {
   template <> struct disposer< Nitrogen::FSForkRefNum >
     {
      typedef Nitrogen::FSForkRefNum  argument_type;
      typedef void                    result_type;
      
      void operator()( Nitrogen::FSForkRefNum fork ) const
        {
         NUCLEUS_REQUIRE_ERRORS( Nitrogen::FileManager );
         
         ::Nitrogen::HandleDestructionOSStatus( ::FSCloseFork( fork ) );
        }
     };
  }

namespace Nitrogen
  {
   
   nucleus::owned<FSForkRefNum> FSOpenFork( const FSRef&   ref,
                                            UniCharCount   forkNameLength,
                                            const UniChar *forkName,
                                            FSIOPermssn    permissions );

   nucleus::owned<FSForkRefNum> FSOpenFork( const FSRef&    ref,
                                            const UniString forkName,
                                            FSIOPermssn     permissions );

   nucleus::owned<FSForkRefNum> FSOpenFork( const FSForkRef& fork,
                                            FSIOPermssn      permissions );
	
	ByteCount FSReadFork( FSForkRefNum    fork,
	                      FSIOPosMode     positionMode,
	                      SInt64          positionOffset,
	                      ByteCount       requestCount,
	                      void *          buffer,
	                      ThrowEOF_Never  policy );
	
	ByteCount FSReadFork( FSForkRefNum fork,
	                      FSIOPosMode  positionMode,
	                      SInt64       positionOffset,
	                      ByteCount    requestCount,
	                      void *       buffer );
	
	inline ByteCount FSReadFork( FSForkRefNum    fork,
	                             ByteCount       requestCount,
	                             void *          buffer,
	                             ThrowEOF_Never  policy )
	{
		return FSReadFork( fork, fsAtMark, 0, requestCount, buffer, policy );
	}
	
	inline ByteCount FSReadFork( FSForkRefNum  fork,
	                             ByteCount     requestCount,
	                             void *        buffer )
	{
		return FSReadFork( fork, fsAtMark, 0, requestCount, buffer );
	}
	
   template < class Element, std::size_t count >
   inline ByteCount FSReadFork( FSForkRefNum fork,
                                FSIOPosMode  positionMode,
                                SInt64       positionOffset,
                                Element      (&buffer)[count] )
     {
      return FSReadFork( fork, positionMode, positionOffset, count*sizeof(Element), buffer );
     }

   template < class Element, std::size_t count >
   inline ByteCount FSReadFork( FSForkRefNum fork,
                                Element      (&buffer)[count] )
     {
      return FSReadFork( fork, count*sizeof(Element), buffer );
     }

   ByteCount FSWriteFork( FSForkRefNum fork,
                          FSIOPosMode  positionMode,
                          SInt64       positionOffset,
                          ByteCount    requestCount,
                          const void * buffer );

   inline ByteCount FSWriteFork( FSForkRefNum fork,
                                 ByteCount    requestCount,
                                 const void * buffer )
     {
      return FSWriteFork( fork, fsAtMark, 0, requestCount, buffer );
     }

   template < class Element, std::size_t count >
   inline ByteCount FSWriteFork( FSForkRefNum  fork,
                                 FSIOPosMode   positionMode,
                                 SInt64        positionOffset,
                                 const Element (&buffer)[count] )
     {
      return FSWriteFork( fork, positionMode, positionOffset, count*sizeof(Element), buffer );
     }

   template < class Element, std::size_t count >
   inline ByteCount FSWriteFork( FSForkRefNum  fork,
                                 const Element (&buffer)[count] )
     {
      return FSWriteFork( fork, count*sizeof(Element), buffer );
     }
   
   SInt64 FSGetForkPosition( FSForkRefNum forkRefNum );
   
   void FSSetForkPosition( FSForkRefNum forkRefNum,
                           FSIOPosMode  positionMode,
                           SInt64       positionOffset );
   
   SInt64 FSGetForkSize( FSForkRefNum forkRefNum );

   void FSSetForkSize( FSForkRefNum forkRefNum,
                       FSIOPosMode  positionMode,
                       SInt64       positionOffset );
   
   UInt64 FSAllocateFork( FSForkRefNum      forkRefNum,
                          FSAllocationFlags flags,
                          FSIOPosMode       positionMode,
                          SInt64            positionOffset,
                          UInt64            requestCount );
   
   void FSFlushFork( FSForkRefNum forkRefNum );
   
   void FSCloseFork( nucleus::owned<FSForkRefNum> forkRefNum );
   
   struct FSGetForkCBInfo_Result
     {
      FSForkRefNum  actualRefNum;
      FSForkInfo    forkInfo;
      FSRef         ref;
      HFSUniStr255  outForkName;
     };
   
   FSGetForkCBInfo_Result FSGetForkCBInfo( FSForkRefNum desiredRefNum );

   FSGetForkCBInfo_Result FSGetForkCBInfo( FSVolumeRefNum  volume,
                                           FSForkIterator& iterator );

   FSGetForkCBInfo_Result FSGetForkCBInfo( FSForkIterator& iterator );


#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template < class DesiredType, class MemberType, MemberType FSVolumeInfo::*member >
   struct Basic_FSVolumeInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSVolumeInfo& info )                { return info.*member; }
      static void Set( FSVolumeInfo& info, const Type& value )   { info.*member = value; }
     };

   template < class DesiredType, class MemberType, MemberType FSVolumeInfo::*member >
   struct Const_FSVolumeInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSVolumeInfo& info )                { return info.*member; }
     };
   
   template < ::FSVolumeInfoBitmap bit > struct FSVolumeInfoBit_Traits;
   
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoCreateDate  > : Const_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::createDate     > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoModDate     > : Const_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::modifyDate     > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoBackupDate  > : Basic_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::backupDate     > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoCheckedDate > : Const_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::checkedDate    > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFileCount   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::fileCount      > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoDirCount    > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::folderCount    > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoNextAlloc   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::nextAllocation > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoRsrcClump   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::rsrcClumpSize  > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoDataClump   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::dataClumpSize  > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoNextID      > : Const_FSVolumeInfoBit_Traits< HFSCatalogNodeID,  ::UInt32,        &FSVolumeInfo::nextCatalogID  > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFlags       > : Basic_FSVolumeInfoBit_Traits< FSVolumeInfoFlags, ::UInt16,        &FSVolumeInfo::flags          > {};
#endif

   struct FSVolumeInfoSizes
     {
      UInt64 totalBytes;
      UInt64 freeBytes;
     };

#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoSizes >
     {
      typedef FSVolumeInfoSizes Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoSizes result;
         result.totalBytes = info.totalBytes;
         result.freeBytes = info.freeBytes;
         return result;
        }
     };
#endif

   struct FSVolumeInfoBlocks
     {
      UInt32 blockSize;
      UInt32 totalBlocks;
      UInt32 freeBlocks;
     };

#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoBlocks >
     {
      typedef FSVolumeInfoBlocks Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoBlocks result;
         result.blockSize = info.blockSize;
         result.totalBlocks = info.totalBlocks;
         result.freeBlocks = info.freeBlocks;
         return result;
        }
     };
#endif
   
   struct FSVolumeInfoFinderInfo
     {
      UInt8 finderInfo[32];
     };
   
#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFinderInfo >
     {
      typedef FSVolumeInfoFinderInfo Type;
      
      template < class Element, std::size_t size >
      static void ArrayAssign( Element (&destination)[size], const Element (&source)[size] )
        {
         std::copy( source, source + size, destination );
        }

      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoFinderInfo result;
         ArrayAssign( result.finderInfo, info.finderInfo );
         return result;
        }

      static void Set( FSVolumeInfo& info, const FSVolumeInfoFinderInfo& value )
        {
         ArrayAssign( info.finderInfo, value.finderInfo );
        }      
     };
#endif
   
   struct FSVolumeInfoFSInfo
     {
      FSFileSystemID filesystemID;
      UInt16 signature;
     };

#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFSInfo >
     {
      typedef FSVolumeInfoFSInfo Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoFSInfo result;
         result.filesystemID = FSFileSystemID( info.filesystemID );
         result.signature = info.signature;
         return result;
        }
     };
#endif
   
   struct FSVolumeInfoDriveInfo
     {
      UInt16 driveNumber;
      DriverReferenceNumber driverRefNum;
     };

#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoDriveInfo >
     {
      typedef FSVolumeInfoDriveInfo Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoDriveInfo result;
         result.driveNumber = info.driveNumber;
         result.driverRefNum = DriverReferenceNumber( info.driverRefNum );
         return result;
        }
     };
#endif


   struct FSGetVolumeInfo_Result
     {
      FSVolumeRefNum actualVolume;
      FSVolumeInfo   info;
      HFSUniStr255   volumeName;
      FSRef          rootDirectory;
     };

   void FSGetVolumeInfo( FSVolumeRefNum     volume,
                         FSVolumeIndex      volumeIndex,
                         FSVolumeRefNum *   actualVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory );

   void FSGetVolumeInfo( FSVolumeRefNum     volume,
                         FSVolumeRefNum *   actualVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory );

   void FSGetVolumeInfo( FSVolumeIndex      volumeIndex,
                         FSVolumeRefNum *   actualVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory );
   
   FSGetVolumeInfo_Result FSGetVolumeInfo( FSVolumeRefNum volume,
                                           FSVolumeInfoBitmap whichInfo );
   
   FSGetVolumeInfo_Result FSGetVolumeInfo( FSVolumeIndex volumeIndex,
                                           FSVolumeInfoBitmap whichInfo);

#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template< ::FSVolumeInfoBitmap bit >
   struct FSGetVolumeInfo_Traits
     {
      typedef typename FSVolumeInfoBit_Traits<bit>::Type Result;
     };
   
   template< ::FSVolumeInfoBitmap bit >
   inline typename FSGetVolumeInfo_Traits<bit>::Result FSGetVolumeInfo( FSVolumeRefNum volume )
     {
      FSVolumeInfo info;
      FSGetVolumeInfo( volume, 0, bit, &info, 0, 0 );
      return FSVolumeInfoBit_Traits<bit>::Get( info );
     }
   
   template< ::FSVolumeInfoBitmap bit >
   inline typename FSGetVolumeInfo_Traits<bit>::Result FSGetVolumeInfo( FSVolumeIndex volume )
     {
      FSVolumeInfo info;
      FSGetVolumeInfo( volume, 0, bit, &info, 0, 0 );
      return FSVolumeInfoBit_Traits<bit>::Get( info );
     }
#endif
     
   FSVolumeRefNum FSGetVolumeRefNum( FSVolumeIndex volumeIndex );
   
   HFSUniStr255 FSGetVolumeName( FSVolumeRefNum volume );
   HFSUniStr255 FSGetVolumeName( FSVolumeIndex volumeIndex );
   
   FSRef FSGetVolumeRootDirectory( FSVolumeRefNum volume );
   FSRef FSGetVolumeRootDirectory( FSVolumeIndex volumeIndex );


   class Volume_ContainerSpecifics : public OSStatus_EndOfEnumeration< nsvErr >
     {
      public:
         typedef FSVolumeRefNum value_type;
         typedef ItemCount size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         value_type get_value( size_type position )
           {
            return FSGetVolumeRefNum( FSVolumeIndex( position ) );
           }
     };

   class Volume_Container : public nucleus::index_until_error_sequence< Volume_ContainerSpecifics >
     {
      friend Volume_Container Volumes();
      private:
         Volume_Container()
           : nucleus::index_until_error_sequence< ::Nitrogen::Volume_ContainerSpecifics >( ::Nitrogen::Volume_ContainerSpecifics() )
           {}
     };

   inline Volume_Container Volumes()
     {
      return Volume_Container();
     }

   typedef Volume_Container FSVolumeRefNum_Container;
   
   inline FSVolumeRefNum_Container VolumeRefNums()
     {
      return Volumes();
     }


   typedef FSGetVolumeInfo_Result VolumeInfo;

   template < ::FSVolumeInfoBitmap whichInfo >
   class VolumeInfo_ContainerSpecifics : public OSStatus_EndOfEnumeration< nsvErr >
     {
      public:
         typedef VolumeInfo value_type;
         typedef ItemCount size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         value_type get_value( size_type position )
           {
            return FSGetVolumeInfo( FSVolumeIndex( position ), whichInfo );
           }
     };

   template < ::FSVolumeInfoBitmap whichInfo > class VolumeInfo_Container;
   template < ::FSVolumeInfoBitmap whichInfo > inline VolumeInfo_Container<whichInfo> VolumeInfos();
   
   template < ::FSVolumeInfoBitmap whichInfo >
   class VolumeInfo_Container : public nucleus::index_until_error_sequence< VolumeInfo_ContainerSpecifics<whichInfo> >
     {
      friend VolumeInfo_Container<whichInfo> VolumeInfos<whichInfo>();

      private:
         VolumeInfo_Container()
           : nucleus::index_until_error_sequence< VolumeInfo_ContainerSpecifics<whichInfo> >( VolumeInfo_ContainerSpecifics<whichInfo>() )
           {}
     };
   
   template < ::FSVolumeInfoBitmap whichInfo >
   inline VolumeInfo_Container<whichInfo> VolumeInfos()
     {
      return VolumeInfo_Container<whichInfo>();
     }

   class VolumeName_ContainerSpecifics : public OSStatus_EndOfEnumeration< nsvErr >
     {
      public:
         typedef HFSUniStr255 value_type;
         typedef ItemCount size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         value_type get_value( size_type position )
           {
            return FSGetVolumeName( FSVolumeIndex( position ) );
           }
     };

   class VolumeName_Container : public nucleus::index_until_error_sequence< VolumeName_ContainerSpecifics >
     {
      friend VolumeName_Container VolumeNames();
      private:
         VolumeName_Container()
           : nucleus::index_until_error_sequence< ::Nitrogen::VolumeName_ContainerSpecifics >( ::Nitrogen::VolumeName_ContainerSpecifics() )
           {}
     };

   inline VolumeName_Container VolumeNames()
     {
      return VolumeName_Container();
     }

   class VolumeRootDirectory_ContainerSpecifics : public OSStatus_EndOfEnumeration< nsvErr >
     {
      public:
         typedef FSRef value_type;
         typedef ItemCount size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         value_type get_value( size_type position )
           {
            return FSGetVolumeRootDirectory( FSVolumeIndex( position ) );
           }
     };

   class VolumeRootDirectory_Container : public nucleus::index_until_error_sequence< VolumeRootDirectory_ContainerSpecifics >
     {
      friend VolumeRootDirectory_Container VolumeRootDirectories();
      private:
         VolumeRootDirectory_Container()
           : nucleus::index_until_error_sequence< ::Nitrogen::VolumeRootDirectory_ContainerSpecifics >( ::Nitrogen::VolumeRootDirectory_ContainerSpecifics() )
           {}
     };

   inline VolumeRootDirectory_Container VolumeRootDirectories()
     {
      return VolumeRootDirectory_Container();
     }


   void FSSetVolumeInfo( FSVolumeRefNum volume,
                         FSVolumeInfoBitmap  whichInfo,
                         const FSVolumeInfo& volumeInfo );
   
#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template< ::FSVolumeInfoBitmap bit >
   struct FSSetVolumeInfo_Traits
     {
      typedef const typename FSVolumeInfoBit_Traits<bit>::Type& ParameterType;
     };

   template< ::FSVolumeInfoBitmap bit >
   inline void FSSetVolumeInfo( FSVolumeRefNum volume, typename FSSetVolumeInfo_Traits<bit>::ParameterType value )
     {
      FSVolumeInfo info;
      FSVolumeInfoBit_Traits<bit>::Set( info, value );
      FSSetVolumeInfo( volume, bit, info );
     }
#endif
   
   
   HFSUniStr255 FSGetDataForkName();
   
   HFSUniStr255 FSGetResourceForkName();
   
   void FSRefMakePath( const FSRef& ref,
                       UInt8 *      path,
                       UInt32       maxPathSize );

   template < UInt32 size >
   inline void FSRefMakePath( const FSRef& ref,
                              UInt8 (&path)[size] )
     {
      FSRefMakePath( ref, path, size );
     }

   nucleus::string FSRefMakePath( const FSRef& ref );
   
   struct FSPathMakeRef_Result
     {
      FSRef ref;
      bool isDirectory;
      
      operator const FSRef&() const  { return ref; }
     };
   
   FSPathMakeRef_Result FSPathMakeRef( const UInt8 *path );
	
	template < class String >
	inline FSPathMakeRef_Result FSPathMakeRef( const String& path )
	{
		return FSPathMakeRef( reinterpret_cast< const UInt8* >( iota::get_string_c_str( path ) ) );
	}
	
}

namespace nucleus
  {   
   template < class String > struct converter< FSRef, String >
     {
      typedef String  argument_type;
      typedef FSRef   result_type;
      
      FSRef operator()( const String& path ) const
        {
         return Nitrogen::FSPathMakeRef( path );
        }
     };

   template <> struct converter< nucleus::string, FSRef >
     {
      typedef FSRef            argument_type;
      typedef nucleus::string  result_type;
      
      nucleus::string operator()( const FSRef& ref ) const
        {
         return Nitrogen::FSRefMakePath( ref );
        }
     };
  }

namespace Nitrogen
{
	
	template < class VolumeIter >
	FSSpec DTGetAPPL( OSType signature, VolumeIter begin, VolumeIter end )
	{
		for ( ;  begin != end;  ++begin )
		{
			FSVolumeRefNum vRefNum = *begin;
			
			try
			{
				FSSpec appl = DTGetAPPL( signature, vRefNum );  // Succeeds or throws
				
				CInfoPBRec pb;
				
				const bool exists = FSpGetCatInfo( appl, pb, FNF_Returns() );
				
				if ( exists )
				{
					return appl;
				}
			}
			catch ( ... )
			{
				
			}
		}
		
		ThrowOSStatus( afpItemNotFound );
		
		// Not reached
		return FSSpec();
	}
	
	inline FSSpec DTGetAPPL( OSType signature )
	{
		return DTGetAPPL( signature, Volumes().begin(), Volumes().end() );
	}
	
}

#endif

