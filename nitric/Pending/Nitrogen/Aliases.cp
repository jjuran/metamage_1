// Aliases.cp

#ifndef NITROGEN_ALIASES_H
#include "Nitrogen/Aliases.h"
#endif

namespace Nitrogen
  {
	
	static Owned< AliasHandle > NewAlias( const FSSpec* fromFile, const FSSpec& target )
	{
		OnlyOnce< RegisterAliasManagerErrors >();
		
		AliasHandle result;
		ThrowOSStatus( ::NewAlias( fromFile, &target, &result ) );
		return Owned< AliasHandle >::Seize( result );
	}
	
	Owned< AliasHandle > NewAlias( const FSSpec& fromFile, const FSSpec& target )
	{
		return NewAlias( &fromFile, target );
	}
	
	Owned< AliasHandle > NewAlias( const FSSpec& target )
	{
		return NewAlias( NULL, target );
	}
	
	Owned< AliasHandle > NewAliasMinimalFromFullPath( const std::string&  fullPath,
	                                                  ConstStr32Param     zoneName,
	                                                  ConstStr31Param     serverName )
	{
		OnlyOnce< RegisterAliasManagerErrors >();
		
		::AliasHandle result;
		ThrowOSStatus( ::NewAliasMinimalFromFullPath( fullPath.size(), fullPath.data(), zoneName, serverName, &result ) );
		return Owned< AliasHandle >::Seize( result );
	}
	
	ResolveAlias_Result ResolveAlias( const FSSpec&  fromFile,
	                                  AliasHandle    alias )
	{
		OnlyOnce< RegisterAliasManagerErrors >();
		
		::Boolean wasChanged;
		ResolveAlias_Result result;
		ThrowOSStatus( ::ResolveAlias( &fromFile, alias, &result.target, &wasChanged ) );
		return result;
	}
	
	ResolveAlias_Result ResolveAlias( AliasHandle    alias )
	{
		OnlyOnce< RegisterAliasManagerErrors >();
		
		::Boolean wasChanged;
		ResolveAlias_Result result;
		ThrowOSStatus( ::ResolveAlias( NULL, alias, &result.target, &wasChanged ) );
		return result;
	}
	
	ResolveAliasFile_Result ResolveAliasFile( const FSSpec& target, bool resolveAliasChains )
	{
		OnlyOnce< RegisterAliasManagerErrors >();
		
		ResolveAliasFile_Result result;
		result.target = target;
		::Boolean targetIsFolder;
		::Boolean wasAliased;
		
		ThrowOSStatus( ::ResolveAliasFile( &result.target, resolveAliasChains, &targetIsFolder, &wasAliased ) );
		
		result.targetIsFolder = targetIsFolder;
		result.wasAliased = wasAliased;
		
		return result;
	}
	
   Owned<AliasHandle> FSNewAlias( const FSRef& fromFile,
                                  const FSRef& target )
     {
      OnlyOnce<RegisterAliasManagerErrors>();
      AliasHandle result;
      ThrowOSStatus( ::FSNewAlias( &fromFile, &target, &result ) );
      return Owned<AliasHandle>::Seize( result );
     }
   
   Owned<AliasHandle> FSNewAlias( const FSRef& target )
     {
      OnlyOnce<RegisterAliasManagerErrors>();
      AliasHandle result;
      ThrowOSStatus( ::FSNewAlias( 0, &target, &result ) );
      return Owned<AliasHandle>::Seize( result );
     }

   Owned<AliasHandle> FSNewAliasMinimal( const FSRef& target )
     {
      OnlyOnce<RegisterAliasManagerErrors>();
      AliasHandle result;
      ThrowOSStatus( ::FSNewAliasMinimal( &target, &result ) );
      return Owned<AliasHandle>::Seize( result );
     }
  
   FSIsAliasFile_Result FSIsAliasFile( const FSRef& fileRef )
     {
      OnlyOnce<RegisterAliasManagerErrors>();
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
      OnlyOnce<RegisterAliasManagerErrors>();
      FSResolveAliasWithMountFlags_Result result;
      ::Boolean wasChanged;
      ThrowOSStatus( ::FSResolveAliasWithMountFlags( &fromFile, inAlias, &result.target, &wasChanged, mountFlags ) );
      result.wasChanged = wasChanged;
      return result;
     }

   FSResolveAliasWithMountFlags_Result FSResolveAliasWithMountFlags( AliasHandle   inAlias,
                                                                     MountFlags    mountFlags )
     {
      OnlyOnce<RegisterAliasManagerErrors>();
      FSResolveAliasWithMountFlags_Result result;
      ::Boolean wasChanged;
      ThrowOSStatus( ::FSResolveAliasWithMountFlags( 0, inAlias, &result.target, &wasChanged, mountFlags ) );
      result.wasChanged = wasChanged;
      return result;
     }
   
   FSResolveAlias_Result FSResolveAlias( const FSRef&  fromFile,
                                         AliasHandle   inAlias )
     {
      OnlyOnce<RegisterAliasManagerErrors>();
      FSResolveAlias_Result result;
      ::Boolean wasChanged;
      ThrowOSStatus( ::FSResolveAlias( &fromFile, inAlias, &result.target, &wasChanged ) );
      result.wasChanged = wasChanged;
      return result;
     }

   FSResolveAlias_Result FSResolveAlias( AliasHandle inAlias )
     {
      OnlyOnce<RegisterAliasManagerErrors>();
      FSResolveAlias_Result result;
      ::Boolean wasChanged;
      ThrowOSStatus( ::FSResolveAlias( 0, inAlias, &result.target, &wasChanged ) );
      result.wasChanged = wasChanged;
      return result;
     }
   
   FSResolveAliasFileWithMountFlags_Result FSResolveAliasFileWithMountFlags( const FSRef& theRef,
                                                                             bool         resolveAliasChains,
                                                                             MountFlags   mountFlags )
     {
      OnlyOnce<RegisterAliasManagerErrors>();
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
      OnlyOnce<RegisterAliasManagerErrors>();
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
      OnlyOnce<RegisterAliasManagerErrors>();
      FSFollowFinderAlias_Result result;
      ::Boolean wasChanged;
      ThrowOSStatus( ::FSFollowFinderAlias( &fromFile, alias, logon, &result.target, &wasChanged ) );
      result.wasChanged = wasChanged;
      return result;
     }

   FSFollowFinderAlias_Result FSFollowFinderAlias( AliasHandle  alias,
                                                   bool         logon )
     {
      OnlyOnce<RegisterAliasManagerErrors>();
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
      OnlyOnce<RegisterAliasManagerErrors>();
      ::Boolean result;
      ThrowOSStatus( ::FSUpdateAlias( &fromFile, &target, alias, &result ) );
      return result;
     }

   bool FSUpdateAlias( const FSRef& target,
                       AliasHandle  alias )
     {
      OnlyOnce<RegisterAliasManagerErrors>();
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
