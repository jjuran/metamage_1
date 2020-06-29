// Nitrogen/Files.cc
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/Files.hh"

// Mac OS
#ifndef __STRINGCOMPARE__
#include <StringCompare.h>
#endif

// Standard C++
#include <limits>

// Debug
#include "debug/assert.hh"


#ifndef MAC_OS_X_VERSION_10_5
typedef SInt16 FSIORefNum;
#endif

namespace nucleus
{
	
#if ! __LP64__
	
	CInfoPBRec&
	//
	initializer< CInfoPBRec >::operator()( CInfoPBRec&  pb,
	                                       SInt16       vRefNum,
	                                       SInt32       dirID,
	                                       StringPtr    name,
	                                       SInt16       index )
	{
		DirInfo& dirInfo = pb.dirInfo;
		
		dirInfo.ioNamePtr = name;
		dirInfo.ioVRefNum = vRefNum;
		dirInfo.ioDrDirID = dirID;
		dirInfo.ioFDirIndex = index;
		
		return pb;
	}
	
#endif  // #if ! __LP64__
	
}

namespace Nitrogen
{
	
	using Mac::ThrowOSStatus;
	
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( FileManager )
	
	
	static void RegisterFileManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class FileManagerErrorsRegistration
	{
		public:
			FileManagerErrorsRegistration()  { RegisterFileManagerErrors(); }
	};
	
	static FileManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
#if ! __LP64__
	
	static inline OSErr FixedAsyncResult( OSErr err, const HParamBlockRec& pb )
	{
		if ( TARGET_CPU_68K )
		{
			// Only File Manager uses HParamBlockRec, so no check needed
			return pb.ioParam.ioResult;
		}
		
		return err;
	}
	
	static inline OSErr FixedAsyncResult( OSErr err, const CInfoPBRec& pb )
	{
		if ( TARGET_CPU_68K )
		{
			// Only File Manager uses HParamBlockRec, so no check needed
			return pb.hFileInfo.ioResult;
		}
		
		return err;
	}
	
	
	void PBHOpenDFSync( HParamBlockRec& pb )
	{
		ThrowOSStatus( ::PBHOpenDFSync( &pb ) );
	}
	
	void PBHOpenDFAsync( HParamBlockRec& pb )
	{
		ThrowOSStatus( FixedAsyncResult( ::PBHOpenDFAsync( &pb ), pb ) );
	}
	
	void PBHOpenRFSync( HParamBlockRec& pb )
	{
		ThrowOSStatus( ::PBHOpenRFSync( &pb ) );
	}
	
	void PBHOpenRFAsync( HParamBlockRec& pb )
	{
		ThrowOSStatus( FixedAsyncResult( ::PBHOpenRFAsync( &pb ), pb ) );
	}
	
	
	void FSClose( nucleus::owned< FSFileRefNum > fileRefNum )
	{
		ThrowOSStatus( ::FSClose( fileRefNum.release() ) );
	}
	
	SInt32 FSWrite( FSFileRefNum file,
	                SInt32       requestCount,
	                const void * buffer )
	{
		SInt32 actualCount = requestCount;
		ThrowOSStatus( ::FSWrite( file,
	                             &actualCount,
	                             buffer ) );
		return actualCount;
	}
	
	SInt32 Allocate( FSFileRefNum      fileRefNum,
	                 SInt32            requestCount )
	{
		SInt32 actualCount = requestCount;
		ThrowOSStatus( ::Allocate( fileRefNum,
	                              &actualCount ) );
		return actualCount;
	}
	
	SInt32 GetEOF( FSFileRefNum fileRefNum )
	{
		SInt32 position;
		ThrowOSStatus( ::GetEOF( fileRefNum, &position ) );
		return position;
	}
	
	void SetEOF( FSFileRefNum fileRefNum,
	             SInt32       positionOffset )
	{
		ThrowOSStatus( ::SetEOF( fileRefNum, positionOffset ) );
	}
	
	SInt32 GetFPos( FSFileRefNum fileRefNum )
	{
		SInt32 position;
		ThrowOSStatus( ::GetFPos( fileRefNum, &position ) );
		return position;
	}
	
	void SetFPos( FSFileRefNum  fileRefNum,
	              FSIOPosMode   positionMode,
	              SInt32        positionOffset )
	{
		ThrowOSStatus( ::SetFPos( fileRefNum, positionMode, positionOffset ) );
	}
	
	
	void PBGetCatInfoSync( CInfoPBRec& pb, FNF_Throws policy )
	{
		FNF_Throws::HandleOSStatus( ::PBGetCatInfoSync( &pb ) );
	}
	
	bool PBGetCatInfoSync( CInfoPBRec& pb, FNF_Returns policy )
	{
		return FNF_Returns::HandleOSStatus( ::PBGetCatInfoSync( &pb ) );
	}
	
	
	void PBGetCatInfoAsync( CInfoPBRec& pb, FNF_Throws policy )
	{
		policy.HandleOSStatus( FixedAsyncResult( ::PBGetCatInfoAsync( &pb ), pb ) );
	}
	
	bool PBGetCatInfoAsync( CInfoPBRec& pb, FNF_Returns policy )
	{
		return policy.HandleOSStatus( FixedAsyncResult( ::PBGetCatInfoAsync( &pb ), pb ) );
	}
	
	
	void PBSetCatInfoSync( CInfoPBRec& pb )
	{
		ThrowOSStatus( ::PBSetCatInfoSync( &pb ) );
	}
	
	void PBSetCatInfoAsync( CInfoPBRec& pb )
	{
		ThrowOSStatus( ::PBSetCatInfoAsync( &pb ) );
	}
	
	FSDirSpec FSpMake_FSDirSpec( const FSSpec& dir )
	{
		CInfoPBRec cInfo;
		FSpGetCatInfo( dir, cInfo );
		
		const bool is_dir = cInfo.hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		if ( !is_dir )
		{
			// I wanted a dir but you gave me a file.  You creep.
			ThrowOSStatus( errFSNotAFolder );
		}
		
		FSDirID dirID = FSDirID( cInfo.dirInfo.ioDrDirID );
		
		return nucleus::make< FSDirSpec >( FSVolumeRefNum( dir.vRefNum ), dirID );
	}
	
	
	nucleus::owned< FSFileRefNum > HOpen( Mac::FSVolumeRefNum   vRefNum,
	                                      Mac::FSDirID          dirID,
	                                      const unsigned char*  name,
	                                      Mac::FSIOPerm         perm )
	{
		SInt16 result;
		ThrowOSStatus( ::HOpen( vRefNum, dirID, name, perm, &result ) );
		
		return nucleus::owned< FSFileRefNum >::seize( FSFileRefNum( result ) );
	}
	
	nucleus::owned< FSFileRefNum > HOpenRF( Mac::FSVolumeRefNum   vRefNum,
	                                        Mac::FSDirID          dirID,
	                                        const unsigned char*  name,
	                                        Mac::FSIOPerm         perm )
	{
		SInt16 result;
		ThrowOSStatus( ::HOpenRF( vRefNum, dirID, name, perm, &result ) );
		
		return nucleus::owned< FSFileRefNum >::seize( FSFileRefNum( result ) );
	}
	
	void HCreate( const FSSpec&   f,
	              Mac::FSCreator  creator,
	              Mac::FSType     type )
	{
		ThrowOSStatus( ::HCreate( f.vRefNum, f.parID, f.name, creator, type ) );
	}
	
	Mac::FSDirID DirCreate( const FSSpec& loc )
	{
		SInt32 id;
		ThrowOSStatus( ::DirCreate( loc.vRefNum, loc.parID, loc.name, &id ) );
		
		return Mac::FSDirID( id );
	}
	
	void HDelete( const FSSpec& file )
	{
		ThrowOSStatus( ::HDelete( file.vRefNum, file.parID, file.name ) );
	}
	
	FInfo HGetFInfo( const FSSpec& f )
	{
		FInfo info;
		ThrowOSStatus( ::HGetFInfo( f.vRefNum, f.parID, f.name, &info ) );
		
		return info;
	}
	
	void HSetFInfo( const FSSpec& f, const FInfo& info )
	{
		ThrowOSStatus( ::HSetFInfo( f.vRefNum, f.parID, f.name, &info ) );
	}
	
	void HSetFLock( const FSSpec& file )
	{
		ThrowOSStatus( ::HSetFLock( file.vRefNum, file.parID, file.name ) );
	}
	
	void HRstFLock( const FSSpec& file )
	{
		ThrowOSStatus( ::HRstFLock( file.vRefNum, file.parID, file.name ) );
	}
	
	void HRename( const FSSpec& file, const unsigned char* name )
	{
		ThrowOSStatus( ::HRename( file.vRefNum, file.parID, file.name, name ) );
	}
	
	void CatMove( const FSSpec& f, const FSSpec& d )
	{
		if ( f.vRefNum != d.vRefNum )
		{
			ThrowOSStatus( paramErr );  // Follow MoreFiles' example
		}
		
		ThrowOSStatus( ::CatMove( f.vRefNum, f.parID, f.name,
		                                     d.parID, d.name ) );
	}
	
	
	void PBHGetVolParmsSync( HParamBlockRec& paramBlock )
	{
		ThrowOSStatus( ::PBHGetVolParmsSync( &paramBlock ) );
	}
	
	GetVolParmsInfoBuffer PBHGetVolParmsSync( FSVolumeRefNum vRefNum )
	{
		HParamBlockRec pb;
		GetVolParmsInfoBuffer info;
		
		pb.ioParam.ioNamePtr  = NULL;
		pb.ioParam.ioVRefNum  = vRefNum;
		pb.ioParam.ioBuffer   = reinterpret_cast< ::Ptr >( &info );
		pb.ioParam.ioReqCount = sizeof info;
		
		PBHGetVolParmsSync( pb );
		
		return info;
	}
	
	// Desktop Manager
	// ---------------
	
	void PBDTGetPath( DTPBRec& pb )
	{
		ThrowOSStatus( ::PBDTGetPath( &pb ) );
	}
	
	void PBDTGetPath( FSVolumeRefNum vRefNum, DTPBRec& pb )
	{
		pb.ioVRefNum = vRefNum;
		pb.ioNamePtr = NULL;
		
		PBDTGetPath( pb );
	}
	
	DTPBRec& FSpDTGetPath( const FSSpec& file, DTPBRec& pb )
	{
		PBDTGetPath( FSVolumeRefNum( file.vRefNum ), pb );
		
		pb.ioNamePtr = const_cast< unsigned char* >( file.name );
		pb.ioDirID   = file.parID;
		
		return pb;
	}
	
	void PBDTGetAPPLSync( DTPBRec& pb )
	{
		::OSStatus err = ::PBDTGetAPPLSync( &pb );
		
		if ( err == fnfErr )
		{
			// PBDTGetAPPLSync() is documented as returning afpItemNotFound, but not fnfErr.
			// We compensate for the bug here.
			// If this is actually a documentation bug, then this block should be removed
			// (although the comments should be left for posterity).
			err = afpItemNotFound;
		}
		
		ThrowOSStatus( err );
	}
	
	FSSpec DTGetAPPL( Mac::FSCreator signature, FSVolumeRefNum vRefNum )
	{
		DTPBRec pb;
		
		pb.ioVRefNum = vRefNum;
		pb.ioNamePtr = NULL;
		
		PBDTGetPath( pb );
		
		::Str255 name = "\p";  // Stack space is free
		
		pb.ioIndex       = 0;
		pb.ioFileCreator = signature;
		pb.ioNamePtr     = name;
		
		PBDTGetAPPLSync( pb );
		
		FSSpec result = { vRefNum, pb.ioAPPLParID };
		
		CopyToPascalString( name, result.name, sizeof result.name - 1 );
		
		return result;
	}
	
	void PBDTSetCommentSync( DTPBRec& pb )
	{
		ThrowOSStatus( ::PBDTSetCommentSync( &pb ) );
	}
	
	void DTSetComment( DTPBRec& pb, const char* comment, unsigned length )
	{
		pb.ioDTBuffer   = const_cast< char* >( comment );
		pb.ioDTReqCount = length;
		
		PBDTSetCommentSync( pb );
	}
	
	void FSpDTSetComment( const FSSpec& file, const char* comment, unsigned length )
	{
		DTPBRec pb;
		
		return DTSetComment( FSpDTGetPath( file, pb ), comment, length );
	}
	
	void PBDTGetCommentSync( DTPBRec& pb )
	{
		ThrowOSStatus( ::PBDTGetCommentSync( &pb ) );
	}
	
	static
	long DTGetComment( DTPBRec& pb, char* buffer, long length )
	{
		pb.ioDTBuffer   = buffer;
		pb.ioDTReqCount = length;
		
		ThrowOSStatus( ::PBDTGetCommentSync( &pb ) );
		
		return pb.ioDTActCount;
	}
	
	nucleus::string DTGetComment( DTPBRec& pb )
	{
		//const ByteCount kMaximumCommentLength      = 200;
		const ByteCount kExperimentalCommentLength = 256;
		
		char comment[ kExperimentalCommentLength ];
		
		long length = DTGetComment( pb, comment, kExperimentalCommentLength );
		
		ASSERT( pb.ioDTActCount >=   0 );
		ASSERT( pb.ioDTActCount <= 200 );
		
		return nucleus::string( comment, pb.ioDTActCount );
	}
	
	nucleus::string FSpDTGetComment( const FSSpec& file )
	{
		DTPBRec pb;
		
		return DTGetComment( FSpDTGetPath( file, pb ) );
	}
	
	long FSpDTGetComment( const FSSpec& file, char* buffer, long length )
	{
		DTPBRec pb;
		
		return DTGetComment( FSpDTGetPath( file, pb ), buffer, length );
	}
	
	
	FSSpec FSMakeFSSpec( FSVolumeRefNum vRefNum, FSDirID dirID, ConstStr255Param name)
	{
		FSSpec result;
		
		OSErr err = ::FSMakeFSSpec( vRefNum, dirID, name, &result );
		
		if ( err != fnfErr )
		{
			ThrowOSStatus( err );
		}
		
		return result;
	}
	
	FSSpec FSMakeFSSpec( const FSRef& ref )
	{
		FSSpec result;
		
		FSGetCatalogInfo( ref, 0, 0, 0, &result, 0 );
		
		return result;
	}
	
	// The cheap and easy way to do it.  This will throw if the pathname exceeds 255 characters.
	// It might be nice to have a backup method for when this one fails.
	FSSpec FSMakeFSSpec( ConstStr255Param pathname )
	{
		return FSMakeFSSpec( FSVolumeRefNum(), FSDirID(), pathname );
	}
	
	static bool FSCompareNames( ConstStr255Param a, ConstStr255Param b )
	{
		const bool caseSensitive        = false;
		const bool diacriticalSensitive = true;
		
		return ::EqualString( a, b, caseSensitive, diacriticalSensitive );
	}
	
	bool FSCompareFSSpecs( const FSSpec& a, const FSSpec& b )
	{
		return    a.vRefNum == b.vRefNum
		       && a.parID   == b.parID
		       && FSCompareNames( a.name, b.name );
	}
	
	nucleus::owned< FSFileRefNum > FSpOpenDF( const FSSpec&   spec,
	                                          FSIOPermssn     permissions )
	{
		SInt16 result;
		ThrowOSStatus( ::FSpOpenDF( &spec, permissions, &result ) );
		return nucleus::owned< FSFileRefNum >::seize( FSFileRefNum( result ) );
	}
	
	nucleus::owned< FSFileRefNum > FSpOpenRF( const FSSpec&   spec,
	                                          FSIOPermssn     permissions )
	{
		SInt16 result;
		ThrowOSStatus( ::FSpOpenRF( &spec, permissions, &result ) );
		return nucleus::owned< FSFileRefNum >::seize( FSFileRefNum( result ) );
	}
	
	FSSpec FSpCreate( const FSSpec&    file,
	                  Mac::FSCreator   creator,
	                  Mac::FSType      type,
	                  Mac::ScriptCode  scriptTag )
	{
		ThrowOSStatus( ::FSpCreate( &file, creator, type, scriptTag ) );
		return file;
	}
	
	FSDirSpec FSpDirCreate( const FSSpec& dir, Mac::ScriptCode scriptTag )
	{
		SInt32 newDirID;
		ThrowOSStatus( ::FSpDirCreate( &dir, scriptTag, &newDirID ) );
		
		return nucleus::make< FSDirSpec >( FSVolumeRefNum( dir.vRefNum ), FSDirID( newDirID ) );
	}
	
	void FSpDelete( const FSSpec& item )
	{
		ThrowOSStatus( ::FSpDelete( &item ) );
	}
	
	FInfo FSpGetFInfo( const FSSpec& file )
	{
		FInfo info;
		ThrowOSStatus( ::FSpGetFInfo( &file, &info ) );
		return info;
	}
	
	void FSpSetFInfo( const FSSpec& file, const FInfo& info )
	{
		ThrowOSStatus( ::FSpSetFInfo( &file, &info ) );
	}
	
	void FSpSetFLock( const FSSpec& file )
	{
		ThrowOSStatus( ::FSpSetFLock( &file ) );
	}
	
	void FSpRstFLock( const FSSpec& file )
	{
		ThrowOSStatus( ::FSpRstFLock( &file ) );
	}
	
	void FSpRename( const FSSpec& item, ConstStr255Param newName )
	{
		ThrowOSStatus( ::FSpRename( &item, newName ) );
	}
	
	void FSpCatMove( const FSSpec& source, const FSSpec& dest )
	{
		ThrowOSStatus( ::FSpCatMove( &source, &dest ) );
	}
	
   FSRef FSpMakeFSRef( const FSSpec& spec )
     {
      FSRef result;
      ThrowOSStatus( ::FSpMakeFSRef( &spec, &result ) );
      return result;
     }

   FSRef FSMakeFSRefUnicode( const FSRef& parentRef,
                             UniCharCount nameLength,
                             const UniChar *name,
                             TextEncoding textEncodingHint )
     {
      FSRef result;
      ThrowOSStatus( ::FSMakeFSRefUnicode( &parentRef,
                                           nameLength,
                                           name,
                                           textEncodingHint,
                                           &result ) );
      return result;
     }

#endif  // #if ! __LP64__
	
/*
Return Value
	A result code. See "File Manager Result Codes".
	If the two FSRef structures refer to the same file or directory, then noErr is returned. 
	If they refer to objects on different volumes, then diffVolErr is returned. 
	If they refer to different files or directories on the same volume, then errFSRefsDifferent is returned. 
	This function may return other errors, including nsvErr, fnfErr, dirNFErr, and volOffLinErr.
*/
   bool FSCompareFSRefs( const FSRef& ref1, const FSRef& ref2 )
     {
      const ::OSStatus error = ::FSCompareFSRefs( &ref1, &ref2 );
      if ( errFSRefsDifferent == error || diffVolErr == error )
         return false;
      ThrowOSStatus( error );
      return true;
     }

   void FileSystemDisposer::operator()( const FSRef& ref ) const
     {
      (void) ::FSDeleteObject( &ref );
     }

#if ! __LP64__
	
   void FileSystemDisposer::operator()( const FSSpec& spec ) const
     {
      (void) ::FSpDelete( &spec );
     }
   
#endif  // #if ! __LP64__
	
   
   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&         parentRef,
                                                   UniCharCount         nameLength,
                                                   const UniChar *      name,
                                                   FSCatalogInfoBitmap  whichInfo,
                                                   const FSCatalogInfo& catalogInfo )
     {
      FSRefSpec result;
      ThrowOSStatus( ::FSCreateFileUnicode( &parentRef,
                                            nameLength,
                                            name,
                                            whichInfo,
                                            &catalogInfo,
                                            &result.fsRef,
                                            &result.fsSpec ) );
      return result;
     }

   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&   parentRef,
                                                   UniCharCount   nameLength,
                                                   const UniChar *name )
     {
      FSRefSpec result;
      ThrowOSStatus( ::FSCreateFileUnicode( &parentRef,
                                            nameLength,
                                            name,
                                            kFSCatInfoNone,
                                            0,
                                            &result.fsRef,
                                            &result.fsSpec ) );
      return result;
     }


   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&         parentRef,
                                                             UniCharCount         nameLength,
                                                             const UniChar *      name,
                                                             FSCatalogInfoBitmap  whichInfo,
                                                             const FSCatalogInfo& catalogInfo )
     {
      FSRefSpecDirID result;
      UInt32 dirID;
      ThrowOSStatus( ::FSCreateDirectoryUnicode( &parentRef,
                                                 nameLength,
                                                 name,
                                                 whichInfo,
                                                 &catalogInfo,
                                                 &result.fsRef,
                                                 &result.fsSpec,
                                                 &dirID ) );
      result.dirID = FSDirID( dirID );
      return result;
     }

   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&   parentRef,
                                                             UniCharCount   nameLength,
                                                             const UniChar *name )
     {
      FSRefSpecDirID result;
      UInt32 dirID;
      ThrowOSStatus( ::FSCreateDirectoryUnicode( &parentRef,
                                                 nameLength,
                                                 name,
                                                 kFSCatInfoNone,
                                                 0,
                                                 &result.fsRef,
                                                 &result.fsSpec,
                                                 &dirID ) );
      result.dirID = FSDirID( dirID );
      return result;
     }


   void FSDeleteObject( const FSRef& ref )
     {
      ThrowOSStatus( ::FSDeleteObject( &ref ) );
     }

   void FSMoveObject( FSRef& ref, const FSRef& destDirectory )
     {
      FSRef newRef;
      ThrowOSStatus( ::FSMoveObject( &ref, &destDirectory, &newRef ) );
      ref = newRef;
     }

   void FSExchangeObjects( const FSRef& ref, const FSRef& destRef )
     {
      ThrowOSStatus( ::FSExchangeObjects( &ref, &destRef ) );
     }

   void FSRenameUnicode( FSRef& ref,
                         UniCharCount            nameLength,
                         const UniChar          *name,
                         TextEncoding            textEncodingHint )
     {
      FSRef newRef;
      ThrowOSStatus( ::FSRenameUnicode( &ref, nameLength, name, textEncodingHint, &newRef ) );
      ref = newRef;
     }

   void FSGetCatalogInfo( const FSRef&        ref,
                          FSCatalogInfoBitmap whichInfo,
                          FSCatalogInfo *     catalogInfo,
                          HFSUniStr255 *      outName,
                          FSSpec *            fsSpec,
                          FSRef *             parentRef )
     {
      ThrowOSStatus( ::FSGetCatalogInfo( &ref, whichInfo, catalogInfo, outName, fsSpec, parentRef ) );
     }

   FSGetCatalogInfo_Result FSGetCatalogInfo( const FSRef&        ref,
                                             FSCatalogInfoBitmap whichInfo )
     {
      FSGetCatalogInfo_Result result;
      FSGetCatalogInfo( ref,
                        whichInfo,
                        &result.catalogInfo,
                        &result.outName,
                        &result.fsSpec,
                        &result.parentRef );
      return result;
     }
   
   void FSSetCatalogInfo( const FSRef&         ref,
                          FSCatalogInfoBitmap  whichInfo,
                          const FSCatalogInfo& catalogInfo )
     {
      ThrowOSStatus( ::FSSetCatalogInfo( &ref, whichInfo, &catalogInfo ) );
     }
   
   nucleus::owned<FSIterator> FSOpenIterator( const FSRef& container, FSIteratorFlags iteratorFlags )
     {
      FSIterator result;
      ThrowOSStatus( ::FSOpenIterator( &container, iteratorFlags, &result ) );
      return nucleus::owned<FSIterator>::seize( result );
     }
   
   void FSCloseIterator( nucleus::owned<FSIterator> iterator )
     {
      ThrowOSStatus( ::FSCloseIterator( iterator.release() ) );
     }

   FSGetCatalogInfoBulk_Result FSGetCatalogInfoBulk( FSIterator          iterator,
                                                     ItemCount           maximumObjects,
                                                     FSCatalogInfoBitmap whichInfo,
                                                     FSCatalogInfo *     catalogInfos,
                                                     FSRef *             refs,
                                                     FSSpec *            specs,
                                                     HFSUniStr255 *      names )
     {
      FSGetCatalogInfoBulk_Result result;
      ::Boolean containerChanged = false;    // Prior to 10.2, FSGetCatalogInfoBulk leaves containerChanged unchanged.
      ::OSStatus error = ::FSGetCatalogInfoBulk( iterator,
                                                 maximumObjects,
                                                 &result.actualObjects,
                                                 &containerChanged,
                                                 whichInfo,
                                                 catalogInfos,
                                                 refs,
                                                 specs,
                                                 names );
          
      if ( error == errFSNoMoreItems )
         result.noMoreItems = true;
       else
        {
         ThrowOSStatus( error );
         result.noMoreItems = false;
        }

      if ( containerChanged )
         throw FSContainterChanged();
      
      return result;
     }

#ifndef __RELIX__

   namespace
     {
      class BulkInfoVectors
        {
         private:
            std::vector<FSCatalogInfo> *infos;
            std::vector<FSRef>         *refs;
            std::vector<FSSpec>        *specs;
            std::vector<HFSUniStr255>  *names;
         
         public:
            BulkInfoVectors( std::vector<FSCatalogInfo> *theInfos,
                             std::vector<FSRef>         *theRefs,
                             std::vector<FSSpec>        *theSpecs,
                             std::vector<HFSUniStr255>  *theNames )
              : infos( theInfos ),
                refs( theRefs ),
                specs( theSpecs ),
                names( theNames )
              {}
            
            ItemCount MaxCapacity() const
              {
               ItemCount capacity = 0;
               if ( infos != 0 ) capacity = std::max( capacity, infos->capacity() );
               if ( refs  != 0 ) capacity = std::max( capacity, refs ->capacity() );
               if ( specs != 0 ) capacity = std::max( capacity, specs->capacity() );
               if ( names != 0 ) capacity = std::max( capacity, names->capacity() );
               return capacity;
              }
            
            void clear()
              {
               if ( infos != 0 ) infos->clear();
               if ( refs  != 0 ) refs ->clear();
               if ( specs != 0 ) specs->clear();
               if ( names != 0 ) names->clear();
              }
            
            void resize( ItemCount size )
              {
               if ( infos != 0 ) infos->resize( size );
               if ( refs  != 0 ) refs ->resize( size );
               if ( specs != 0 ) specs->resize( size );
               if ( names != 0 ) names->resize( size );
              }
            
            FSCatalogInfo *InfoAt( ItemCount index ) { return (infos != 0) ? &(*infos)[index] : 0; }
            FSRef         *RefAt ( ItemCount index ) { return (refs  != 0) ? &(*refs )[index] : 0; }
            FSSpec        *SpecAt( ItemCount index ) { return (specs != 0) ? &(*specs)[index] : 0; }
            HFSUniStr255  *NameAt( ItemCount index ) { return (names != 0) ? &(*names)[index] : 0; }
        };
     }
      
   void FSGetCatalogInfoBulk( FSIterator                  iterator,
                              FSCatalogInfoBitmap         whichInfo,
                              std::vector<FSCatalogInfo> *infos,
                              std::vector<FSRef>         *refs,
                              std::vector<FSSpec>        *specs,
                              std::vector<HFSUniStr255>  *names )
     {
      BulkInfoVectors vectors( infos, refs, specs, names );
      ItemCount startingSize = std::max<ItemCount>( 64, vectors.MaxCapacity() );
      vectors.clear();
      
      ItemCount actualObjects = 0;
      
      for( ItemCount size = startingSize; true; size += std::min( size, std::numeric_limits<ItemCount>::max() - size ) )
        {
         vectors.resize( size );
         FSGetCatalogInfoBulk_Result infoResult = FSGetCatalogInfoBulk( iterator,
                                                                        size - actualObjects,
                                                                        whichInfo,
                                                                        vectors.InfoAt( actualObjects ),
                                                                        vectors.RefAt ( actualObjects ),
                                                                        vectors.SpecAt( actualObjects ),
                                                                        vectors.NameAt( actualObjects ) );
         actualObjects += infoResult.actualObjects;
         if ( infoResult.noMoreItems || size == std::numeric_limits<ItemCount>::max() )
            break;
        }
      
      vectors.resize( actualObjects );
     }

#endif

   FSCatalogSearch_Result FSCatalogSearch( FSIterator            iterator,
                                           const FSSearchParams& searchCriteria,
                                           ItemCount             maximumObjects,
                                           FSCatalogInfoBitmap   whichInfo,
                                           FSCatalogInfo *       catalogInfos,
                                           FSRef *               refs,
                                           FSSpec *              specs,
                                           HFSUniStr255 *        names )
     {
      FSCatalogSearch_Result result;
      ::Boolean containerChanged;
      ::OSStatus error = ::FSCatalogSearch( iterator,
                                            &searchCriteria,
                                            maximumObjects,
                                            &result.actualObjects,
                                            &containerChanged,
                                            whichInfo,
                                            catalogInfos,
                                            refs,
                                            specs,
                                            names );
      if ( error == errFSNoMoreItems )
         result.noMoreItems = true;
       else
        {
         ThrowOSStatus( error );
         result.noMoreItems = false;
        }

      if ( containerChanged )
         throw FSContainterChanged();

      return result;
     }

	void FSCreateFork( const FSRef&    ref,
	                   UniCharCount    forkNameLength,
	                   const UniChar*  forkName )
     {
      ThrowOSStatus( ::FSCreateFork( &ref, forkNameLength, forkName ) );
     }

	void FSDeleteFork( const FSRef&    ref,
	                   UniCharCount    forkNameLength,
	                   const UniChar*  forkName )
	{
		ThrowOSStatus( ::FSDeleteFork( &ref, forkNameLength, forkName ) );
	}
	
   FSIterateForks_Result FSIterateForks( const FSRef&    ref,
                                         CatPositionRec& forkIterator )
     {
      FSIterateForks_Result result;
      SInt64 forkSize;
      ThrowOSStatus( ::FSIterateForks( &ref,
                                       &forkIterator,
                                       &result.forkName,
                                       &forkSize,
                                       &result.forkPhysicalSize ) );
      result.forkSize = static_cast<UInt64>( forkSize );
      return result;
     }   

   nucleus::owned<FSForkRefNum> FSOpenFork( const FSRef&   ref,
                                            UniCharCount   forkNameLength,
                                            const UniChar *forkName,
                                            FSIOPermssn    permissions )
     {
      FSIORefNum result;
      ThrowOSStatus( ::FSOpenFork( &ref, forkNameLength, forkName, permissions, &result ) );
      return nucleus::owned<FSForkRefNum>::seize( FSForkRefNum( result ) );
     }

	ByteCount FSReadFork( FSForkRefNum     fork,
	                      FSIOPosMode      positionMode,
	                      SInt64           positionOffset,
	                      ByteCount        requestCount,
	                      void *           buffer,
	                      ThrowEOF_Always  policy )
	{
		ByteCount actualCount;
		
		::OSStatus err = ::FSReadFork( fork,
		                               positionMode,
		                               positionOffset,
		                               requestCount,
		                               buffer,
		                               &actualCount );
		
		if ( err == noErr  &&  actualCount != requestCount )
		{
			err = eofErr;
		}
		
		ThrowOSStatus( err );
		
		return actualCount;
	}
	
	ByteCount FSReadFork( FSForkRefNum    fork,
	                      FSIOPosMode     positionMode,
	                      SInt64          positionOffset,
	                      ByteCount       requestCount,
	                      void *          buffer,
	                      ThrowEOF_Never  policy )
	{
		ByteCount actualCount;
		
		::OSStatus err = ::FSReadFork( fork,
		                               positionMode,
		                               positionOffset,
		                               requestCount,
		                               buffer,
		                               &actualCount );
		      
		if ( err != eofErr )
		{
			ThrowOSStatus( err );
		}
		
		return actualCount;
	}
	
	ByteCount FSReadFork( FSForkRefNum  fork,
	                      FSIOPosMode   positionMode,
	                      SInt64        positionOffset,
	                      ByteCount     requestCount,
	                      void *        buffer )
	{
		ByteCount actualCount = FSReadFork( fork,
		                                    positionMode,
		                                    positionOffset,
		                                    requestCount,
		                                    buffer,
		                                    ThrowEOF_Never() );
		
		if ( actualCount == 0 )
		{
			ThrowOSStatus( eofErr );
		}
		
		return actualCount;
	}
	
   ByteCount FSWriteFork( FSForkRefNum fork,
                          FSIOPosMode  positionMode,
                          SInt64       positionOffset,
                          ByteCount    requestCount,
                          const void * buffer )
     {
      ByteCount actualCount;
      ThrowOSStatus( ::FSWriteFork( fork,
                                    positionMode,
                                    positionOffset,
                                    requestCount,
                                    buffer,
                                    &actualCount ) );
      return actualCount;
     }

   SInt64 FSGetForkPosition( FSForkRefNum forkRefNum )
     {
      SInt64 position;
      ThrowOSStatus( ::FSGetForkPosition( forkRefNum, &position ) );
      return position;
     }
   
   void FSSetForkPosition( FSForkRefNum forkRefNum,
                           FSIOPosMode  positionMode,
                           SInt64       positionOffset )
     {
      ThrowOSStatus( ::FSSetForkPosition( forkRefNum, positionMode, positionOffset ) );
     }

   SInt64 FSGetForkSize( FSForkRefNum forkRefNum )
     {
      SInt64 position;
      ThrowOSStatus( ::FSGetForkSize( forkRefNum, &position ) );
      return position;
     }
   
   void FSSetForkSize( FSForkRefNum forkRefNum,
                       FSIOPosMode  positionMode,
                       SInt64       positionOffset )
     {
      ThrowOSStatus( ::FSSetForkSize( forkRefNum, positionMode, positionOffset ) );
     }

   UInt64 FSAllocateFork( FSForkRefNum      forkRefNum,
                          FSAllocationFlags flags,
                          FSIOPosMode       positionMode,
                          SInt64            positionOffset,
                          UInt64            requestCount )
     {
      UInt64 actualCount;
      ThrowOSStatus( ::FSAllocateFork( forkRefNum,
                                       flags,
                                       positionMode,
                                       positionOffset,
                                       requestCount,
                                       &actualCount ) );
      return actualCount;
     }

   void FSFlushFork( FSForkRefNum forkRefNum )
     {
      ThrowOSStatus( ::FSFlushFork( forkRefNum ) );
     }
   
   void FSCloseFork( nucleus::owned<FSForkRefNum> forkRefNum )
     {
      ThrowOSStatus( ::FSCloseFork( forkRefNum.release() ) );
     }
   
   FSGetForkCBInfo_Result FSGetForkCBInfo( FSForkRefNum desiredRefNum )
     {
      FSGetForkCBInfo_Result result;
      FSIORefNum actualRefNum;
      ThrowOSStatus( ::FSGetForkCBInfo( desiredRefNum,
                                        0,
                                        0,
                                        &actualRefNum,
                                        &result.forkInfo,
                                        &result.ref,
                                        &result.outForkName ) );
      result.actualRefNum = FSForkRefNum( actualRefNum );
      return result;
     }

   FSGetForkCBInfo_Result FSGetForkCBInfo( FSVolumeRefNum  volume,
                                           FSForkIterator& iterator )
     {
      FSGetForkCBInfo_Result result;
      FSIORefNum actualRefNum;
      SInt16 realIterator = iterator;
      ThrowOSStatus( ::FSGetForkCBInfo( 0,
                                        volume,
                                        &realIterator,
                                        &actualRefNum,
                                        &result.forkInfo,
                                        &result.ref,
                                        &result.outForkName ) );
      result.actualRefNum = FSForkRefNum( actualRefNum );
      iterator = FSForkIterator( realIterator );
      return result;
     }

   FSGetForkCBInfo_Result FSGetForkCBInfo( FSForkIterator& iterator )
     {
      return FSGetForkCBInfo( FSVolumeRefNum(), iterator );
     }

   void FSGetVolumeInfo( FSVolumeRefNum     volume,
                         FSVolumeIndex      volumeIndex,
                         FSVolumeRefNum *   resultVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory )
     {
      ::FSVolumeRefNum actualVolume;
      ThrowOSStatus( ::FSGetVolumeInfo( volume,
                                        volumeIndex,
                                        &actualVolume,
                                        whichInfo,
                                        info,
                                        volumeName,
                                        rootDirectory ) );
      if ( resultVolume != 0 )
         *resultVolume = FSVolumeRefNum( actualVolume );
     }

   void FSGetVolumeInfo( FSVolumeRefNum     volume,
                         FSVolumeRefNum *   resultVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory )
     {
      return FSGetVolumeInfo( volume,
                              FSVolumeIndex(),
                              resultVolume,
                              whichInfo,
                              info,
                              volumeName,
                              rootDirectory );
     }

   void FSGetVolumeInfo( FSVolumeIndex      volumeIndex,
                         FSVolumeRefNum *   resultVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory )
     {
      return FSGetVolumeInfo( FSVolumeRefNum(),
                              volumeIndex,
                              resultVolume,
                              whichInfo,
                              info,
                              volumeName,
                              rootDirectory );
     }

   FSGetVolumeInfo_Result FSGetVolumeInfo( FSVolumeRefNum volume,
                                           FSVolumeInfoBitmap whichInfo )
     {
      FSGetVolumeInfo_Result result;
      FSGetVolumeInfo( volume,
                       &result.actualVolume,
                       whichInfo,
                       &result.info,
                       &result.volumeName,
                       &result.rootDirectory );
      return result;
     }
   
   FSGetVolumeInfo_Result FSGetVolumeInfo( FSVolumeIndex volumeIndex,
                                           FSVolumeInfoBitmap whichInfo )
     {
      FSGetVolumeInfo_Result result;
      FSGetVolumeInfo( volumeIndex,
                       &result.actualVolume,
                       whichInfo,
                       &result.info,
                       &result.volumeName,
                       &result.rootDirectory );
      return result;
     }
	
	
#if ! __LP64__
	
	static void PBHGetVInfoSync( HParamBlockRec& pb )
	{
		ThrowOSStatus( ::PBHGetVInfoSync( &pb ) );
	}
	
#endif  // #if ! __LP64__
	
	FSVolumeRefNum FSGetVolumeRefNum( FSVolumeIndex volumeIndex )
	{
	#if ! TARGET_CPU_68K
		
		if ( TARGET_API_MAC_CARBON  ||  &::FSGetVolumeInfo != NULL )
		{
			FSVolumeRefNum result;
			FSGetVolumeInfo( volumeIndex,
			                 &result,
			                 kFSVolInfoNone,
			                 0,
			                 0,
			                 0 );
			return result;
		}
		else
		
	#endif
		{
		#if ! __LP64__
			
			HParamBlockRec pb;
			
			pb.volumeParam.ioNamePtr = NULL;
			pb.volumeParam.ioVRefNum = 0;
			pb.volumeParam.ioVolIndex = volumeIndex;
			
			PBHGetVInfoSync( pb );
			
			return FSVolumeRefNum( pb.volumeParam.ioVRefNum );
			
		#endif
		}
	}

   HFSUniStr255 FSGetVolumeName( FSVolumeRefNum volume )
     {
      HFSUniStr255 result;
      FSGetVolumeInfo( volume,
                       0,
                       kFSVolInfoNone,
                       0,
                       &result,
                       0 );
      return result;
     }
   
   HFSUniStr255 FSGetVolumeName( FSVolumeIndex volumeIndex )
     {
      HFSUniStr255 result;
      FSGetVolumeInfo( volumeIndex,
                       0,
                       kFSVolInfoNone,
                       0,
                       &result,
                       0 );
      return result;
     }
   
   FSRef FSGetVolumeRootDirectory( FSVolumeRefNum volume )
     {
      FSRef result;
      FSGetVolumeInfo( volume,
                       0,
                       kFSVolInfoNone,
                       0,
                       0,
                       &result );
      return result;
     }

   FSRef FSGetVolumeRootDirectory( FSVolumeIndex volumeIndex )
     {
      FSRef result;
      FSGetVolumeInfo( volumeIndex,
                       0,
                       kFSVolInfoNone,
                       0,
                       0,
                       &result );
      return result;
     }
   
   void FSSetVolumeInfo( FSVolumeRefNum volume,
                         FSVolumeInfoBitmap  whichInfo,
                         const FSVolumeInfo& volumeInfo )
     {
      ThrowOSStatus( ::FSSetVolumeInfo( volume, whichInfo, &volumeInfo ) );
     }
   
   HFSUniStr255 FSGetDataForkName()
     {
      HFSUniStr255 result;
      ThrowOSStatus( ::FSGetDataForkName( &result ) );
      return result;
     }
   
   HFSUniStr255 FSGetResourceForkName()
     {
      HFSUniStr255 result;
      ThrowOSStatus( ::FSGetResourceForkName( &result ) );
      return result;
     }

   void FSRefMakePath( const FSRef& ref,
                       UInt8 *      path,
                       UInt32       maxPathSize )
     {
      ThrowOSStatus( ::FSRefMakePath( &ref, path, maxPathSize ) );
     }
   
   nucleus::string FSRefMakePath( const FSRef& ref )
     {
      for ( UInt32 maxPathSize = 1024; true; maxPathSize *= 2 )
        {
         try
           {
            nucleus::mutable_string result( maxPathSize, '\0' );
            FSRefMakePath( ref, reinterpret_cast<UInt8 *>( &result[0] ), result.size() );
            result.resize( std::strlen( result.c_str() ) );
            return result;
           }
         catch ( const Mac::OSStatus& err )
           {
            if ( err != pathTooLongErr  &&  err != buffersTooSmall )
               throw;
            if ( maxPathSize > std::numeric_limits<UInt32>::max() / 2 )
               throw;
            // Otherwise, try again with a larger string
           }
        }
     }
   
   FSPathMakeRef_Result FSPathMakeRef( const UInt8 *path )
     {
      FSPathMakeRef_Result result;
      ::Boolean isDirectory;
      ThrowOSStatus( ::FSPathMakeRef( path, &result.ref, &isDirectory ) );
      result.isDirectory = isDirectory;
      return result;
     }
   
   void RegisterFileManagerErrors()
     {
      RegisterOSStatus< notOpenErr                    >();
      RegisterOSStatus< dirFulErr                     >();
      RegisterOSStatus< dskFulErr                     >();
      RegisterOSStatus< nsvErr                        >();
      RegisterOSStatus< ioErr                         >();
      RegisterOSStatus< bdNamErr                      >();
      RegisterOSStatus< fnOpnErr                      >();
      RegisterOSStatus< eofErr                        >();
      RegisterOSStatus< posErr                        >();
      RegisterOSStatus< mFulErr                       >();
      RegisterOSStatus< tmfoErr                       >();
      RegisterOSStatus< fnfErr                        >();
      RegisterOSStatus< wPrErr                        >();
      RegisterOSStatus< fLckdErr                      >();
      RegisterOSStatus< vLckdErr                      >();
      RegisterOSStatus< fBsyErr                       >();
      RegisterOSStatus< dupFNErr                      >();
      RegisterOSStatus< opWrErr                       >();
      RegisterOSStatus< paramErr                      >();
      RegisterOSStatus< rfNumErr                      >();
      RegisterOSStatus< gfpErr                        >();
      RegisterOSStatus< volOffLinErr                  >();
      RegisterOSStatus< permErr                       >();
      RegisterOSStatus< volOnLinErr                   >();
      RegisterOSStatus< nsDrvErr                      >();
      RegisterOSStatus< noMacDskErr                   >();
      RegisterOSStatus< extFSErr                      >();
      RegisterOSStatus< fsRnErr                       >();
      RegisterOSStatus< badMDBErr                     >();
      RegisterOSStatus< wrPermErr                     >();

      RegisterOSStatus< memFullErr                    >();
      RegisterOSStatus< dirNFErr                      >();
      RegisterOSStatus< tmwdoErr                      >();
      RegisterOSStatus< badMovErr                     >();
      RegisterOSStatus< wrgVolTypErr                  >();
      RegisterOSStatus< volGoneErr                    >();
      RegisterOSStatus< fsDSIntErr                    >();

      RegisterOSStatus< fidNotFound                   >();
      RegisterOSStatus< fidExists                     >();
      RegisterOSStatus< notAFileErr                   >();
      RegisterOSStatus< diffVolErr                    >();
      RegisterOSStatus< catChangedErr                 >();
      RegisterOSStatus< desktopDamagedErr             >();
      RegisterOSStatus< sameFileErr                   >();
      RegisterOSStatus< badFidErr                     >();
      RegisterOSStatus< notARemountErr                >();
      RegisterOSStatus< fileBoundsErr                 >();
      RegisterOSStatus< fsDataTooBigErr               >();
      RegisterOSStatus< volVMBusyErr                  >();
      RegisterOSStatus< badFCBErr                     >();
      RegisterOSStatus< errFSUnknownCall              >();
      RegisterOSStatus< errFSBadFSRef                 >();
      RegisterOSStatus< errFSBadForkName              >();
      RegisterOSStatus< errFSBadBuffer                >();
      RegisterOSStatus< errFSBadForkRef               >();
      RegisterOSStatus< errFSBadInfoBitmap            >();
      RegisterOSStatus< errFSMissingCatInfo           >();
      RegisterOSStatus< errFSNotAFolder               >();
      RegisterOSStatus< errFSForkNotFound             >();
      RegisterOSStatus< errFSNameTooLong              >();
      RegisterOSStatus< errFSMissingName              >();
      RegisterOSStatus< errFSBadPosMode               >();
      RegisterOSStatus< errFSBadAllocFlags            >();
      RegisterOSStatus< errFSNoMoreItems              >();
      RegisterOSStatus< errFSBadItemCount             >();
      RegisterOSStatus< errFSBadSearchParams          >();
      RegisterOSStatus< errFSRefsDifferent            >();
      RegisterOSStatus< errFSForkExists               >();
      RegisterOSStatus< errFSBadIteratorFlags         >();
      RegisterOSStatus< errFSIteratorNotFound         >();
      RegisterOSStatus< errFSIteratorNotSupported     >();

      RegisterOSStatus< afpAccessDenied               >();
      RegisterOSStatus< afpAuthContinue               >();
      RegisterOSStatus< afpBadUAM                     >();
      RegisterOSStatus< afpBadVersNum                 >();
      RegisterOSStatus< afpBitmapErr                  >();
      RegisterOSStatus< afpCantMove                   >();
      RegisterOSStatus< afpDenyConflict               >();
      RegisterOSStatus< afpDirNotEmpty                >();
      RegisterOSStatus< afpDiskFull                   >();
      RegisterOSStatus< afpEofError                   >();
      RegisterOSStatus< afpFileBusy                   >();
      RegisterOSStatus< afpFlatVol                    >();
      RegisterOSStatus< afpItemNotFound               >();
      RegisterOSStatus< afpLockErr                    >();
      RegisterOSStatus< afpMiscErr                    >();
      RegisterOSStatus< afpNoMoreLocks                >();
      RegisterOSStatus< afpNoServer                   >();
      RegisterOSStatus< afpObjectExists               >();
      RegisterOSStatus< afpObjectNotFound             >();
      RegisterOSStatus< afpParmErr                    >();
      RegisterOSStatus< afpRangeNotLocked             >();
      RegisterOSStatus< afpRangeOverlap               >();
      RegisterOSStatus< afpSessClosed                 >();
      RegisterOSStatus< afpUserNotAuth                >();
      RegisterOSStatus< afpCallNotSupported           >();
      RegisterOSStatus< afpObjectTypeErr              >();
      RegisterOSStatus< afpTooManyFilesOpen           >();
      RegisterOSStatus< afpServerGoingDown            >();
      RegisterOSStatus< afpCantRename                 >();
      RegisterOSStatus< afpDirNotFound                >();
      RegisterOSStatus< afpIconTypeError              >();
      RegisterOSStatus< afpVolLocked                  >();
      RegisterOSStatus< afpObjectLocked               >();
      RegisterOSStatus< afpContainsSharedErr          >();
      RegisterOSStatus< afpIDNotFound                 >();
      RegisterOSStatus< afpIDExists                   >();
      RegisterOSStatus< afpDiffVolErr                 >();
      RegisterOSStatus< afpCatalogChanged             >();
      RegisterOSStatus< afpSameObjectErr              >();
      RegisterOSStatus< afpBadIDErr                   >();
      RegisterOSStatus< afpPwdSameErr                 >();
      RegisterOSStatus< afpPwdTooShortErr             >();
      RegisterOSStatus< afpPwdExpiredErr              >();
      RegisterOSStatus< afpInsideSharedErr            >();
      RegisterOSStatus< afpInsideTrashErr             >();
      RegisterOSStatus< afpPwdNeedsChangeErr          >();
      RegisterOSStatus< afpPwdPolicyErr               >();
      RegisterOSStatus< afpAlreadyLoggedInErr         >();
      RegisterOSStatus< afpCallNotAllowed             >();
      RegisterOSStatus< afpBadDirIDType               >();
      RegisterOSStatus< afpCantMountMoreSrvre         >();
      RegisterOSStatus< afpAlreadyMounted             >();
      RegisterOSStatus< afpSameNodeErr                >();

      RegisterOSStatus< pathTooLongErr                >();
      RegisterOSStatus< buffersTooSmall               >();
     }
	
}
