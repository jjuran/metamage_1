// Nitrogen/Aliases.cc
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2006 by Lisa Lippincott, Joshua Juran, and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/Aliases.hh"


namespace Nitrogen
  {
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( AliasManager )
	
	
	static void RegisterAliasManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class AliasManagerErrorsRegistration
	{
		public:
			AliasManagerErrorsRegistration()  { RegisterAliasManagerErrors(); }
	};
	
	static AliasManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	static nucleus::owned< AliasHandle > NewAlias( const FSSpec* fromFile, const FSSpec& target )
	{
		::AliasHandle result;
		ThrowOSStatus( ::NewAlias( fromFile, &target, &result ) );
		
		return nucleus::owned< AliasHandle >::seize( result );
	}
	
	nucleus::owned< AliasHandle > NewAlias( const FSSpec&  fromFile,
	                                        const FSSpec&  target )
	{
		return NewAlias( &fromFile, target );
	}
	
	nucleus::owned< AliasHandle > NewAlias( const FSSpec& target )
	{
		return NewAlias( NULL, target );
	}
	
	nucleus::owned< AliasHandle > NewAlias( CFDataRef theData )
	{
	   CFIndex dataSize = CFDataGetLength ( theData );
	   nucleus::owned<AliasHandle> retVal = NewHandle<AliasRecord> ( dataSize );
	   CFDataGetBytes ( theData, CFRangeMake ( 0, dataSize ), (UInt8 *) *retVal );
	   return retVal;
	}


	nucleus::owned< AliasHandle > NewAliasMinimalFromFullPath( const char*      fullPath,
	                                                           unsigned         length,
	                                                           ConstStr32Param  zoneName,
	                                                           ConstStr31Param  serverName )
	{
		::AliasHandle result;
		ThrowOSStatus( ::NewAliasMinimalFromFullPath( length,
		                                              fullPath,
		                                              zoneName,
		                                              serverName,
		                                              &result ) );
		
		return nucleus::owned< AliasHandle >::seize( result );
	}
	
	ResolveAlias_Result ResolveAlias( const FSSpec&  fromFile,
	                                  AliasHandle    alias )
	{
		::Boolean wasChanged;
		ResolveAlias_Result result;
		
		ThrowOSStatus( ::ResolveAlias( &fromFile,
		                               alias,
		                               &result.target,
		                               &wasChanged ) );
		
		result.wasChanged = wasChanged;
		
		return result;
	}
	
	ResolveAlias_Result ResolveAlias( AliasHandle alias )
	{
		::Boolean wasChanged;
		ResolveAlias_Result result;
		
		ThrowOSStatus( ::ResolveAlias( NULL,
		                               alias,
		                               &result.target,
		                               &wasChanged ) );
		
		result.wasChanged = wasChanged;
		
		return result;
	}
	
	ResolveAliasFile_Result ResolveAliasFile( const FSSpec& target,
	                                          bool resolveAliasChains )
	{
		ResolveAliasFile_Result result;
		result.target = target;
		
		::Boolean targetIsFolder;
		::Boolean wasAliased;
		
		ThrowOSStatus( ::ResolveAliasFile( &result.target,
		                                   resolveAliasChains,
		                                   &targetIsFolder,
		                                   &wasAliased ) );
		
		result.targetIsFolder = targetIsFolder;
		result.wasAliased     = wasAliased;
		
		return result;
	}
	
   nucleus::owned<AliasHandle> FSNewAlias( const FSRef& fromFile,
                                           const FSRef& target )
     {
      AliasHandle result;
      ThrowOSStatus( ::FSNewAlias( &fromFile, &target, &result ) );
      return nucleus::owned<AliasHandle>::seize( result );
     }
   
   nucleus::owned<AliasHandle> FSNewAlias( const FSRef& target )
     {
      AliasHandle result;
      ThrowOSStatus( ::FSNewAlias( 0, &target, &result ) );
      return nucleus::owned<AliasHandle>::seize( result );
     }

   nucleus::owned<AliasHandle> FSNewAliasMinimal( const FSRef& target )
     {
      AliasHandle result;
      ThrowOSStatus( ::FSNewAliasMinimal( &target, &result ) );
      return nucleus::owned<AliasHandle>::seize( result );
     }
  
   FSIsAliasFile_Result FSIsAliasFile( const FSRef& fileRef )
     {
      ::Boolean aliasFileFlag;
      ::Boolean folderFlag;
      ThrowOSStatus( ::FSIsAliasFile( &fileRef, &aliasFileFlag, &folderFlag ) );
      
      FSIsAliasFile_Result result;
      result.aliasFileFlag = aliasFileFlag;
      result.folderFlag = folderFlag;
      return result;
     }
   
   FSResolveAliasWithMountFlags_Result FSResolveAliasWithMountFlags( const FSRef&  fromFile,
                                                                     AliasHandle   inAlias,
                                                                     MountFlags    mountFlags )
     {
      FSResolveAliasWithMountFlags_Result result;
      ::Boolean wasChanged;
      ThrowOSStatus( ::FSResolveAliasWithMountFlags( &fromFile, inAlias, &result.target, &wasChanged, mountFlags ) );
      result.wasChanged = wasChanged;
      return result;
     }

   FSResolveAliasWithMountFlags_Result FSResolveAliasWithMountFlags( AliasHandle   inAlias,
                                                                     MountFlags    mountFlags )
     {
      FSResolveAliasWithMountFlags_Result result;
      ::Boolean wasChanged;
      ThrowOSStatus( ::FSResolveAliasWithMountFlags( 0, inAlias, &result.target, &wasChanged, mountFlags ) );
      result.wasChanged = wasChanged;
      return result;
     }
   
   FSResolveAlias_Result FSResolveAlias( const FSRef&  fromFile,
                                         AliasHandle   inAlias )
     {
      FSResolveAlias_Result result;
      ::Boolean wasChanged;
      ThrowOSStatus( ::FSResolveAlias( &fromFile, inAlias, &result.target, &wasChanged ) );
      result.wasChanged = wasChanged;
      return result;
     }

   FSResolveAlias_Result FSResolveAlias( AliasHandle inAlias )
     {
      FSResolveAlias_Result result;
      ::Boolean wasChanged;
      ThrowOSStatus( ::FSResolveAlias( 0, inAlias, &result.target, &wasChanged ) );
      result.wasChanged = wasChanged;
      return result;
     }
   
//	Resolve an alias inside a CFDataRef. Copy the data into a handle.
	FSResolveAlias_Result FSResolveAlias ( CFDataRef alias )
	  {
	   CFIndex dataSize = CFDataGetLength ( alias );
	   nucleus::owned<Handle> h = NewHandle ( dataSize );
	   CFDataGetBytes ( alias, CFRangeMake ( 0, dataSize ), (UInt8 *) *h.get () );
	   return FSResolveAlias ((AliasHandle) h.get ().Get() );
	  }


   nucleus::owned<CFDataRef> AliasAsDataRef ( CFAllocatorRef allocator, AliasHandle inAlias )
     {
      CFDataRef result = ::CFDataCreate ( allocator, (UInt8*) *inAlias, GetHandleSize((Handle) inAlias ));
      if ( NULL == result )
        throw AliasAsDataRef_Failed ();
      return nucleus::owned<CFDataRef>::seize ( result );
     }


   FSResolveAliasFileWithMountFlags_Result FSResolveAliasFileWithMountFlags( const FSRef& theRef,
                                                                             bool         resolveAliasChains,
                                                                             MountFlags   mountFlags )
     {
      FSResolveAliasFileWithMountFlags_Result result;
      result.theRef = theRef;
      ::Boolean targetIsFolder;
      ::Boolean wasAliased;
      ThrowOSStatus( ::FSResolveAliasFileWithMountFlags( &result.theRef, resolveAliasChains, &targetIsFolder, &wasAliased, mountFlags ) );
      result.targetIsFolder = targetIsFolder;
      result.wasAliased = wasAliased;
      return result;
     }

   FSResolveAliasFile_Result FSResolveAliasFile( const FSRef& theRef,
                                                 bool         resolveAliasChains )

     {
      FSResolveAliasFile_Result result;
      result.theRef = theRef;
      ::Boolean targetIsFolder;
      ::Boolean wasAliased;
      ThrowOSStatus( ::FSResolveAliasFile( &result.theRef, resolveAliasChains, &targetIsFolder, &wasAliased ) );
      result.targetIsFolder = targetIsFolder;
      result.wasAliased = wasAliased;
      return result;
     }
      
   FSFollowFinderAlias_Result FSFollowFinderAlias( FSRef        fromFile,
                                                   AliasHandle  alias,
                                                   bool         logon )
     {
      FSFollowFinderAlias_Result result;
      ::Boolean wasChanged;
      ThrowOSStatus( ::FSFollowFinderAlias( &fromFile, alias, logon, &result.target, &wasChanged ) );
      result.wasChanged = wasChanged;
      return result;
     }

   FSFollowFinderAlias_Result FSFollowFinderAlias( AliasHandle  alias,
                                                   bool         logon )
     {
      FSFollowFinderAlias_Result result;
      ::Boolean wasChanged;
      ThrowOSStatus( ::FSFollowFinderAlias( 0, alias, logon, &result.target, &wasChanged ) );
      result.wasChanged = wasChanged;
      return result;
     }

   bool FSUpdateAlias( const FSRef& fromFile,
                       const FSRef& target,
                       AliasHandle  alias )
     {
      ::Boolean result;
      ThrowOSStatus( ::FSUpdateAlias( &fromFile, &target, alias, &result ) );
      return result;
     }

   bool FSUpdateAlias( const FSRef& target,
                       AliasHandle  alias )
     {
      ::Boolean result;
      ThrowOSStatus( ::FSUpdateAlias( 0, &target, alias, &result ) );
      return result;
     }
   
   void RegisterAliasManagerErrors()
     {
      RegisterOSStatus< nsvErr          >();
      RegisterOSStatus< fnfErr          >();
      RegisterOSStatus< paramErr        >();
      RegisterOSStatus< dirNFErr        >();
      RegisterOSStatus< userCanceledErr >();
     }
  }
