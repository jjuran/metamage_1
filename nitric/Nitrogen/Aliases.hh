// Nitrogen/Aliases.hh
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott, Joshua Juran, and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_ALIASES_HH
#define NITROGEN_ALIASES_HH

#ifndef __ALIASES__
#include <Aliases.h>
#endif

// iota
#include "iota/string_traits.hh"

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"
#include "nucleus/string.hh"

#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif
#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif
#ifndef NITROGEN_RESOURCES_HH
#include "Nitrogen/Resources.hh"
#endif
#ifndef NITROGEN_CFDATA_HH
#include "Nitrogen/CFData.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( AliasManager );
	
 	
 	static const ResType rAliasType = ResType( ::rAliasType );
   
	template <> struct ResType_Traits< rAliasType > : Handle_ResType_Traits< AliasRecord > {};
	
	enum MountFlags
	{
		kMountFlags_Max = nucleus::enumeration_traits< unsigned long >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( MountFlags )
	
   using ::AliasHandle;
  }

namespace nucleus
  {
   template <> struct disposer_traits< AliasHandle > : disposer_traits< Nitrogen::Handle >  {};
  }

namespace Nitrogen
  {
	// 90
	nucleus::owned< AliasHandle > NewAlias( const FSSpec& fromFile,
	                                        const FSSpec& target );
	
	nucleus::owned< AliasHandle > NewAlias( const FSSpec& target );
	nucleus::owned< AliasHandle > NewAlias( CFDataRef theData );
	
	// 127
	nucleus::owned< AliasHandle > NewAliasMinimalFromFullPath( const char*      fullPath,
	                                                           unsigned         length,
	                                                           ConstStr32Param  zoneName,
	                                                           ConstStr31Param  serverName );
	
	template < class String >
	inline nucleus::owned< AliasHandle >
	//
	NewAliasMinimalFromFullPath( const String&    fullPath,
	                             ConstStr32Param  zoneName   = "\p",
	                             ConstStr31Param  serverName = "\p" )
	{
		return NewAliasMinimalFromFullPath( iota::get_string_data( fullPath ),
		                                    iota::get_string_size( fullPath ),
		                                    zoneName,
		                                    serverName );
	}
	
	struct ResolveAlias_Result
	{
		FSSpec target;
		bool wasChanged;
		
		operator const FSSpec&() const    { return target; }
	};
	
	// 148
	ResolveAlias_Result ResolveAlias( const FSSpec&  fromFile,
	                                  AliasHandle    alias );
	
	ResolveAlias_Result ResolveAlias( AliasHandle    alias );
	
	struct ResolveAliasFile_Result
	{
		FSSpec target;
		bool targetIsFolder;
		bool wasAliased;
		
		operator const FSSpec&() const    { return target; }
	};
	
	// 221
	ResolveAliasFile_Result ResolveAliasFile( const FSSpec&  target,
	                                          bool           resolveAliasChains );
	
   nucleus::owned<AliasHandle> FSNewAlias( const FSRef& fromFile,
                                           const FSRef& target );
   
   nucleus::owned<AliasHandle> FSNewAlias( const FSRef& target );
  }

namespace nucleus
  {   
   template <> struct converter< nucleus::owned< AliasHandle >, FSRef >
     {
      typedef FSRef                          argument_type;
      typedef nucleus::owned< AliasHandle >  result_type;
      
      nucleus::owned< AliasHandle > operator()( const FSRef& ref ) const
        {
         return Nitrogen::FSNewAlias( ref );
        }
     };
  }

namespace Nitrogen
  {   
   nucleus::owned<AliasHandle> FSNewAliasMinimal( const FSRef& target );
   
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

   FSResolveAlias_Result FSResolveAlias( CFDataRef alias );

   struct AliasAsDataRef_Failed {};
   nucleus::owned<CFDataRef> AliasAsDataRef ( CFAllocatorRef allocator, AliasHandle inAlias );
  }

namespace nucleus
  {
   template <> struct converter< FSRef, AliasHandle >
     {
      typedef AliasHandle  argument_type;
      typedef FSRef        result_type;
      
      FSRef operator()( AliasHandle alias ) const
        {
         return Nitrogen::FSResolveAlias( alias );
        }
     };
  }

namespace Nitrogen
  { 
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
