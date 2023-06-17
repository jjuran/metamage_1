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
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_INITIALIZE_HH
#include "nucleus/initialize.hh"
#endif

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// iota
#include "iota/string_traits.hh"

// Nitrogen
#ifndef MAC_DEVICES_TYPES_DRIVERREFNUM_HH
#include "Mac/Devices/Types/DriverRefNum.hh"
#endif
#ifndef MAC_FILES_FUNCTIONS_FLUSHVOL_HH
#include "Mac/Files/Functions/FlushVol.hh"
#endif
#ifndef MAC_FILES_FUNCTIONS_UNMOUNTVOL_HH
#include "Mac/Files/Functions/UnmountVol.hh"
#endif
#ifndef MAC_FILES_TYPES_FSCREATOR_HH
#include "Mac/Files/Types/FSCreator.hh"
#endif
#ifndef MAC_FILES_TYPES_FSDIRID_HH
#include "Mac/Files/Types/FSDirID.hh"
#endif
#ifndef MAC_FILES_TYPES_FSDIRSPEC_HH
#include "Mac/Files/Types/FSDirSpec.hh"
#endif
#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#include "Mac/Files/Types/FSFileRefNum.hh"
#endif
#ifndef MAC_FILES_TYPES_FSIOPERM_HH
#include "Mac/Files/Types/FSIOPerm.hh"
#endif
#ifndef MAC_FILES_TYPES_FSIOPOSMODE_HH
#include "Mac/Files/Types/FSIOPosMode.hh"
#endif
#ifndef MAC_FILES_TYPES_FSREFNAMESPEC_HH
#include "Mac/Files/Types/FSRefNameSpec.hh"
#endif
#ifndef MAC_FILES_TYPES_FSSHARINGFLAGS_HH
#include "Mac/Files/Types/FSSharingFlags.hh"
#endif
#ifndef MAC_FILES_TYPES_FSSIGNATURE_HH
#include "Mac/Files/Types/FSSignature.hh"
#endif
#ifndef MAC_FILES_TYPES_FSTYPE_HH
#include "Mac/Files/Types/FSType.hh"
#endif
#ifndef MAC_FILES_TYPES_FSUSERPRIVILEGES_HH
#include "Mac/Files/Types/FSUserPrivileges.hh"
#endif
#ifndef MAC_FILES_TYPES_FSVOLUMEREFNUM_HH
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#endif
#ifndef MAC_SCRIPT_TYPES_SCRIPTCODE_HH
#include "Mac/Script/Types/ScriptCode.hh"
#endif

#ifndef NITROGEN_CFSTRING_HH
#include "Nitrogen/CFString.hh"
#endif
#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
#endif

#ifndef NITROGEN_DEVICES_HH
#include "Nitrogen/Devices.hh"
#endif
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif
#ifndef NITROGEN_UPP_HH
#include "Nitrogen/UPP.hh"
#endif

#ifndef __RELIX__
#include <vector>
#endif


#ifdef __MWERKS__
#if __MWERKS__ <= 0x2401
#define NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS 1
#endif
#endif


#undef PBGetCatInfo

#if ! __LP64__
	
inline OSErr PBGetCatInfo( CInfoPBRec* pb, Boolean async )
{
	return async ? PBGetCatInfoAsync( pb )
	             : PBGetCatInfoSync ( pb );
}

#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( FileManager );
	
	
	using Mac::FSSignature;
	
	typedef FSSignature FileSignature;
	
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
	
	using Mac::FSSharingFlags;
	
	typedef FSSharingFlags FSIOFileAttributes;
	typedef FSSharingFlags FSIOFlAttrib;
	
	using Mac::FSUserPrivileges;
	
	typedef FSUserPrivileges FSIOACUser;
	
	enum FSIteratorFlags
	{
		kFSIteratorFlags_Max = nucleus::enumeration_traits< ::FSIteratorFlags >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSIteratorFlags )
	
	using Mac::FSFileRefNum;
	
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
	
	using Mac::FSIOPosMode;
	using Mac::FSIOPositioningMode;
	
	using Mac::fsAtMark;
	using Mac::fsFromStart;
	using Mac::fsFromLEOF;
	using Mac::fsFromMark;
	
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
	
	using Mac::DriverRefNum;
	
	typedef DriverRefNum DRefNum;
	typedef DriverRefNum DriverReferenceNumber;
	
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
	
}

namespace Nitrogen
{
	
	// Trivial PBFoo() wrappers
	
#if ! __LP64__
	
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
	
#endif  // #if ! __LP64__
	
	using Mac::FSDirSpec;
	
}

namespace nucleus
{
	
#if ! __LP64__
	
	template <>
	struct initializer< CInfoPBRec >
	{
		CInfoPBRec& operator()( CInfoPBRec&  pb,
		                        SInt16       vRefNum,
		                        SInt32       dirID,
		                        StringPtr    name,
		                        SInt16       index );
	};
	
	#endif
	
}

namespace Nitrogen
{
	
	// PBHSetVolSync
	
	// ...
	
	struct FNF_Throws
	{
		typedef void Result;
		
		static void HandleOSStatus( ::OSStatus err )
		{
			Mac::ThrowOSStatus( err );
		}
	};
	
	struct FNF_Returns
	{
		typedef bool Result;
		
		static bool HandleOSStatus( ::OSStatus err )
		{
			if ( err == fnfErr )
			{
				return false;
			}
			
			Mac::ThrowOSStatus( err );
			
			return true;
		}
	};
	
	
#if ! __LP64__
	
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
	
	nucleus::owned< FSFileRefNum > HOpen( Mac::FSVolumeRefNum   vRefNum,
	                                      Mac::FSDirID          dirID,
	                                      const unsigned char*  name,
	                                      Mac::FSIOPerm         perm );
	
	inline
	nucleus::owned< FSFileRefNum > HOpen( const FSSpec&  spec,
	                                      Mac::FSIOPerm  perm )
	{
		return HOpen( Mac::FSVolumeRefNum( spec.vRefNum ),
		              Mac::FSDirID       ( spec.parID   ),
		              spec.name,
		              perm );
	}
	
	
	nucleus::owned< FSFileRefNum > HOpenRF( Mac::FSVolumeRefNum   vRefNum,
	                                        Mac::FSDirID          dirID,
	                                        const unsigned char*  name,
	                                        Mac::FSIOPerm         perm );
	
	inline
	nucleus::owned< FSFileRefNum > HOpenRF( const FSSpec&  spec,
	                                        Mac::FSIOPerm  perm )
	{
		return HOpenRF( Mac::FSVolumeRefNum( spec.vRefNum ),
		                Mac::FSDirID       ( spec.parID   ),
		                spec.name,
		                perm );
	}
	
	// ...
	
	void HCreate( const FSSpec&   file,
	              Mac::FSCreator  creator,
	              Mac::FSType     type );
	
	inline
	void HCreate( const FSSpec&  file, const FileSignature& signature )
	{
		HCreate( file, signature.creator, signature.type );
	}
	
	Mac::FSDirID DirCreate( const FSSpec& loc );
	
	void HDelete( const FSSpec& file );
	
	FInfo HGetFInfo( const FSSpec& file );
	
	void HSetFInfo( const FSSpec& file, const FInfo& info );
	
	void HSetFLock( const FSSpec& file );
	void HRstFLock( const FSSpec& file );
	
	void HRename( const FSSpec& file, const unsigned char* name );
	
	template < unsigned char n >
	inline
	void HRename( const FSSpec& item, const Str< n >& newName )
	{
		HRename( item, newName + 0 );
	}
	
	template < class String >
	inline
	void HRename( const FSSpec& item, const String& newName )
	{
		using iota::get_string_data;
		using iota::get_string_size;
		
		HRename( item, Str255( get_string_data( newName ),
		                       get_string_size( newName ) ) );
	}
	
	void CatMove( const FSSpec& file, const FSSpec& new_parent );
	
	// ...
	
	void PBHGetVolParmsSync( HParamBlockRec& paramBlock );
	
	GetVolParmsInfoBuffer PBHGetVolParmsSync( FSVolumeRefNum vRefNum );
	
	FSSpec FSMakeFSSpec( FSVolumeRefNum    vRefNum,
	                     FSDirID           dirID,
	                     ConstStr255Param  name = NULL );
	
	inline FSSpec FSMakeFSSpec( const FSDirSpec&  dir,
	                            ConstStr255Param  name = NULL )
	{
		return FSMakeFSSpec( dir.vRefNum, dir.dirID, name );
	}
	
	FSSpec FSMakeFSSpec( const FSRef& ref );
	
#endif  // #if ! __LP64__
	
}

namespace nucleus
{
	
#if ! __LP64__
	
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
	
#endif  // #if ! __LP64__
	
}

namespace Nitrogen
{
	
#if ! __LP64__
	
	FSSpec FSMakeFSSpec( ConstStr255Param pathname );
	
	bool FSCompareFSSpecs( const FSSpec& a, const FSSpec& b );
	
	
	nucleus::owned< FSFileRefNum > FSpOpenDF( const FSSpec&   spec,
	                                          FSIOPermssn     permissions );
	
	
	nucleus::owned< FSFileRefNum > FSpOpenRF( const FSSpec&   spec,
	                                          FSIOPermssn     permissions );
	
	
	FSSpec FSpCreate( const FSSpec&    file,
	                  Mac::FSCreator   creator,
	                  Mac::FSType      type,
	                  Mac::ScriptCode  scriptTag = Mac::smSystemScript );
	
	inline FSSpec FSpCreate( const FSSpec&         file,
	                         const FileSignature&  signature,
	                         Mac::ScriptCode       scriptTag = Mac::smSystemScript )
	{
		return FSpCreate( file, signature.creator, signature.type, scriptTag );
	}
	
	FSDirSpec FSpDirCreate( const FSSpec&    dir,
	                        Mac::ScriptCode  scriptTag = Mac::smSystemScript );
	
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
		using iota::get_string_data;
		using iota::get_string_size;
		
		FSpRename( item, Str255( get_string_data( newName ),
		                         get_string_size( newName ) ) );
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
	
#endif  // #if ! __LP64__
	
}

namespace nucleus
{
	
#if ! __LP64__
	
	template <> struct converter< FSRef, FSSpec >
	{
		typedef FSSpec  argument_type;
		typedef FSRef   result_type;
		
		FSRef operator()( const FSSpec& spec ) const
		{
			return Nitrogen::FSpMakeFSRef( spec );
		}
	};
	
#endif  // #if ! __LP64__
	
}

namespace Nitrogen
{
	
	using Mac::FSRefNameSpec;
	
	
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
	
	inline FSRef FSMakeFSRefUnicode( const FSRefNameSpec&  file,
	                                 TextEncoding          textEncodingHint )
	{
		return FSMakeFSRefUnicode( file.parent, file.name, textEncodingHint );
	}
	
	template < class UniString >
	FSRef FSMakeFSRefUnicode( const FSRef&     parentRef,
	                          const UniString& name,
	                          TextEncoding     textEncodingHint )
	{
		return FSMakeFSRefUnicode( parentRef, name.size(), name.data(), textEncodingHint );
	}
	
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

namespace nucleus
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
	
	template < class UniString >
	FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&         parentRef,
	                                                const UniString&     name,
	                                                FSCatalogInfoBitmap  whichInfo,
	                                                const FSCatalogInfo& catalogInfo )
	{
		return FSCreateFileUnicode( parentRef, name.size(), name.data(), whichInfo, catalogInfo );
	}
	
	template < class UniString >
	FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&     parentRef,
	                                                const UniString& name )
	{
		return FSCreateFileUnicode( parentRef, name.size(), name.data() );
	}
	
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
	
	template < class UniString >
	FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&         parentRef,
	                                                          const UniString&     name,
	                                                          FSCatalogInfoBitmap  whichInfo,
	                                                          const FSCatalogInfo& catalogInfo )
	{
		return FSCreateDirectoryUnicode( parentRef, name.size(), name.data(), whichInfo, catalogInfo );
	}
	
	template < class UniString >
	FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&     parentRef,
	                                                          const UniString& name )
	{
		return FSCreateDirectoryUnicode( parentRef, name.size(), name.data() );
	}
	
	void FSDeleteObject( const FSRef& ref );
	
	void FSMoveObject( FSRef& ref, const FSRef& destDirectory );
	
	void FSExchangeObjects( const FSRef& ref, const FSRef& destRef );
	
	void FSRenameUnicode( FSRef&         ref,
	                      UniCharCount   nameLength,
	                      const UniChar *name,
	                      TextEncoding   textEncodingHint );
	
	template < class UniString >
	void FSRenameUnicode( FSRef&            ref,
	                      const UniString&  name,
	                      TextEncoding      textEncodingHint )
	{
		FSRenameUnicode( ref, name.size(), name.data(), textEncodingHint );
	}
	
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

#if ! __LP64__
	
	template <> struct FSCatalogInfoBit_Traits< kFSCatInfoPermissions  > : Array_FSCatalogInfoBit_Traits< Permissions,   ::UInt32, 4, &FSCatalogInfo::permissions,   &Permissions::permissions     > {};
	template <> struct FSCatalogInfoBit_Traits< kFSCatInfoFinderInfo   > : Array_FSCatalogInfoBit_Traits< FinderInfo,    ::UInt8, 16, &FSCatalogInfo::finderInfo,    &FinderInfo::finderInfo       > {};
	template <> struct FSCatalogInfoBit_Traits< kFSCatInfoFinderXInfo  > : Array_FSCatalogInfoBit_Traits< ExtFinderInfo, ::UInt8, 16, &FSCatalogInfo::extFinderInfo, &ExtFinderInfo::extFinderInfo > {};
	
#endif  // #if ! __LP64__
	
#endif
	
	void FSGetCatalogInfo( const FSRef&        ref,
	                       FSCatalogInfoBitmap whichInfo,
	                       FSCatalogInfo *     catalogInfo,
	                       HFSUniStr255 *      outName,
	                       FSSpec *            fsSpec,
	                       FSRef *             parentRef );
	
	FSGetCatalogInfo_Result FSGetCatalogInfo( const FSRef& ref, FSCatalogInfoBitmap whichInfo );
	
#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
	
	template < ::FSCatalogInfoBitmap bit >
	struct FSGetCatalogInfo_Traits
	{
		typedef typename FSCatalogInfoBit_Traits<bit>::Type Result;
	};
	
	template < ::FSCatalogInfoBitmap bit >
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
	
#if ! __LP64__
	
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
	
#endif  // #if ! __LP64__
	
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
			
			(void) ::FSCloseIterator( iterator );
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
	
#ifndef __RELIX__
	
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
	
}

namespace Nitrogen
{
	
	void FSCreateFork( const FSRef&    ref,
	                   UniCharCount    forkNameLength,
	                   const UniChar*  forkName );
	
	inline void FSCreateFork( const FSRef& ref )
	{
		FSCreateFork( ref, 0, NULL );
	}
	
	template < class UniString >
	void FSCreateFork( const FSRef&      ref,
	                   const UniString&  forkName )
	{
		FSCreateFork( ref, forkName.size(), forkName.data() );
	}
	
	void FSDeleteFork( const FSRef&    ref,
	                   UniCharCount    forkNameLength,
	                   const UniChar*  forkName );
	
	inline void FSDeleteFork( const FSRef& ref )
	{
		FSDeleteFork( ref, 0, NULL );
	}
	
	template < class UniString >
	void FSDeleteFork( const FSRef&      ref,
	                   const UniString&  forkName )
	{
		FSDeleteFork( ref, forkName.size(), forkName.data() );
	}
	
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
			
			(void) ::FSCloseFork( fork );
		}
	};
	
}

namespace Nitrogen
{
	
	nucleus::owned<FSForkRefNum> FSOpenFork( const FSRef&   ref,
	                                         UniCharCount   forkNameLength,
	                                         const UniChar *forkName,
	                                         FSIOPermssn    permissions );
	
	inline nucleus::owned< FSForkRefNum > FSOpenFork( const FSRef&  ref,
	                                                  FSIOPerm      perm )
	{
		return FSOpenFork( ref, 0, NULL , perm );
	}
	
	template < class UniString >
	nucleus::owned<FSForkRefNum> FSOpenFork( const FSRef&    ref,
	                                         const UniString forkName,
	                                         FSIOPermssn     permissions )
	{
		return FSOpenFork( ref, forkName.size(), forkName.data(), permissions );
	}
	
	ByteCount FSReadFork( FSForkRefNum     fork,
	                      FSIOPosMode      positionMode,
	                      SInt64           positionOffset,
	                      ByteCount        requestCount,
	                      void *           buffer,
	                      ThrowEOF_Always  policy );
	
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
	
	inline ByteCount FSReadFork( FSForkRefNum     fork,
	                             ByteCount        requestCount,
	                             void *           buffer,
	                             ThrowEOF_Always  policy )
	{
		return FSReadFork( fork, fsAtMark, 0, requestCount, buffer, policy );
	}
	
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
		using iota::get_string_c_str;
		
		return FSPathMakeRef( reinterpret_cast< const UInt8* >( get_string_c_str( path ) ) );
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

#undef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS

#endif
