// CFURL.h

#ifndef NITROGEN_CFURL_H
#define NITROGEN_CFURL_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFURL__
#include FRAMEWORK_HEADER(CoreFoundation,CFURL.h)
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif
#ifndef NITROGEN_CFDATA_H
#include "Nitrogen/CFData.h"
#endif

#include <string>

namespace Nitrogen
  {
   using ::CFURLPathStyle;
   using ::CFURLRef;
   
   using ::CFURLGetTypeID;
   
   template <> struct CFType_Traits< CFURLRef >: Basic_CFType_Traits< CFURLRef, ::CFURLGetTypeID > {};

   template <> struct OwnedDefaults< CFURLRef >: OwnedDefaults< CFTypeRef >  {};

   class CFURLCreateWithBytes_Failed {};
   Owned<CFURLRef> CFURLCreateWithBytes( CFAllocatorRef     allocator,
                                         const UInt8 *      URLBytes,
                                         std::size_t        length,
                                         CFStringEncoding   encoding,
                                         CFURLRef           baseURL = 0 );

   inline Owned<CFURLRef> CFURLCreateWithBytes( const UInt8 *      URLBytes,
                                                std::size_t        length,
                                                CFStringEncoding   encoding,
                                                CFURLRef           baseURL = 0 )
     {
      return Nitrogen::CFURLCreateWithBytes( kCFAllocatorDefault, URLBytes, length, encoding, baseURL );
     }

   Owned<CFURLRef> CFURLCreateWithBytes( CFAllocatorRef     allocator,
                                         const std::string& URLBytes,
                                         CFStringEncoding   encoding,
                                         CFURLRef           baseURL = 0 );

   inline Owned<CFURLRef> CFURLCreateWithBytes( const std::string& URLBytes,
                                                CFStringEncoding   encoding,
                                                CFURLRef           baseURL = 0 )
     {
      return Nitrogen::CFURLCreateWithBytes( kCFAllocatorDefault, URLBytes, encoding, baseURL );
     }

   class CFURLCreateData_Failed {};
   Owned<CFDataRef> CFURLCreateData( CFAllocatorRef     allocator,
                                     CFURLRef           url,
                                     CFStringEncoding   encoding,
                                     bool               escapeWhitespace );
   
   inline Owned<CFDataRef> CFURLCreateData( CFURLRef           url,
                                            CFStringEncoding   encoding,
                                            Boolean            escapeWhitespace )
     {
      return Nitrogen::CFURLCreateData( kCFAllocatorDefault, url, encoding, escapeWhitespace );
     }

   class CFURLCreateWithString_Failed {};
   Owned<CFURLRef> CFURLCreateWithString( CFAllocatorRef   allocator,
                                          CFStringRef      URLString,
                                          CFURLRef         baseURL = 0 );

   inline Owned<CFURLRef> CFURLCreateWithString( CFStringRef      URLString,
                                                 CFURLRef         baseURL = 0 )
     {
      return Nitrogen::CFURLCreateWithString( kCFAllocatorDefault, URLString, baseURL );
     }

   class CFURLCreateWithFileSystemPath_Failed {};
   Owned<CFURLRef> CFURLCreateWithFileSystemPath( CFAllocatorRef   allocator,
                                                  CFStringRef      filePath,
                                                  CFURLPathStyle   pathStyle,
                                                  bool             isDirectory );

   inline Owned<CFURLRef> CFURLCreateWithFileSystemPath( CFStringRef      filePath,
                                                         CFURLPathStyle   pathStyle,
                                                         bool             isDirectory )
     {
      return Nitrogen::CFURLCreateWithFileSystemPath( kCFAllocatorDefault, filePath, pathStyle, isDirectory );
     }


   class CFURLCreateFromFileSystemRepresentation_Failed {};
   Owned<CFURLRef> CFURLCreateFromFileSystemRepresentation( CFAllocatorRef   allocator,
                                                            const UInt8 *    buffer,
                                                            CFIndex          bufLen,
                                                            bool             isDirectory );

   inline Owned<CFURLRef> CFURLCreateFromFileSystemRepresentation( const UInt8 *    buffer,
                                                                   CFIndex          bufLen,
                                                                   bool             isDirectory )
     {
      return Nitrogen::CFURLCreateFromFileSystemRepresentation( kCFAllocatorDefault, buffer, bufLen, isDirectory );
     }

   class CFURLCreateWithFileSystemPathRelativeToBase_Failed {};
   Owned<CFURLRef> CFURLCreateWithFileSystemPathRelativeToBase( CFAllocatorRef   allocator,
                                                                CFStringRef      filePath,
                                                                CFURLPathStyle   pathStyle,
                                                                bool             isDirectory,
                                                                CFURLRef         baseURL );

   inline Owned<CFURLRef> CFURLCreateWithFileSystemPathRelativeToBase( CFStringRef      filePath,
                                                                       CFURLPathStyle   pathStyle,
                                                                       bool             isDirectory,
                                                                       CFURLRef         baseURL )
     {
      return Nitrogen::CFURLCreateWithFileSystemPathRelativeToBase( kCFAllocatorDefault, filePath, pathStyle, isDirectory, baseURL );
     }

   class CFURLCreateFromFileSystemRepresentationRelativeToBase_Failed {};
   Owned<CFURLRef> CFURLCreateFromFileSystemRepresentationRelativeToBase( CFAllocatorRef   allocator,
                                                                          const UInt8 *    buffer,
                                                                          CFIndex          bufLen,
                                                                          bool             isDirectory,
                                                                          CFURLRef         baseURL );

   inline Owned<CFURLRef> CFURLCreateFromFileSystemRepresentationRelativeToBase( const UInt8 *    buffer,
                                                                                 CFIndex          bufLen,
                                                                                 bool             isDirectory,
                                                                                 CFURLRef         baseURL )
     {
      return Nitrogen::CFURLCreateFromFileSystemRepresentationRelativeToBase( kCFAllocatorDefault, buffer, bufLen, isDirectory, baseURL );
     }

   class CFURLGetFileSystemRepresentation_Failed {};
   void CFURLGetFileSystemRepresentation( CFURLRef   url,
                                          bool       resolveAgainstBase,
                                          UInt8 *    buffer,
                                          CFIndex    maxBufLen );

   template < CFIndex maxBufLen >
   inline void CFURLGetFileSystemRepresentation( CFURLRef   url,
                                                 bool       resolveAgainstBase,
                                                 UInt8      (&buffer)[maxBufLen] )
     {
      Nitrogen::CFURLGetFileSystemRepresentation( url, resolveAgainstBase, buffer, maxBufLen );
     }

   class CFURLCopyAbsoluteURL_Failed {};
   Owned<CFURLRef> CFURLCopyAbsoluteURL( CFURLRef relativeURL );

   class CFURLGetString_Failed {};
   CFStringRef CFURLGetString( CFURLRef anURL );

   using ::CFURLGetBaseURL;

   /*...*/

   class CFURLCreateFromFSRef_Failed {};
   Owned<CFURLRef> CFURLCreateFromFSRef( CFAllocatorRef   allocator,
                                         const FSRef&     fsRef );
   
   inline Owned<CFURLRef> CFURLCreateFromFSRef( const FSRef& fsRef )
     {
      return Nitrogen::CFURLCreateFromFSRef( kCFAllocatorDefault, fsRef );
     }

   class CFURLGetFSRef_Failed {};
   FSRef CFURLGetFSRef( CFURLRef url );
   
   template <> struct Converter< FSRef, CFURLRef >: public std::unary_function< CFURLRef, FSRef >
     {
      FSRef operator()( CFURLRef url ) const;
     };

   template <> struct Converter< Owned<CFURLRef>, FSRef >: public std::unary_function< FSRef, Owned<CFURLRef> >
     {
      Owned<CFURLRef> operator()( const FSRef& ref ) const
        {
         return CFURLCreateFromFSRef( ref );
        }
     };
  }

#endif
