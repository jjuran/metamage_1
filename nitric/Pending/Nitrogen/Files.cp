// Files.cp

#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_INITIALIZE_H
#include "Nitrogen/Initialize.h"
#endif

#include <limits>

namespace Nitrogen
  {
	
	void FSClose( Owned< FSFileRefNum > fileRefNum )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		ThrowOSStatus( ::FSClose( fileRefNum.Release() ) );
	}
	
	SInt32 FSRead( FSFileRefNum file,
	               SInt32       requestCount,
	               void *       buffer )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		SInt32 actualCount = requestCount;
		
		OSStatus err = ::FSRead( file,
	                             &actualCount,
	                             buffer );
		
		if ( err == eofErr  &&  actualCount > 0 )
		{
			err = noErr;
		}
		
		ThrowOSStatus( err );
		
		return actualCount;
	}
	
	SInt32 FSWrite( FSFileRefNum file,
	                SInt32       requestCount,
	                const void * buffer )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		SInt32 actualCount = requestCount;
		ThrowOSStatus( ::FSWrite( file,
	                             &actualCount,
	                             buffer ) );
		return actualCount;
	}
	
	SInt32 Allocate( FSFileRefNum      fileRefNum,
	                 SInt32            requestCount )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		SInt32 actualCount = requestCount;
		ThrowOSStatus( ::Allocate( fileRefNum,
	                              &actualCount ) );
		return actualCount;
	}
	
	SInt32 GetEOF( FSFileRefNum fileRefNum )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		SInt32 position;
		ThrowOSStatus( ::GetEOF( fileRefNum, &position ) );
		return position;
	}
	
	void SetEOF( FSFileRefNum fileRefNum,
	             SInt32       positionOffset )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		ThrowOSStatus( ::SetEOF( fileRefNum, positionOffset ) );
	}
	
	SInt32 GetFPos( FSFileRefNum fileRefNum )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		SInt32 position;
		ThrowOSStatus( ::GetFPos( fileRefNum, &position ) );
		return position;
	}
	
	void SetFPos( FSFileRefNum  fileRefNum,
	              FSIOPosMode   positionMode,
	              SInt32        positionOffset )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		ThrowOSStatus( ::SetFPos( fileRefNum, positionMode.Get(), positionOffset ) );
	}
	
	CInfoPBRec& PBGetCatInfoSync( CInfoPBRec& paramBlock )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		ThrowOSStatus( ::PBGetCatInfoSync( &paramBlock ) );
		
		return paramBlock;
	}
	
	template <>
	struct Initializer< CInfoPBRec >
	{
		CInfoPBRec& operator()( CInfoPBRec&     paramBlock,
		                        FSVolumeRefNum  vRefNum,
		                        FSDirID         dirID,
		                        StringPtr       name,
		                        SInt16          index )
		{
			DirInfo& dirInfo = paramBlock.dirInfo;
			
			dirInfo.ioNamePtr = name;
			dirInfo.ioVRefNum = vRefNum;
			dirInfo.ioDrDirID = dirID;
			dirInfo.ioFDirIndex = index;
			
			return paramBlock;
		}
	};
	
	CInfoPBRec& FSpGetCatInfo( const FSSpec& item, CInfoPBRec& paramBlock )
	{
		// There is/was a file sharing problem with null or empty names,
		// but an FSSpec's name is never empty (and can't be null).
		
		// ioFDirIndex = 0:  use ioDrDirID and ioNamePtr
		
		PBGetCatInfoSync( Initialize< CInfoPBRec >( paramBlock,
		                                            item.vRefNum,
		                                            item.parID,
		                                            const_cast< StringPtr >( item.name ),
		                                            0 ) );
		
		// Don't break the const contract on item.name
		paramBlock.dirInfo.ioNamePtr = NULL;
		
		return paramBlock;
	}
	
	CInfoPBRec& FSpGetCatInfo( const FSDirSpec&  dir,
	                           CInfoPBRec&       paramBlock,
	                           StringPtr         name )
	{
		// ioFDirIndex < 0:  use ioDrDirID only
		
		return PBGetCatInfoSync( Initialize< CInfoPBRec >( paramBlock,
		                                                   dir.vRefNum,
		                                                   dir.dirID,
		                                                   name,  // Output only
		                                                   -1 ) );
		
	}
	
	CInfoPBRec& FSpGetCatInfo( const FSDirSpec&  dir,
	                           UInt16            index,
	                           CInfoPBRec&       paramBlock,
	                           StringPtr         name )
	{
		// ioFDirIndex > 0:  use ioDrDirID only
		
		// Assert index != 0
		
		return PBGetCatInfoSync( Initialize< CInfoPBRec >( paramBlock,
		                                                   dir.vRefNum,
		                                                   dir.dirID,
		                                                   name,  // Output only
		                                                   index ) );
		
	}
	
	inline bool TestIsDirectory( const CInfoPBRec& paramBlock )
	{
		return paramBlock.hFileInfo.ioFlAttrib & char( kioFlAttribDirMask );
	}
	
	FSDirSpec Converter< FSDirSpec, FSSpec >::operator()( const FSSpec& dir ) const
	{
		CInfoPBRec pb;
		FSpGetCatInfo( dir, pb );
		
		if ( !TestIsDirectory( pb ) )
		{
			// I wanted a dir but you gave me a file.  You creep.
			throw ErrFSNotAFolder();
		}
		
		FSDirID dirID = FSDirID( pb.dirInfo.ioDrDirID );
		
		return Make< FSDirSpec >( dir.vRefNum, dirID );
	}
	
	void PBHGetVolParmsSync( HParamBlockRec& paramBlock )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
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
	
	void PBDTGetPath( DTPBRec& pb )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		ThrowOSStatus( ::PBDTGetPath( &pb ) );
	}
	
	void PBDTGetAPPLSync( DTPBRec& pb )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		try
		{
			ThrowOSStatus( ::PBDTGetAPPLSync( &pb ) );
		}
		catch ( FNFErr )
		{
			// PBDTGetAPPLSync() is documented as returning afpItemNotFound, but not fnfErr.
			// We compensate for the bug here.
			// If this is actually a documentation bug, then this block should be removed
			// (although the comments should be left for posterity).
			throw AFPItemNotFound();
		}
	}
	
	FSSpec DTGetAPPL( OSType signature, FSVolumeRefNum vRefNum )
	{
		DTPBRec pb;
		
		pb.ioVRefNum = vRefNum;
		pb.ioNamePtr = NULL;
		
		PBDTGetPath( pb );
		
		Str255 name;  // Stack space is free
		pb.ioIndex       = 0;
		pb.ioFileCreator = signature;
		pb.ioNamePtr     = name;
		
		PBDTGetAPPLSync( pb );
		
		return FSMakeFSSpec( vRefNum, FSDirID( pb.ioAPPLParID ), name );
	}
	
	FSSpec FSMakeFSSpec( FSVolumeRefNum vRefNum, FSDirID dirID, ConstStr255Param name)
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		FSSpec result;
		
		ThrowOSStatus( ::FSMakeFSSpec( vRefNum, dirID, name, &result ) );
		
		return result;
	}
	
	// The cheap and easy way to do it.  This will throw if the pathname exceeds 255 characters.
	// It might be nice to have a backup method for when this one fails.
	FSSpec FSMakeFSSpec( ConstStr255Param pathname )
	{
		return FSMakeFSSpec( FSVolumeRefNum(), FSDirID(), pathname );
	}
	
	Owned< FSFileRefNum > FSpOpenDF( const FSSpec&   spec,
	                                 FSIOPermssn     permissions )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		SInt16 result;
		ThrowOSStatus( ::FSpOpenDF( &spec, permissions, &result ) );
		return Owned< FSFileRefNum >::Seize( FSFileRefNum( result ) );
	}
	
	Owned< FSFileRefNum > FSpOpenRF( const FSSpec&   spec,
	                                 FSIOPermssn     permissions )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		SInt16 result;
		ThrowOSStatus( ::FSpOpenRF( &spec, permissions, &result ) );
		return Owned< FSFileRefNum >::Seize( FSFileRefNum( result ) );
	}
	
	FSSpec FSpCreate( const FSSpec& file, OSType creator, OSType type, ScriptCode scriptTag )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		ThrowOSStatus( ::FSpCreate( &file, creator, type, scriptTag ) );
		return file;
	}
	
	FSDirSpec FSpDirCreate( const FSSpec& dir, ScriptCode scriptTag )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		SInt32 newDirID;
		ThrowOSStatus( ::FSpDirCreate( &dir, scriptTag, &newDirID ) );
		
		return Make< FSDirSpec >( FSVolumeRefNum( dir.vRefNum ), FSDirID( newDirID ) );
	}
	
	void FSpDelete( const FSSpec& item )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		ThrowOSStatus( ::FSpDelete( &item ) );
	}
	
	FInfo FSpGetFInfo( const FSSpec& file )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		FInfo info;
		ThrowOSStatus( ::FSpGetFInfo( &file, &info ) );
		return info;
	}
	
	void FSpCatMove( const FSSpec& source, const FSSpec& dest )
	{
		OnlyOnce< RegisterFileManagerErrors >();
		
		ThrowOSStatus( ::FSpCatMove( &source, &dest ) );
	}
	
   FSRef FSpMakeFSRef( const FSSpec& spec )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSRef result;
      ThrowOSStatus( ::FSpMakeFSRef( &spec, &result ) );
      return result;
     }

   FSRef FSMakeFSRefUnicode( const FSRef& parentRef,
                             UniCharCount nameLength,
                             const UniChar *name,
                             TextEncoding textEncodingHint )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSRef result;
      ThrowOSStatus( ::FSMakeFSRefUnicode( &parentRef,
                                           nameLength,
                                           name,
                                           textEncodingHint,
                                           &result ) );
      return result;
     }

   FSRef FSMakeFSRefUnicode( const FSRef& parentRef,
                             const UniString& name,
                             TextEncoding textEncodingHint )
     {
      return FSMakeFSRefUnicode( parentRef, name.size(), name.data(), textEncodingHint );
     }
   
   bool FSCompareFSRefs( const FSRef& ref1, const FSRef& ref2 )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      const OSStatus error = ::FSCompareFSRefs( &ref1, &ref2 );
      if ( error == errFSRefsDifferent )
         return false;
      ThrowOSStatus( error );
      return true;
     }

   void FileSystemDisposer::operator()( const FSRef& ref ) const
     {
      OnlyOnce<RegisterFileManagerErrors>();
      DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::FSDeleteObject( &ref ) );
     }

   void FileSystemDisposer::operator()( const FSSpec& spec ) const
     {
      OnlyOnce<RegisterFileManagerErrors>();
      DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::FSpDelete( &spec ) );
     }
   
   
   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&         parentRef,
                                                   UniCharCount         nameLength,
                                                   const UniChar *      name,
                                                   FSCatalogInfoBitmap  whichInfo,
                                                   const FSCatalogInfo& catalogInfo )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSRefSpec result;
      ThrowOSStatus( ::FSCreateFileUnicode( &parentRef,
                                            nameLength,
                                            name,
                                            whichInfo,
                                            &catalogInfo,
                                            &result.fsRef,
                                            &result.fsSpec ) );
      return Owned<FSRefSpec>::Seize( result );
     }

   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&   parentRef,
                                                   UniCharCount   nameLength,
                                                   const UniChar *name )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSRefSpec result;
      ThrowOSStatus( ::FSCreateFileUnicode( &parentRef,
                                            nameLength,
                                            name,
                                            kFSCatInfoNone,
                                            0,
                                            &result.fsRef,
                                            &result.fsSpec ) );
      return Owned<FSRefSpec>::Seize( result );
     }


   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&         parentRef,
                                                   const UniString&     name,
                                                   FSCatalogInfoBitmap  whichInfo,
                                                   const FSCatalogInfo& catalogInfo )
     {
      return FSCreateFileUnicode( parentRef, name.size(), name.data(), whichInfo, catalogInfo );
     }


   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&     parentRef,
                                                   const UniString& name )
     {
      return FSCreateFileUnicode( parentRef, name.size(), name.data() );
     }

   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&         parentRef,
                                                             UniCharCount         nameLength,
                                                             const UniChar *      name,
                                                             FSCatalogInfoBitmap  whichInfo,
                                                             const FSCatalogInfo& catalogInfo )
     {
      OnlyOnce<RegisterFileManagerErrors>();
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
      return Owned<FSRefSpecDirID>::Seize( result );
     }

   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&   parentRef,
                                                             UniCharCount   nameLength,
                                                             const UniChar *name )
     {
      OnlyOnce<RegisterFileManagerErrors>();
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
      return Owned<FSRefSpecDirID>::Seize( result );
     }


   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&         parentRef,
                                                             const UniString&     name,
                                                             FSCatalogInfoBitmap  whichInfo,
                                                             const FSCatalogInfo& catalogInfo )
     {
      return FSCreateDirectoryUnicode( parentRef, name.size(), name.data(), whichInfo, catalogInfo );
     }


   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&     parentRef,
                                                             const UniString& name )
     {
      return FSCreateDirectoryUnicode( parentRef, name.size(), name.data() );
     }

   void FSDeleteObject( Owned<FSRef> owned )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSRef unowned = owned.Release();
      ThrowOSStatus( ::FSDeleteObject( &unowned ) );
     }

   void FSMoveObject( Owned<FSRef>& ref, const FSRef& destDirectory )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSRef newRef;
      ThrowOSStatus( ::FSMoveObject( &ref.Get(), &destDirectory, &newRef ) );
      ref.Release();
      ref = Owned<FSRef>::Seize( newRef );
     }

   void FSExchangeObjects( const FSRef& ref, const FSRef& destRef )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      ThrowOSStatus( ::FSExchangeObjects( &ref, &destRef ) );
     }

   void FSRenameUnicode( Owned<FSRef>&  ref,
                         UniCharCount   nameLength,
                         const UniChar *name,
                         TextEncoding   textEncodingHint )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSRef newRef;
      ThrowOSStatus( ::FSRenameUnicode( &ref.Get(), nameLength, name, textEncodingHint, &newRef ) );
      ref.Release();
      ref = Owned<FSRef>::Seize( newRef );
     }

   void FSRenameUnicode( Owned<FSRef>&    ref,
                         const UniString& name,
                         TextEncoding     textEncodingHint )
     {
      FSRenameUnicode( ref, name.size(), name.data(), textEncodingHint );
     }

   void FSGetCatalogInfo( const FSRef&        ref,
                          FSCatalogInfoBitmap whichInfo,
                          FSCatalogInfo *     catalogInfo,
                          HFSUniStr255 *      outName,
                          FSSpec *            fsSpec,
                          FSRef *             parentRef )
     {
      OnlyOnce<RegisterFileManagerErrors>();
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
      OnlyOnce<RegisterFileManagerErrors>();
      ThrowOSStatus( ::FSSetCatalogInfo( &ref, whichInfo, &catalogInfo ) );
     }
   
   Owned<FSIterator> FSOpenIterator( const FSRef& container, FSIteratorFlags iteratorFlags )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSIterator result;
      ThrowOSStatus( ::FSOpenIterator( &container, iteratorFlags, &result ) );
      return Owned<FSIterator>::Seize( result );
     }
   
   void FSCloseIterator( Owned<FSIterator> iterator )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      ThrowOSStatus( ::FSCloseIterator( iterator.Release() ) );
     }

   FSGetCatalogInfoBulk_Result FSGetCatalogInfoBulk( FSIterator          iterator,
                                                     ItemCount           maximumObjects,
                                                     FSCatalogInfoBitmap whichInfo,
                                                     FSCatalogInfo *     catalogInfos,
                                                     FSRef *             refs,
                                                     FSSpec *            specs,
                                                     HFSUniStr255 *      names )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSGetCatalogInfoBulk_Result result;
      ::Boolean containerChanged = false;    // Prior to 10.2, FSGetCatalogInfoBulk leaves containerChanged unchanged.
      OSStatus error = ::FSGetCatalogInfoBulk( iterator,
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

   FSCatalogSearch_Result FSCatalogSearch( FSIterator            iterator,
                                           const FSSearchParams& searchCriteria,
                                           ItemCount             maximumObjects,
                                           FSCatalogInfoBitmap   whichInfo,
                                           FSCatalogInfo *       catalogInfos,
                                           FSRef *               refs,
                                           FSSpec *              specs,
                                           HFSUniStr255 *        names )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSCatalogSearch_Result result;
      ::Boolean containerChanged;
      OSStatus error = ::FSCatalogSearch( iterator,
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

   Owned<FSForkRef> FSCreateFork( const FSRef&   ref,
                                  UniCharCount   forkNameLength,
                                  const UniChar *forkName )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      ThrowOSStatus( ::FSCreateFork( &ref, forkNameLength, forkName ) );
      return Owned<FSForkRef>::Seize( FSForkRef( ref, forkNameLength, forkName ) );
     }

   Owned<FSForkRef> FSCreateFork( const FSRef&     ref,
                                  const UniString& forkName )
     {
      return FSCreateFork( ref, forkName.size(), forkName.data() );
     }
                                  
   void FSDeleteFork( Owned<FSForkRef> fork )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      OSStatus error = ::FSDeleteFork( &fork.Get().File(),
                                       fork.Get().Name().size(),
                                       fork.Get().Name().data() );
      fork.Release();
      ThrowOSStatus( error );
     }

   FSIterateForks_Result FSIterateForks( const FSRef&    ref,
                                         CatPositionRec& forkIterator )
     {
      OnlyOnce<RegisterFileManagerErrors>();
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

   Owned<FSForkRefNum> FSOpenFork( const FSRef&   ref,
                                   UniCharCount   forkNameLength,
                                   const UniChar *forkName,
                                   FSIOPermssn    permissions )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      SInt16 result;
      ThrowOSStatus( ::FSOpenFork( &ref, forkNameLength, forkName, permissions, &result ) );
      return Owned<FSForkRefNum>::Seize( FSForkRefNum( result ) );
     }

   Owned<FSForkRefNum> FSOpenFork( const FSRef&    ref,
                                   const UniString forkName,
                                   FSIOPermssn     permissions )
     {
      return FSOpenFork( ref, forkName.size(), forkName.data(), permissions );
     }

   Owned<FSForkRefNum> FSOpenFork( const FSForkRef& fork,
                                   FSIOPermssn      permissions )
     {
      return FSOpenFork( fork.File(), fork.Name(), permissions );
     }

   ByteCount FSReadFork( FSForkRefNum fork,
                         FSIOPosMode  positionMode,
                         SInt64       positionOffset,
                         ByteCount    requestCount,
                         void *       buffer )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      ByteCount actualCount;
      ThrowOSStatus( ::FSReadFork( fork,
                                   positionMode,
                                   positionOffset,
                                   requestCount,
                                   buffer,
                                   &actualCount ) );
      return actualCount;
     }

   ByteCount FSWriteFork( FSForkRefNum fork,
                          FSIOPosMode  positionMode,
                          SInt64       positionOffset,
                          ByteCount    requestCount,
                          const void * buffer )
     {
      OnlyOnce<RegisterFileManagerErrors>();
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
      OnlyOnce<RegisterFileManagerErrors>();
      SInt64 position;
      ThrowOSStatus( ::FSGetForkPosition( forkRefNum, &position ) );
      return position;
     }
   
   void FSSetForkPosition( FSForkRefNum forkRefNum,
                           FSIOPosMode  positionMode,
                           SInt64       positionOffset )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      ThrowOSStatus( ::FSSetForkPosition( forkRefNum, positionMode, positionOffset ) );
     }

   SInt64 FSGetForkSize( FSForkRefNum forkRefNum )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      SInt64 position;
      ThrowOSStatus( ::FSGetForkSize( forkRefNum, &position ) );
      return position;
     }
   
   void FSSetForkSize( FSForkRefNum forkRefNum,
                       FSIOPosMode  positionMode,
                       SInt64       positionOffset )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      ThrowOSStatus( ::FSSetForkSize( forkRefNum, positionMode, positionOffset ) );
     }

   UInt64 FSAllocateFork( FSForkRefNum      forkRefNum,
                          FSAllocationFlags flags,
                          FSIOPosMode       positionMode,
                          SInt64            positionOffset,
                          UInt64            requestCount )
     {
      OnlyOnce<RegisterFileManagerErrors>();
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
      OnlyOnce<RegisterFileManagerErrors>();
      ThrowOSStatus( ::FSFlushFork( forkRefNum ) );
     }
   
   void FSCloseFork( Owned<FSForkRefNum> forkRefNum )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      ThrowOSStatus( ::FSCloseFork( forkRefNum.Release() ) );
     }
   
   FSGetForkCBInfo_Result FSGetForkCBInfo( FSForkRefNum desiredRefNum )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSGetForkCBInfo_Result result;
      SInt16 actualRefNum;
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
      OnlyOnce<RegisterFileManagerErrors>();
      FSGetForkCBInfo_Result result;
      SInt16 actualRefNum;
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
      OnlyOnce<RegisterFileManagerErrors>();
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
	
	
	static void PBHGetVInfoSync( HParamBlockRec& pb )
	{
		ThrowOSStatus( ::PBHGetVInfoSync( &pb ) );
	}
	
	FSVolumeRefNum FSGetVolumeRefNum( FSVolumeIndex volumeIndex )
	{
	#if TARGET_CPU_PPC
		
		if ( TARGET_API_MAC_CARBON  ||  TARGET_CPU_PPC  &&  ::FSGetVolumeInfo != NULL )
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
			OnlyOnce< RegisterFileManagerErrors >();
			
			HParamBlockRec pb;
			
			pb.volumeParam.ioNamePtr = NULL;
			pb.volumeParam.ioVRefNum = 0;
			pb.volumeParam.ioVolIndex = volumeIndex;
			
			PBHGetVInfoSync( pb );
			
			return FSVolumeRefNum( pb.volumeParam.ioVRefNum );
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
      OnlyOnce<RegisterFileManagerErrors>();
      ThrowOSStatus( ::FSSetVolumeInfo( volume, whichInfo, &volumeInfo ) );
     }
   
   HFSUniStr255 FSGetDataForkName()
     {
      OnlyOnce<RegisterFileManagerErrors>();
      HFSUniStr255 result;
      ThrowOSStatus( ::FSGetDataForkName( &result ) );
      return result;
     }
   
   HFSUniStr255 FSGetResourceForkName()
     {
      OnlyOnce<RegisterFileManagerErrors>();
      HFSUniStr255 result;
      ThrowOSStatus( ::FSGetResourceForkName( &result ) );
      return result;
     }

   void FSRefMakePath( const FSRef& ref,
                       UInt8 *      path,
                       UInt32       maxPathSize )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      ThrowOSStatus( ::FSRefMakePath( &ref, path, maxPathSize ) );
     }
   
   std::string FSRefMakePath( const FSRef& ref )
     {
      for ( UInt32 maxPathSize = 1024; true; maxPathSize *= 2 )
        {
         try
           {
            std::string result( maxPathSize, '\0' );
            FSRefMakePath( ref, reinterpret_cast<UInt8 *>( &result[0] ), result.size() );
            result.erase( std::strlen( result.c_str() ) );
            return result;
           }
         catch ( const PathTooLongErr& )
           {
            if ( maxPathSize > std::numeric_limits<UInt32>::max() / 2 )
               throw;
            // Otherwise, try again with a larger string
           }
         catch ( const BuffersTooSmall& )
           {
            if ( maxPathSize > std::numeric_limits<UInt32>::max() / 2 )
               throw;
            // Otherwise, try again with a larger string
           }
        }
     }
   
   FSPathMakeRef_Result FSPathMakeRef( const UInt8 *path )
     {
      OnlyOnce<RegisterFileManagerErrors>();
      FSPathMakeRef_Result result;
      ::Boolean isDirectory;
      ThrowOSStatus( ::FSPathMakeRef( path, &result.ref, &isDirectory ) );
      result.isDirectory = isDirectory;
      return result;
     }
   
   FSPathMakeRef_Result FSPathMakeRef( const std::string& path )
     {
      return FSPathMakeRef( reinterpret_cast< const UInt8* >( path.c_str() ) );
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
