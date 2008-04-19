// Nitrogen/Files.h
// ----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2007 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_FILES_H
#define NITROGEN_FILES_H

#ifndef IO_IO_HH
#include "io/io.hh"
#endif
#ifndef IO_FILES_HH
#include "io/files.hh"
#endif

#ifndef NUCLEUS_ARRAYCONTAINERFUNCTIONS_H
#include "Nucleus/ArrayContainerFunctions.h"
#endif
#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_INDEXTYPE_H
#include "Nucleus/IndexType.h"
#endif
#ifndef NUCLEUS_INDEXUNTILFAILURECONTAINER_H
#include "Nucleus/IndexUntilFailureContainer.h"
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __FILES__
#include FRAMEWORK_HEADER(CarbonCore,Files.h)
#endif
#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
// Needed for smSystemScript
#ifndef NITROGEN_SCRIPT_H
#include "Nitrogen/Script.h"
#endif
#ifndef NITROGEN_STR_H
#include "Nitrogen/Str.h"
#endif
#ifndef NITROGEN_TEXTCOMMON_H
#include "Nitrogen/TextCommon.h"
#endif

#include <vector>


namespace Nitrogen
  {
	
	class FileManagerErrorsRegistrationDependency
	{
		public:
			FileManagerErrorsRegistrationDependency();
	};
	
	inline void ThrowFileManagerError( ::OSStatus err )
	{
		if ( err != noErr )
		{
			(void) FileManagerErrorsRegistrationDependency();
			
			ThrowOSStatus( err );
		}
	}
	
	
	struct FileSignature
	{
		OSType creator;
		OSType type;
		
		FileSignature() : creator(), type()  {}
		
		FileSignature( OSType creator,
		               OSType type ) : creator( creator ),
		                               type   ( type    )  {}
		
		FileSignature( const FInfo& fInfo ) : creator( OSType( fInfo.fdCreator ) ),
		                                      type   ( OSType( fInfo.fdType    ) )  {}
	};
	
	
	enum FSDirID
	{
		fsRtParID = ::fsRtParID,
		fsRtDirID = ::fsRtDirID,
		
		kFSDirID_Max = Nucleus::Enumeration_Traits< UInt32 >::max
	};
	
   typedef Nucleus::Flag< class FSNodeFlags_Tag, UInt16 >::Type FSNodeFlags;
   
   NUCLEUS_DEFINE_FLAG_OPS( FSNodeFlags )
   
   typedef Nucleus::ID< class FSNodeID_Tag, UInt32 >::Type FSNodeID;
   
   typedef Nucleus::ID< class FSVolumeRefNum_Tag, ::FSVolumeRefNum >::Type FSVolumeRefNum;

	typedef Nucleus::Flag< class FSSharingFlags_Tag, UInt8 >::Type FSSharingFlags;
	
	NUCLEUS_DEFINE_FLAG_OPS( FSSharingFlags )
	
   typedef FSSharingFlags FSIOFileAttributes;
   typedef FSSharingFlags FSIOFlAttrib;
   
	typedef Nucleus::Flag< class FSUserPrivileges_Tag, UInt8 >::Type FSUserPrivileges;
	
	NUCLEUS_DEFINE_FLAG_OPS( FSUserPrivileges )
	
	typedef FSUserPrivileges FSIOACUser;
	
	typedef Nucleus::Flag< class FSIteratorFlags_Tag, ::FSIteratorFlags >::Type FSIteratorFlags;
	
	NUCLEUS_DEFINE_FLAG_OPS( FSIteratorFlags )
	
	typedef Nucleus::ID< class FSFileRefNum_Tag, SInt16 >::Type FSFileRefNum;
	typedef Nucleus::ID< class FSForkRefNum_Tag, SInt16 >::Type FSForkRefNum;
    
	enum FSIOPermssn
	{
		fsCurPerm    = ::fsCurPerm,
		fsRdPerm     = ::fsRdPerm,
		fsWrPerm     = ::fsWrPerm,
		fsRdWrPerm   = ::fsRdWrPerm,
		fsRdWrShPerm = ::fsRdWrShPerm,
		fsRdDenyPerm = ::fsRdDenyPerm,
		fsWrDenyPerm = ::fsWrDenyPerm,
		
		kFSIOPermssn_Max = Nucleus::Enumeration_Traits< SInt8 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSIOPermssn )
	
	typedef FSIOPermssn FSIOPermissions;
	
	enum FSIOPosMode
	{
		fsAtMark    = ::fsAtMark,
		fsFromStart = ::fsFromStart,
		fsFromLEOF  = ::fsFromLEOF,
		fsFromMark  = ::fsFromMark,
		
		kFSIOPosMode_Max = Nucleus::Enumeration_Traits< UInt16 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSIOPosMode )
	
	typedef FSIOPosMode FSIOPositioningMode;
	
   typedef Nucleus::Flag< class FSAllocationFlags_Tag, ::FSAllocationFlags >::Type FSAllocationFlags;
  
	NUCLEUS_DEFINE_FLAG_OPS( FSAllocationFlags )
	
   typedef Nucleus::ID< class FSForkIterator_Tag, SInt16 >::Type FSForkIterator;

   typedef Nucleus::Flag< class FSVolumeInfoFlags_Tag, UInt16 >::Type FSVolumeInfoFlags;
   typedef FSVolumeInfoFlags FSIOVAtrb;
   typedef FSVolumeInfoFlags FSIOVolumeAttributes;
   
	NUCLEUS_DEFINE_FLAG_OPS( FSVolumeInfoFlags )
	
   struct FSVolumeIndex_Specifics
     {
      typedef ::ItemCount UnderlyingType;
      static const UnderlyingType defaultValue = 0;
     };

   typedef Nucleus::IndexType< FSVolumeIndex_Specifics > FSVolumeIndex;
   
   typedef Nucleus::ID< class FSFileSystemID_Tag, UInt16 >::Type FSFileSystemID;
   typedef FSFileSystemID IOFSID;
   
	typedef Nucleus::ID< class DriverReferenceNumber_Tag, SInt16 >::Type DriverReferenceNumber;
	
   typedef DriverReferenceNumber DRefNum;
   typedef DriverReferenceNumber DriverRefNum;

   typedef Nucleus::ID< class HFSCatalogNodeID_Tag, UInt32 >::Type HFSCatalogNodeID;
   
   using ::HFSUniStr255;
   using ::FSSpec;
   using ::FSRef;
  }

namespace Nucleus
  {
   template <> struct Converter< Nucleus::Owned<Nitrogen::CFStringRef>, Nitrogen::HFSUniStr255 >: public std::unary_function< Nitrogen::HFSUniStr255, Nucleus::Owned<Nitrogen::CFStringRef> >
     {
      Nucleus::Owned<Nitrogen::CFStringRef> operator()( const Nitrogen::HFSUniStr255& in ) const
        {
         return Nitrogen::CFStringCreateWithCharacters( in.unicode, in.length );
        }
     };
   
   template <> struct Converter< Nitrogen::HFSUniStr255, Nitrogen::CFStringRef >: public std::unary_function< Nitrogen::CFStringRef, Nitrogen::HFSUniStr255 >
     {
      Nitrogen::HFSUniStr255 operator()( Nitrogen::CFStringRef in ) const
        {
         CFIndex length = Nitrogen::CFStringGetLength( in );
         if ( length > 255 )
            throw Nitrogen::StringTooLong();
         Nitrogen::HFSUniStr255 result;
         result.length = Convert<UInt16>( length );
         Nitrogen::CFStringGetCharacters( in, CFRangeMake( 0, length ), result.unicode );
         return result;
        }
     };

   template <> struct Converter< Nitrogen::UniString, Nitrogen::HFSUniStr255 >: public std::unary_function< Nitrogen::HFSUniStr255, Nitrogen::UniString >
     {
      Nitrogen::UniString operator()( const Nitrogen::HFSUniStr255& in ) const
        {
         return Nitrogen::UniString( in.unicode, in.unicode+in.length );
        }
     };
  
   template <> struct Converter< Nitrogen::HFSUniStr255, Nitrogen::UniString >: public std::unary_function< Nitrogen::UniString, Nitrogen::HFSUniStr255 >
     {
      Nitrogen::HFSUniStr255 operator()( const Nitrogen::UniString& in ) const
        {
         if ( in.size() > 255 )
            throw Nitrogen::StringTooLong();
         Nitrogen::HFSUniStr255 result;
         result.length = Convert<UInt16>( in.size() );
         std::copy( in.begin(), in.end(), result.unicode );
         return result;
        }
     };

	template <> struct Disposer< Nitrogen::FSFileRefNum > : public std::unary_function< Nitrogen::FSFileRefNum, void >,
	                                                        private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::FSFileRefNum file ) const
		{
			(void) Nitrogen::FileManagerErrorsRegistrationDependency();
			HandleDestructionOSStatus( ::FSClose( file ) );
		}
	};
  }

namespace Nitrogen
  {
	
	void UnmountVol( ConstStr63Param volName = NULL );
	
	void UnmountVol( FSVolumeRefNum vRefNum );
	
	//void UnmountVol( FSDriveIndex drive );
	
	void FlushVol( ConstStr63Param volName = NULL );
	
	void FlushVol( FSVolumeRefNum vRefNum );
	
	//void FlushVol( FSDriveIndex drive );
	
	// HSetVol
	
	void FSClose( Nucleus::Owned< FSFileRefNum > fileRefNum );
	
	struct ThrowOnEOF       {};  // type not used, semantics implied by default
	struct ReturnZeroOnEOF  {};
	
	SInt32 FSRead( FSFileRefNum     file,
	               SInt32           requestCount,
	               void *           buffer,
	               ReturnZeroOnEOF  /* policy */ );
	
	SInt32 FSRead( FSFileRefNum  file,
	               SInt32        requestCount,
	               void *        buffer );
	
	template < class Element, std::size_t count >
	inline SInt32 FSRead( FSFileRefNum  file,
	                      Element       (&buffer)[count] )
	{
		return FSRead( file, count * sizeof (Element), buffer );
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
	
	namespace Detail
	{
		
		struct FSDirSpec_Storage
		{
			FSVolumeRefNum  vRefNum;
			FSDirID         dirID;
			
			FSDirSpec_Storage() : vRefNum(), dirID() {}
		};
		
	}
	
	struct FSDirSpec : public Detail::FSDirSpec_Storage
	{
		FSDirSpec() : FSDirSpec_Storage() {}
		
		FSDirSpec( const Detail::FSDirSpec_Storage& storage ) : FSDirSpec_Storage( storage )  {}
		
		explicit FSDirSpec( const FSSpec& dir );
		
		FSDirSpec& operator/=( const unsigned char* name );
		
		FSDirSpec& operator/=( const std::string& name )
		{
			return *this /= Str63( name );
		}
	};
	
	inline FSDirSpec::FSDirSpec( const FSSpec& dir ) : FSDirSpec_Storage( Nucleus::Convert< FSDirSpec >( dir ) )
	{
	}
	
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

namespace Nucleus
{
	
	template <>
	struct Maker< Nitrogen::FSDirSpec >
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
	
}

namespace Nitrogen
  {
	
	// PBHSetVolSync
	
	// ...
	
	CInfoPBRec& PBGetCatInfoSync( CInfoPBRec& paramBlock );
	
	void PBSetCatInfoSync( CInfoPBRec& cInfo );
	
	// ...
	
	const CInfoPBRec* FSpGetCatInfo( const FSSpec& item );
	
	CInfoPBRec& FSpGetCatInfo( const FSSpec& item, CInfoPBRec& paramBlock );
	
	CInfoPBRec& FSpGetCatInfo( const FSDirSpec&  dir,
	                           CInfoPBRec&       paramBlock,
	                           StringPtr         name        = NULL );
	
	CInfoPBRec& FSpGetCatInfo( const FSDirSpec&  dir,
	                           UInt16            index,
	                           CInfoPBRec&       paramBlock,
	                           StringPtr         name        = NULL );
	
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
	
	void DTSetComment( DTPBRec& pb, const std::string& comment );
	
	void FSpDTSetComment( const FSSpec& file, const std::string& comment );
	
	// PBDTRemoveCommentSync
	
	void PBDTGetCommentSync( DTPBRec& pb );
	
	std::string DTGetComment( DTPBRec& pb );
	
	std::string FSpDTGetComment( const FSSpec& file );
	
	// PBDTFlushSync
	// PBDTResetSync
	// PBDTGetInfoSync
	// PBDTOpenInform
	// PBDTDeleteSync
	
	FSSpec FSMakeFSSpec( FSVolumeRefNum    vRefNum,
	                     FSDirID           dirID,
	                     ConstStr255Param  name );
	
	inline FSSpec FSMakeFSSpec( const FSDirSpec& dir, ConstStr255Param name )
	{
		return FSMakeFSSpec( dir.vRefNum, dir.dirID, name );
	}
	
  }

namespace Nucleus
  {
	template <> struct Converter< Nitrogen::FSSpec, Nitrogen::FSDirSpec >: public std::unary_function< Nitrogen::FSDirSpec, Nitrogen::FSSpec >
	{
		Nitrogen::FSSpec operator()( const Nitrogen::FSDirSpec& dir ) const
		{
			return Nitrogen::FSMakeFSSpec( dir, NULL );
		}
	};
	
	template <> struct Converter< Nitrogen::FSDirSpec, Nitrogen::FSSpec >: public std::unary_function< Nitrogen::FSSpec, Nitrogen::FSDirSpec >
	{
		Nitrogen::FSDirSpec operator()( const Nitrogen::FSSpec& dir ) const;
	};
  }

namespace Nitrogen
  {
	
	FSSpec FSMakeFSSpec( ConstStr255Param pathname );
	
	bool FSCompareFSSpecs( const FSSpec& a, const FSSpec& b );
	
	Nucleus::Owned< FSFileRefNum > FSpOpenDF( const FSSpec&   spec,
	                                          FSIOPermssn     permissions );
	
	Nucleus::Owned< FSFileRefNum > FSpOpenRF( const FSSpec&   spec,
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
	
	inline void FSpRename( const FSSpec& item, const std::string& newName )
	{
		FSpRename( item, Str255( newName ) );
	}
	
	// dest is the directory to move source *into*, not the actual new location of source.
	void FSpCatMove( const FSSpec& source, const FSSpec& dest );
	
	inline void FSpCatMove( const FSSpec& source, FSDirSpec dest )
	{
		FSpCatMove( source, Nucleus::Convert< FSSpec >( dest ) );
	}
	
	inline void FSpCatMove( const FSSpec& source, FSDirID dest )
	{
		FSpCatMove( source, Nucleus::Make< FSDirSpec >( FSVolumeRefNum( source.vRefNum ), dest ) );
	}
	
	void FSpExchangeFiles( const FSSpec& a, const FSSpec& b );
	
	// ...
	
   // 5414
   FSRef FSpMakeFSRef( const FSSpec& );
  }

namespace Nucleus
  {
   template <> struct Converter< Nitrogen::FSRef, Nitrogen::FSSpec >: public std::unary_function< Nitrogen::FSSpec, Nitrogen::FSRef >
     {
      Nitrogen::FSRef operator()( const Nitrogen::FSSpec& spec ) const
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

   class FileSystemDisposer: private DefaultDestructionOSStatusPolicy
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

   template <> struct OwnedDefaults< Nitrogen::FSRef >           { typedef Nitrogen::FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< Nitrogen::FSSpec >          { typedef Nitrogen::FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< Nitrogen::FSDirID >         { typedef Nitrogen::FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< Nitrogen::FSRefSpec >       { typedef Nitrogen::FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< Nitrogen::FSRefSpecDirID >  { typedef Nitrogen::FileSystemDisposer Disposer; };
   
   template <> struct LivelinessTraits< Nitrogen::FSRef,          Nitrogen::FileSystemDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
   template <> struct LivelinessTraits< Nitrogen::FSSpec,         Nitrogen::FileSystemDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
   template <> struct LivelinessTraits< Nitrogen::FSRefSpec,      Nitrogen::FileSystemDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
   template <> struct LivelinessTraits< Nitrogen::FSRefSpecDirID, Nitrogen::FileSystemDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
  }

namespace Nitrogen
  {


   typedef Nucleus::Owned<FSRefSpec> FSCreateFileUnicode_Result;
   
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
   
   typedef Nucleus::Owned<FSRefSpecDirID> FSCreateDirectoryUnicode_Result;
   
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

namespace Nucleus
{   
   template <> struct Converter< Nitrogen::FSSpec, Nitrogen::FSRef >: public std::unary_function< Nitrogen::FSRef, Nitrogen::FSSpec >
     {
      Nitrogen::FSSpec operator()( const Nitrogen::FSRef& ref ) const
        {
         ::FSSpec result;
         Nitrogen::FSGetCatalogInfo( ref, 0, 0, 0, &result, 0 );
         return result;
        }
     };
	
	template <> struct Converter< Nitrogen::FSDirSpec, Nitrogen::FSRef > : public std::unary_function< Nitrogen::FSRef, Nitrogen::FSDirSpec >
	{
		Nitrogen::FSDirSpec operator()( const Nitrogen::FSRef& ref ) const
		{
			return Convert< Nitrogen::FSDirSpec >( Convert< Nitrogen::FSSpec >( ref ) );
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

namespace Nucleus
  {
   template <> struct Disposer<Nitrogen::FSIterator>: public std::unary_function< Nitrogen::FSIterator, void >,
                                                      private Nitrogen::DefaultDestructionOSStatusPolicy
     {
      void operator()( Nitrogen::FSIterator iterator ) const
        {
         (void) Nitrogen::FileManagerErrorsRegistrationDependency();
         HandleDestructionOSStatus( ::FSCloseIterator( iterator ) );
        }
     };
  }

namespace Nitrogen
  {

   Nucleus::Owned<FSIterator> FSOpenIterator( const FSRef& container, FSIteratorFlags iteratorFlags );
    
   void FSCloseIterator( Nucleus::Owned<FSIterator> );

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
   
   void FSGetCatalogInfoBulk( FSIterator                  iterator,
                              FSCatalogInfoBitmap         whichInfo,
                              std::vector<FSCatalogInfo> *catalogInfos,
                              std::vector<FSRef>         *refs = 0,
                              std::vector<FSSpec>        *specs = 0,
                              std::vector<HFSUniStr255>  *names = 0 );
                              
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
         
         typedef ErrFSNoMoreItems EndOfEnumeration;
         
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

namespace Nucleus
  {
   template <> struct Disposer<Nitrogen::FSForkRef>: public std::unary_function< Nitrogen::FSForkRef, void >,
                                                     private Nitrogen::DefaultDestructionOSStatusPolicy
     {
      void operator()( const Nitrogen::FSForkRef& fork ) const
        {
         (void) Nitrogen::FileManagerErrorsRegistrationDependency();
         HandleDestructionOSStatus( ::FSDeleteFork( &fork.File(),
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

namespace Nucleus
  {
   template <> struct Disposer<Nitrogen::FSForkRefNum>: public std::unary_function< Nitrogen::FSForkRefNum, void >,
                                                        private Nitrogen::DefaultDestructionOSStatusPolicy
     {
      void operator()( Nitrogen::FSForkRefNum fork ) const
        {
         (void) Nitrogen::FileManagerErrorsRegistrationDependency();
         HandleDestructionOSStatus( ::FSCloseFork( fork ) );
        }
     };
  }

namespace Nitrogen
  {
   
   Nucleus::Owned<FSForkRefNum> FSOpenFork( const FSRef&   ref,
                                            UniCharCount   forkNameLength,
                                            const UniChar *forkName,
                                            FSIOPermssn    permissions );

   Nucleus::Owned<FSForkRefNum> FSOpenFork( const FSRef&    ref,
                                            const UniString forkName,
                                            FSIOPermssn     permissions );

   Nucleus::Owned<FSForkRefNum> FSOpenFork( const FSForkRef& fork,
                                            FSIOPermssn      permissions );
	
	ByteCount FSReadFork( FSForkRefNum     fork,
	                      FSIOPosMode      positionMode,
	                      SInt64           positionOffset,
	                      ByteCount        requestCount,
	                      void *           buffer,
	                      ReturnZeroOnEOF  /* policy */ );
	
	ByteCount FSReadFork( FSForkRefNum fork,
	                      FSIOPosMode  positionMode,
	                      SInt64       positionOffset,
	                      ByteCount    requestCount,
	                      void *       buffer );
	
	inline ByteCount FSReadFork( FSForkRefNum     fork,
	                             ByteCount        requestCount,
	                             void *           buffer,
	                             ReturnZeroOnEOF  policy )
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
   
   void FSCloseFork( Nucleus::Owned<FSForkRefNum> forkRefNum );
   
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
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoFinderInfo result;
         Nucleus::ArrayAssign( result.finderInfo, info.finderInfo );
         return result;
        }

      static void Set( FSVolumeInfo& info, const FSVolumeInfoFinderInfo& value )
        {
         Nucleus::ArrayAssign( info.finderInfo, value.finderInfo );
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

   class Volume_ContainerSpecifics
     {
      public:
         typedef FSVolumeRefNum value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeRefNum( position );
           }
     };

   class Volume_Container: public Nucleus::IndexUntilFailureContainer< Volume_ContainerSpecifics >
     {
      friend Volume_Container Volumes();
      private:
         Volume_Container()
           : Nucleus::IndexUntilFailureContainer< ::Nitrogen::Volume_ContainerSpecifics >( ::Nitrogen::Volume_ContainerSpecifics() )
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
   class VolumeInfo_ContainerSpecifics
     {
      public:
         typedef VolumeInfo value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeInfo( position, whichInfo );
           }
     };

   template < ::FSVolumeInfoBitmap whichInfo > class VolumeInfo_Container;
   template < ::FSVolumeInfoBitmap whichInfo > inline VolumeInfo_Container<whichInfo> VolumeInfos();
   
   template < ::FSVolumeInfoBitmap whichInfo >
   class VolumeInfo_Container: public Nucleus::IndexUntilFailureContainer< VolumeInfo_ContainerSpecifics<whichInfo> >
     {
      friend VolumeInfo_Container<whichInfo> VolumeInfos<whichInfo>();

      private:
         VolumeInfo_Container()
           : Nucleus::IndexUntilFailureContainer< VolumeInfo_ContainerSpecifics<whichInfo> >( VolumeInfo_ContainerSpecifics<whichInfo>() )
           {}
     };
   
   template < ::FSVolumeInfoBitmap whichInfo >
   inline VolumeInfo_Container<whichInfo> VolumeInfos()
     {
      return VolumeInfo_Container<whichInfo>();
     }

   class VolumeName_ContainerSpecifics
     {
      public:
         typedef HFSUniStr255 value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeName( position );
           }
     };

   class VolumeName_Container: public Nucleus::IndexUntilFailureContainer< VolumeName_ContainerSpecifics >
     {
      friend VolumeName_Container VolumeNames();
      private:
         VolumeName_Container()
           : Nucleus::IndexUntilFailureContainer< ::Nitrogen::VolumeName_ContainerSpecifics >( ::Nitrogen::VolumeName_ContainerSpecifics() )
           {}
     };

   inline VolumeName_Container VolumeNames()
     {
      return VolumeName_Container();
     }

   class VolumeRootDirectory_ContainerSpecifics
     {
      public:
         typedef FSRef value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeRootDirectory( position );
           }
     };

   class VolumeRootDirectory_Container: public Nucleus::IndexUntilFailureContainer< VolumeRootDirectory_ContainerSpecifics >
     {
      friend VolumeRootDirectory_Container VolumeRootDirectories();
      private:
         VolumeRootDirectory_Container()
           : Nucleus::IndexUntilFailureContainer< ::Nitrogen::VolumeRootDirectory_ContainerSpecifics >( ::Nitrogen::VolumeRootDirectory_ContainerSpecifics() )
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

   std::string FSRefMakePath( const FSRef& ref );
   
   struct FSPathMakeRef_Result
     {
      FSRef ref;
      bool isDirectory;
      
      operator const FSRef&() const  { return ref; }
     };
   
   FSPathMakeRef_Result FSPathMakeRef( const UInt8 *path );
   FSPathMakeRef_Result FSPathMakeRef( const std::string& path );
  }

namespace Nucleus
  {   
   template <> struct Converter< Nitrogen::FSRef, std::string >: public std::unary_function< std::string, Nitrogen::FSRef >
     {
      Nitrogen::FSRef operator()( const std::string& path ) const
        {
         return Nitrogen::FSPathMakeRef( path );
        }
     };

   template <> struct Converter< std::string, Nitrogen::FSRef >: public std::unary_function< Nitrogen::FSRef, std::string >
     {
      std::string operator()( const Nitrogen::FSRef& ref ) const
        {
         return Nitrogen::FSRefMakePath( ref );
        }
     };
  }

namespace Nitrogen
{
	
	struct FSSpec_Io_Details
	{
		typedef FSSpec file_spec;
		
		typedef FSDirSpec optimized_directory_spec;
		
		typedef ConstStr63Param filename_parameter;
		
		typedef Nitrogen::Str63 filename_result;
		
		typedef CInfoPBRec file_catalog_record;
		
		typedef Nitrogen::FSFileRefNum stream;
		
		typedef SInt32 byte_count;
		
		typedef SInt32 position_offset;
	};
	
	struct FSRef_Io_Details
	{
		typedef FSRef file_spec;
		
		typedef FSRef optimized_directory_spec;
		
		typedef const HFSUniStr255& filename_parameter;
		
		typedef HFSUniStr255 filename_result;
		
		typedef Nitrogen::FSForkRefNum stream;
		
		typedef ::ByteCount byte_count;
		
		typedef SInt64 position_offset;
	};
	
}

namespace io
{
	
	template <> struct filespec_traits< FSSpec              > : public Nitrogen::FSSpec_Io_Details {};
	template <> struct filespec_traits< Nitrogen::FSDirSpec > : public Nitrogen::FSSpec_Io_Details {};
	
	// Get file info
	
	inline Nitrogen::Str63 get_filename( const FSSpec& file, overload = overload() )
	{
		return file.name;
	}
	
	inline Nitrogen::Str63 get_filename( const Nitrogen::FSDirSpec& dir, overload = overload() )
	{
		return get_filename( Nucleus::Convert< FSSpec >( dir ) );
	}
	
	inline Nitrogen::FSDirSpec get_preceding_directory( const FSSpec& file, overload = overload() )
	{
		return Nucleus::Make< Nitrogen::FSDirSpec >( Nitrogen::FSVolumeRefNum( file.vRefNum ),
		                                             Nitrogen::FSDirID       ( file.parID   ) );
	}
	
	inline Nitrogen::FSDirSpec get_preceding_directory( const Nitrogen::FSDirSpec& dir, overload = overload() )
	{
		return get_preceding_directory( Nucleus::Convert< FSSpec >( dir ) );
	}
	
	inline Nitrogen::FSDirSpec get_parent_directory_of_directory( const Nitrogen::FSDirSpec& dir, overload = overload() )
	{
		return get_preceding_directory( dir );
	}
	
	// Path descent
	
	namespace path_descent_operators
	{
		
		inline FSSpec operator/( const Nitrogen::FSDirSpec& dir, const unsigned char* name )
		{
			return Nitrogen::FSMakeFSSpec( dir, name );
		}
		
		inline FSSpec operator/( const FSSpec& dir, const unsigned char* name )
		{
			return Nucleus::Convert< Nitrogen::FSDirSpec >( dir ) / name;
		}
		
		inline FSSpec operator/( const Nitrogen::FSDirSpec& dir, const std::string& name )
		{
			return dir / Nitrogen::Str63( name );
		}
		
		inline FSSpec operator/( const FSSpec& dir, const std::string& name )
		{
			return dir / Nitrogen::Str63( name );
		}
		
	}
	
	// Existence
	
	inline bool item_exists     ( const Nitrogen::FSDirSpec& dir, overload = overload() )  { return true; }
	inline bool file_exists     ( const Nitrogen::FSDirSpec& dir, overload = overload() )  { return false; }
	inline bool directory_exists( const Nitrogen::FSDirSpec& dir, overload = overload() )  { return true; }
	
	inline bool item_exists( const FSSpec& item, overload )
	{
		return Nitrogen::FSpGetCatInfo( item ) != NULL;
	}
	
	inline bool item_is_directory( const HFileInfo& hFileInfo, overload = overload() )
	{
		return hFileInfo.ioFlAttrib & kioFlAttribDirMask;
	}
	
	inline bool item_is_directory( const CInfoPBRec& cInfo, overload = overload() )
	{
		return item_is_directory( cInfo.hFileInfo );
	}
	
	inline bool item_is_file( const CInfoPBRec& cInfo, overload = overload() )
	{
		return !item_is_directory( cInfo );
	}
	
	inline bool file_exists( const FSSpec& file, overload = overload() )
	{
		const CInfoPBRec* cInfo = Nitrogen::FSpGetCatInfo( file );
		
		return cInfo != NULL  &&  item_is_file( *cInfo );
	}
	
	inline bool directory_exists( const FSSpec& dir, overload = overload() )
	{
		const CInfoPBRec* cInfo = Nitrogen::FSpGetCatInfo( dir );
		
		return cInfo != NULL  &&  item_is_directory( *cInfo );
	}
	
	// Delete
	
	inline void delete_file( const FSSpec& file, overload = overload() )
	{
		Nitrogen::FSpDelete( file );
	}
	
	inline void delete_file_only( const FSSpec& file, overload = overload() )
	{
		if ( directory_exists( file ) )
		{
			throw Nitrogen::NotAFileErr();
		}
		
		delete_file( file );
	}
	
	inline void delete_empty_directory( const FSSpec& dir, overload = overload() )
	{
		Nitrogen::FSpDelete( dir );
	}
	
	inline void delete_empty_directory( const Nitrogen::FSDirSpec& dir, overload = overload() )
	{
		delete_empty_directory( Nucleus::Convert< FSSpec >( dir ) );
	}
	
	inline void delete_empty_directory_only( const FSSpec& dir, overload = overload() )
	{
		if ( file_exists( dir ) )
		{
			throw Nitrogen::DirNFErr();
		}
		
		delete_empty_directory( dir );
	}
	
	inline void delete_empty_directory_only( const Nitrogen::FSDirSpec& dir, overload = overload() )
	{
		delete_empty_directory( dir );
	}
	
	// Open
	
	inline Nucleus::Owned< Nitrogen::FSFileRefNum > open_for_reading( const FSSpec& file, overload = overload() )
	{
		return Nitrogen::FSpOpenDF( file, Nitrogen::fsRdPerm );
	}
	
	inline Nucleus::Owned< Nitrogen::FSFileRefNum > open_for_writing( const FSSpec& file, overload = overload() )
	{
		return Nitrogen::FSpOpenDF( file, Nitrogen::fsWrPerm );
	}
	
	inline Nucleus::Owned< Nitrogen::FSFileRefNum > open_for_io( const FSSpec& file, overload = overload() )
	{
		return Nitrogen::FSpOpenDF( file, Nitrogen::fsRdWrPerm );
	}
	
	// Stream operations
	
	inline SInt32 get_file_size( Nitrogen::FSFileRefNum stream, overload = overload() )
	{
		return Nitrogen::GetEOF( stream );
	}
	
	template < class ByteCount >
	inline SInt32 read( Nitrogen::FSFileRefNum input, char* data, ByteCount byteCount, overload = overload() )
	{
		return Nitrogen::FSRead( input, byteCount, data, Nitrogen::ReturnZeroOnEOF() );
	}
	
	template < class ByteCount >
	inline SInt32 write( Nitrogen::FSFileRefNum output, const char* data, ByteCount byteCount, overload = overload() )
	{
		return Nitrogen::FSWrite( output, byteCount, data );
	}
	
	
	template <> struct filespec_traits< FSRef  > : public Nitrogen::FSRef_Io_Details {};
	
	// Get file info
	
	inline HFSUniStr255 get_filename( const FSRef& file, overload = overload() )
	{
		FSCatalogInfo info;
		
		HFSUniStr255 name;
		
		Nitrogen::FSGetCatalogInfo( file, kFSCatInfoNone, &info, &name, NULL, NULL );
		
		return name;
	}
	
	inline FSRef get_preceding_directory( const FSRef& file, overload = overload() )
	{
		FSCatalogInfo info;
		
		FSRef parent;
		
		Nitrogen::FSGetCatalogInfo( file, kFSCatInfoNone, &info, NULL, NULL, &parent );
		
		return parent;
	}
	
	inline FSRef get_parent_directory_of_directory( const FSRef& dir, overload = overload() )
	{
		return get_preceding_directory( dir );
	}
	
	// Path descent
	
	namespace path_descent_operators
	{
		
		inline FSRef operator/( const FSRef& dir, const HFSUniStr255& name )
		{
			return Nitrogen::FSMakeFSRefUnicode( dir, name, Nitrogen::TextEncoding( kTextEncodingUnknown ) );
		}
		
	}
	
	// Existence
	
	inline bool item_exists( const FSRef& item, overload = overload() )  { return true; }
	
	inline bool directory_exists( const FSRef& dir, overload = overload() )
	{
		FSCatalogInfo info;
		
		Nitrogen::FSGetCatalogInfo( dir, kFSCatInfoNodeFlags, &info, NULL, NULL, NULL );
		
		return info.nodeFlags & kFSNodeIsDirectoryMask;
	}
	
	inline bool file_exists( const FSRef& file, overload = overload() )
	{
		return !directory_exists( file );
	}
	
	// Delete
	
	inline void delete_file( const FSRef& file, overload = overload() )
	{
		Nitrogen::FSDeleteObject( file );
	}
	
	inline void delete_file_only( const FSRef& file, overload = overload() )
	{
		if ( directory_exists( file ) )
		{
			throw Nitrogen::NotAFileErr();
		}
		
		delete_file( file );
	}
	
	inline void delete_empty_directory( const FSRef& dir, overload = overload() )
	{
		Nitrogen::FSDeleteObject( dir );
	}
	
	inline void delete_empty_directory_only( const FSRef& dir, overload = overload() )
	{
		if ( file_exists( dir ) )
		{
			throw Nitrogen::DirNFErr();
		}
		
		delete_empty_directory( dir );
	}
	
	// Open
	
	inline Nucleus::Owned< Nitrogen::FSForkRefNum > open_for_reading( const FSRef& file, overload = overload() )
	{
		return Nitrogen::FSOpenFork( file, Nitrogen::UniString(), Nitrogen::fsRdPerm );
	}
	
	inline Nucleus::Owned< Nitrogen::FSForkRefNum > open_for_writing( const FSRef& file, overload = overload() )
	{
		return Nitrogen::FSOpenFork( file, Nitrogen::UniString(), Nitrogen::fsWrPerm );
	}
	
	inline Nucleus::Owned< Nitrogen::FSForkRefNum > open_for_io( const FSRef& file, overload = overload() )
	{
		return Nitrogen::FSOpenFork( file, Nitrogen::UniString(), Nitrogen::fsRdWrPerm );
	}
	
	// Stream operations
	
	inline SInt64 get_file_size( Nitrogen::FSForkRefNum stream, overload = overload() )
	{
		return Nitrogen::FSGetForkSize( stream );
	}
	
	template < class ByteCount >
	inline ByteCount read( Nitrogen::FSForkRefNum input, char* data, ByteCount byteCount, overload = overload() )
	{
		return Nitrogen::FSReadFork( input, byteCount, data, Nitrogen::ReturnZeroOnEOF() );
	}
	
	template < class ByteCount >
	inline ByteCount write( Nitrogen::FSForkRefNum output, const char* data, ByteCount byteCount, overload = overload() )
	{
		return Nitrogen::FSWriteFork( output, byteCount, data );
	}
	
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
				
				if ( io::file_exists( appl ) )
				{
					return appl;
				}
			}
			catch ( ... )
			{
				
			}
		}
		
		throw AFPItemNotFound();
	}
	
	inline FSSpec DTGetAPPL( OSType signature )
	{
		return DTGetAPPL( signature, Volumes().begin(), Volumes().end() );
	}
	
	inline FSDirSpec& FSDirSpec::operator/=( const unsigned char* name )
	{
		using namespace io::path_descent_operators;
		
		return *this = FSDirSpec( *this / name );
	}
	
}

#endif

