// Aliases.h

#ifndef NITROGEN_ALIASES_H
#define NITROGEN_ALIASES_H

#ifndef __ALIASES__
#include <Aliases.h>
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif

namespace Nitrogen
  {
   void RegisterAliasManagerErrors();
   
   class MountFlags_Tag {};
   typedef FlagType< MountFlags_Tag, unsigned long, 0 > MountFlags;
   
   template <> struct OwnedDefaults< AliasHandle >           { typedef Disposer<Handle> Disposer; };
      
   Owned<AliasHandle> FSNewAlias( const FSRef& fromFile,
                                  const FSRef& target );
   
   Owned<AliasHandle> FSNewAlias( const FSRef& target );
   
   template <> struct Converter< Owned<AliasHandle>, FSRef >: public std::unary_function< FSRef, Owned<AliasHandle> >
     {
      Owned<AliasHandle> operator()( const FSRef& ref ) const
        {
         return FSNewAlias( ref );
        }
     };
   
   Owned<AliasHandle> FSNewAliasMinimal( const FSRef& target );
   
   struct FSIsAliasFile_Result
     {
      bool aliasFileFlag;
      bool folderFlag;
      
      operator bool() const     { return aliasFileFlag; }
     };
   
   FSIsAliasFile_Result FSIsAliasFile( const FSRef& fileRef );
 
   struct FSResolveAlias_Result
     {
      FSRef target;
      bool wasChanged;
      
      operator const FSRef&() const    { return target; }
     };
  
   typedef FSResolveAlias_Result FSResolveAliasWithMountFlags_Result;
   
   FSResolveAliasWithMountFlags_Result FSResolveAliasWithMountFlags( const FSRef&  fromFile,
                                                                     AliasHandle   inAlias,
                                                                     MountFlags    mountFlags );

   FSResolveAliasWithMountFlags_Result FSResolveAliasWithMountFlags( AliasHandle   inAlias,
                                                                     MountFlags    mountFlags );
   
   FSResolveAlias_Result FSResolveAlias( const FSRef&  fromFile,
                                         AliasHandle   inAlias );

   FSResolveAlias_Result FSResolveAlias( AliasHandle   inAlias );

   template <> struct Converter< FSRef, AliasHandle >: public std::unary_function< AliasHandle, FSRef >
     {
      FSRef operator()( AliasHandle alias ) const
        {
         return FSResolveAlias( alias );
        }
     };
 
   struct FSResolveAliasFile_Result
     {
      FSRef theRef;
      bool  targetIsFolder;
      bool  wasAliased;
      
      operator const FSRef&() const    { return theRef; }
     };
   
   typedef FSResolveAliasFile_Result FSResolveAliasFileWithMountFlags_Result;
   
   FSResolveAliasFileWithMountFlags_Result FSResolveAliasFileWithMountFlags( const FSRef& theRef,
                                                                             bool         resolveAliasChains,
                                                                             MountFlags   mountFlags );
   
   FSResolveAliasFile_Result FSResolveAliasFile( const FSRef& theRef,
                                                 bool         resolveAliasChains = true );
   
   typedef FSResolveAlias_Result FSFollowFinderAlias_Result;
   
   FSFollowFinderAlias_Result FSFollowFinderAlias( FSRef        fromFile,
                                                   AliasHandle  alias,
                                                   bool         logon );
         // fromFIle is passed by value because the Carbon function takes pointer to a
         // non-const FSRef.  I think that's an error in the header, but prefer to be safe.
   
            
   FSFollowFinderAlias_Result FSFollowFinderAlias( AliasHandle  alias,
                                                   bool         logon );
    
   bool FSUpdateAlias( const FSRef& fromFile,
                       const FSRef& target,
                       AliasHandle  alias );

   bool FSUpdateAlias( const FSRef& target,
                       AliasHandle  alias );
  }

#endif
