// Files.h

#ifndef NITROGEN_FILES_H
#define NITROGEN_FILES_H

#ifndef IO_IO_HH
#include "io/io.hh"
#endif

#ifndef NUCLEUS_ARRAYCONTAINERFUNCTIONS_H
#include "Nucleus/ArrayContainerFunctions.h"
#endif
#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif
#ifndef NUCLEUS_FLAGTYPE_H
#include "Nucleus/FlagType.h"
#endif
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_IDTYPE_H
#include "Nucleus/IDType.h"
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
	
	typedef Nucleus::ID< class FSDirID_Tag, UInt32 >::Type FSDirID;
	
	static const FSDirID fsRtParID = FSDirID( ::fsRtParID );
	static const FSDirID fsRtDirID = FSDirID( ::fsRtDirID );
	
   class FSNodeFlagsTag {};
   typedef Nucleus::FlagType< FSNodeFlagsTag, UInt16, 0 > FSNodeFlags;
   
   class FSNodeIDTag {};
   typedef Nucleus::IDType< FSNodeIDTag, UInt32, 0 > FSNodeID;
   
   class FSVolumeRefNumTag {};
   typedef Nucleus::IDType< FSVolumeRefNumTag, ::FSVolumeRefNum, 0 > FSVolumeRefNum;

	typedef Nucleus::Flag< class FSSharingFlags_Tag, UInt8 >::Type FSSharingFlags;
	
	NUCLEUS_DEFINE_FLAG_OPS( FSSharingFlags )
	
   typedef FSSharingFlags FSIOFileAttributes;
   typedef FSSharingFlags FSIOFlAttrib;
   
	typedef Nucleus::Flag< class FSUserPrivileges_Tag, UInt8 >::Type FSUserPrivileges;
	
	NUCLEUS_DEFINE_FLAG_OPS( FSUserPrivileges )
	
   typedef FSUserPrivileges FSIOACUser;

   class FSIteratorFlagsTag {};
   typedef Nucleus::FlagType< FSIteratorFlagsTag, ::FSIteratorFlags, 0 > FSIteratorFlags;
   
	typedef Nucleus::ID< class FSFileRefNum_Tag, SInt16 >::Type FSFileRefNum;
	
   class FSForkRefNumTag {};
   typedef Nucleus::IDType< FSForkRefNumTag, SInt16, 0 > FSForkRefNum;
    
	typedef Nucleus::Flag< class FSIOPermssn_Tag, SInt8 >::Type FSIOPermssn;
	
	NUCLEUS_DEFINE_FLAG_OPS( FSIOPermssn )
	
	typedef FSIOPermssn FSIOPermissions;
	
	static const FSIOPermssn fsCurPerm    = FSIOPermssn( ::fsCurPerm    );
	static const FSIOPermssn fsRdPerm     = FSIOPermssn( ::fsRdPerm     );
	static const FSIOPermssn fsWrPerm     = FSIOPermssn( ::fsWrPerm     );
	static const FSIOPermssn fsRdWrPerm   = FSIOPermssn( ::fsRdWrPerm   );
	static const FSIOPermssn fsRdWrShPerm = FSIOPermssn( ::fsRdWrShPerm );
	static const FSIOPermssn fsRdDenyPerm = FSIOPermssn( ::fsRdDenyPerm );
	static const FSIOPermssn fsWrDenyPerm = FSIOPermssn( ::fsWrDenyPerm );
	
   class FSIOPosModeTag {};
   typedef Nucleus::FlagType< FSIOPosModeTag, UInt16, fsAtMark > FSIOPosMode;
   typedef FSIOPosMode FSIOPositioningMode;
	
   class FSAllocationFlagsTag {};
   typedef Nucleus::FlagType< FSAllocationFlagsTag, ::FSAllocationFlags, 0 > FSAllocationFlags;
  
   class FSForkIteratorTag {};
   typedef Nucleus::IDType< FSForkIteratorTag, SInt16, 0 > FSForkIterator;

   class FSVolumeInfoFlagsTag {};
   typedef Nucleus::FlagType< FSVolumeInfoFlagsTag, UInt16, 0 > FSVolumeInfoFlags;
   typedef FSVolumeInfoFlags FSIOVAtrb;
   typedef FSVolumeInfoFlags FSIOVolumeAttributes;
   
   struct FSVolumeIndex_Specifics
     {
      typedef ::ItemCount UnderlyingType;
      static const UnderlyingType defaultValue = 0;
     };

   typedef Nucleus::IndexType< FSVolumeIndex_Specifics > FSVolumeIndex;
   
   class FSFileSystemIDTag {};
   typedef Nucleus::IDType< FSFileSystemIDTag, UInt16, 0 > FSFileSystemID;
   typedef FSFileSystemID IOFSID;
   
	typedef Nucleus::ID< class DriverReferenceNumber_Tag, SInt16 >::Type DriverReferenceNumber;
	
   typedef DriverReferenceNumber DRefNum;
   typedef DriverReferenceNumber DriverRefNum;

   class HFSCatalogNodeIDTag {};
   typedef Nucleus::IDType< HFSCatalogNodeIDTag, UInt32, 0 > HFSCatalogNodeID;
   
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
	
	SInt32 FSRead( FSFileRefNum  file,
	               SInt32        requestCount,
	               void *        buffer );
	
	template < class Element, std::size_t count >
	SInt32 FSRead( FSFileRefNum  file,
	               Element       (&buffer)[count] )
	{
		return FSRead( file, count * sizeof (Element), buffer );
	}
	
	SInt32 FSWrite( FSFileRefNum  file,
	                SInt32        requestCount,
	                const void *  buffer );
	
	template < class Element, std::size_t count >
	SInt32 FSWrite( FSFileRefNum   file,
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
		FSVolumeRefNum vRefNum;
		FSDirID dirID;
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
	                  ScriptCode     scriptTag = ScriptCode( smSystemScript ) );
	
	FSDirSpec FSpDirCreate( const FSSpec&  dir, 
	                        ScriptCode     scriptTag = ScriptCode( smSystemScript ) );
	
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

   void FSDeleteObject( Nucleus::Owned<FSRef> );

   void FSMoveObject( Nucleus::Owned<FSRef>&, const FSRef& destDirectory );

   void FSExchangeObjects( const FSRef& ref, const FSRef& destRef );
   
   void FSRenameUnicode( Nucleus::Owned<FSRef>&  ref,
                         UniCharCount            nameLength,
                         const UniChar          *name,
                         TextEncoding            textEncodingHint );

   void FSRenameUnicode( Nucleus::Owned<FSRef>&    ref,
                         const UniString&          name,
                         TextEncoding              textEncodingHint );

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
   typename FSGetCatalogInfo_Traits<bit>::Result FSGetCatalogInfo( const FSRef& ref )
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
   void FSSetCatalogInfo( const FSRef& ref, typename FSSetCatalogInfo_Traits<bit>::ParameterType value )
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
   template < ::FSCatalogInfoBitmap whichInfo > FSCatalogInfo_Container<whichInfo> FSCatalogInfos();
   
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
   FSCatalogInfo_Container<whichInfo> FSCatalogInfos()
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

   Nucleus::Owned<FSForkRef> FSCreateFork( const FSRef&   ref,
                                  UniCharCount   forkNameLength,
                                  const UniChar *forkName );

   Nucleus::Owned<FSForkRef> FSCreateFork( const FSRef&     ref,
                                  const UniString& forkName );

   void FSDeleteFork( Nucleus::Owned<FSForkRef> );
   
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
   
   ByteCount FSReadFork( FSForkRefNum fork,
                         FSIOPosMode  positionMode,
                         SInt64       positionOffset,
                         ByteCount    requestCount,
                         void *       buffer );

   inline ByteCount FSReadFork( FSForkRefNum fork,
                                ByteCount    requestCount,
                                void *       buffer )
     {
      return FSReadFork( fork, FSIOPosMode::Make( fsAtMark ), 0, requestCount, buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSReadFork( FSForkRefNum fork,
                         FSIOPosMode  positionMode,
                         SInt64       positionOffset,
                         Element      (&buffer)[count] )
     {
      return FSReadFork( fork, positionMode, positionOffset, count*sizeof(Element), buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSReadFork( FSForkRefNum fork,
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
      return FSWriteFork( fork, FSIOPosMode::Make( fsAtMark ), 0, requestCount, buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSWriteFork( FSForkRefNum  fork,
                          FSIOPosMode   positionMode,
                          SInt64        positionOffset,
                          const Element (&buffer)[count] )
     {
      return FSWriteFork( fork, positionMode, positionOffset, count*sizeof(Element), buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSWriteFork( FSForkRefNum  fork,
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
   typename FSGetVolumeInfo_Traits<bit>::Result FSGetVolumeInfo( FSVolumeRefNum volume )
     {
      FSVolumeInfo info;
      FSGetVolumeInfo( volume, 0, bit, &info, 0, 0 );
      return FSVolumeInfoBit_Traits<bit>::Get( info );
     }
   
   template< ::FSVolumeInfoBitmap bit >
   typename FSGetVolumeInfo_Traits<bit>::Result FSGetVolumeInfo( FSVolumeIndex volume )
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
   template < ::FSVolumeInfoBitmap whichInfo > VolumeInfo_Container<whichInfo> VolumeInfos();
   
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
   VolumeInfo_Container<whichInfo> VolumeInfos()
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
   void FSSetVolumeInfo( FSVolumeRefNum volume, typename FSSetVolumeInfo_Traits<bit>::ParameterType value )
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
   void FSRefMakePath( const FSRef& ref,
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
		
		typedef Nitrogen::FSFileRefNum stream;
		
		typedef SInt32 byte_count;
	};
	
	struct FSRef_Io_Details
	{
		typedef FSRef file_spec;
		
		typedef Nitrogen::FSForkRefNum stream;
		
		typedef SInt64 byte_count;
	};
	
}

namespace io
{
	
	template <> struct filespec_traits< FSSpec > : public Nitrogen::FSSpec_Io_Details {};
	
	inline Nucleus::Owned< Nitrogen::FSFileRefNum > open_for_reading( const FSSpec& file )
	{
		return Nitrogen::FSpOpenDF( file, Nitrogen::fsRdPerm );
	}
	
	inline Nucleus::Owned< Nitrogen::FSFileRefNum > open_for_writing( const FSSpec& file )
	{
		return Nitrogen::FSpOpenDF( file, Nitrogen::fsWrPerm );
	}
	
	inline SInt32 get_file_size( Nitrogen::FSFileRefNum stream )
	{
		return Nitrogen::GetEOF( stream );
	}
	
	inline SInt32 read( Nitrogen::FSFileRefNum input, char* data, SInt32 byteCount )
	{
		return Nitrogen::FSRead( input, byteCount, data );
	}
	
	inline SInt32 write( Nitrogen::FSFileRefNum output, const char* data, SInt32 byteCount )
	{
		return Nitrogen::FSWrite( output, byteCount, data );
	}
	
	
	template <> struct filespec_traits< FSRef  > : public Nitrogen::FSRef_Io_Details {};
	
	inline Nucleus::Owned< Nitrogen::FSForkRefNum > open_for_reading( const FSRef& file )
	{
		return Nitrogen::FSOpenFork( file, Nitrogen::UniString(), Nitrogen::fsRdPerm );
	}
	
	inline Nucleus::Owned< Nitrogen::FSForkRefNum > open_for_writing( const FSRef& file )
	{
		return Nitrogen::FSOpenFork( file, Nitrogen::UniString(), Nitrogen::fsWrPerm );
	}
	
	inline SInt64 get_file_size( Nitrogen::FSForkRefNum stream )
	{
		return Nitrogen::FSGetForkSize( stream );
	}
	
	inline ByteCount read( Nitrogen::FSForkRefNum input, char* data, ByteCount byteCount )
	{
		return Nitrogen::FSReadFork( input, byteCount, data );
	}
	
	inline ByteCount write( Nitrogen::FSForkRefNum output, const char* data, ByteCount byteCount )
	{
		return Nitrogen::FSWriteFork( output, byteCount, data );
	}
	
}

#endif

