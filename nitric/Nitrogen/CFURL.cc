// Nitrogen/CFURL.cc
// -----------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/CFURL.hh"

#ifndef __FILES__
#include <Files.h>
#endif


namespace Nitrogen
  {
   nucleus::owned<CFURLRef> CFURLCreateWithBytes( CFAllocatorRef     allocator,
                                         const UInt8 *      URLBytes,
                                         std::size_t        length,
                                         CFStringEncoding   encoding,
                                         CFURLRef           baseURL )
     {
      CFURLRef result = ::CFURLCreateWithBytes( allocator,
                                                URLBytes,
                                                static_cast<CFIndex>( length ),
                                                encoding,
                                                baseURL );
      if ( result == 0 )
         throw CFURLCreateWithBytes_Failed();
      return nucleus::owned<CFURLRef>::seize( result );
     }

   nucleus::owned<CFDataRef> CFURLCreateData( CFAllocatorRef     allocator,
                                     CFURLRef           url,
                                     CFStringEncoding   encoding,
                                     bool               escapeWhitespace )
     {
      CFDataRef result = ::CFURLCreateData( allocator,
                                            url,
                                            encoding,
                                            escapeWhitespace );
      if ( result == 0 )
         throw CFURLCreateData_Failed();
      return nucleus::owned<CFDataRef>::seize( result );
     }
	
   nucleus::owned<CFURLRef> CFURLCreateWithString( CFAllocatorRef   allocator,
                                          CFStringRef      URLString,
                                          CFURLRef         baseURL )
     {
      CFURLRef result = ::CFURLCreateWithString( allocator, URLString, baseURL );
      if ( result == 0 )
         throw CFURLCreateWithString_Failed();
      return nucleus::owned<CFURLRef>::seize( result );
     }

   nucleus::owned<CFURLRef> CFURLCreateWithFileSystemPath( CFAllocatorRef   allocator,
                                                  CFStringRef      filePath,
                                                  CFURLPathStyle   pathStyle,
                                                  bool             isDirectory )
     {
      CFURLRef result = ::CFURLCreateWithFileSystemPath( allocator, filePath, pathStyle, isDirectory );
      if ( result == 0 )
         throw CFURLCreateWithFileSystemPath_Failed();
      return nucleus::owned<CFURLRef>::seize( result );
     }

   nucleus::owned<CFURLRef> CFURLCreateFromFileSystemRepresentation( CFAllocatorRef   allocator,
                                                            const UInt8 *    buffer,
                                                            CFIndex          bufLen,
                                                            bool             isDirectory )
     {
      CFURLRef result = ::CFURLCreateFromFileSystemRepresentation( allocator, buffer, bufLen, isDirectory );
      if ( result == 0 )
         throw CFURLCreateFromFileSystemRepresentation_Failed();
      return nucleus::owned<CFURLRef>::seize( result );
     }

   nucleus::owned<CFURLRef> CFURLCreateWithFileSystemPathRelativeToBase( CFAllocatorRef   allocator,
                                                                CFStringRef      filePath,
                                                                CFURLPathStyle   pathStyle,
                                                                bool             isDirectory,
                                                                CFURLRef         baseURL )
     {
      CFURLRef result = ::CFURLCreateWithFileSystemPathRelativeToBase( allocator, filePath, pathStyle, isDirectory, baseURL );
      if ( result == 0 )
         throw CFURLCreateWithFileSystemPathRelativeToBase_Failed();
      return nucleus::owned<CFURLRef>::seize( result );
     }

   nucleus::owned<CFURLRef> CFURLCreateFromFileSystemRepresentationRelativeToBase( CFAllocatorRef   allocator,
                                                                          const UInt8 *    buffer,
                                                                          CFIndex          bufLen,
                                                                          bool             isDirectory,
                                                                          CFURLRef         baseURL )
     {
      CFURLRef result = ::CFURLCreateFromFileSystemRepresentationRelativeToBase( allocator, buffer, bufLen, isDirectory, baseURL );
      if ( result == 0 )
         throw CFURLCreateFromFileSystemRepresentationRelativeToBase_Failed();
      return nucleus::owned<CFURLRef>::seize( result );
     }

   void CFURLGetFileSystemRepresentation( CFURLRef   url,
                                          bool       resolveAgainstBase,
                                          UInt8 *    buffer,
                                          CFIndex    maxBufLen )
     {
      if ( !::CFURLGetFileSystemRepresentation( url, resolveAgainstBase, buffer, maxBufLen ) )
         throw CFURLGetFileSystemRepresentation_Failed();
     }

   nucleus::owned<CFURLRef> CFURLCopyAbsoluteURL( CFURLRef relativeURL )
     {
      CFURLRef result = ::CFURLCopyAbsoluteURL( relativeURL );
      if ( result == 0 )
         throw CFURLCopyAbsoluteURL_Failed();
      return nucleus::owned<CFURLRef>::seize( result );
     }

   CFStringRef CFURLGetString( CFURLRef anURL )
     {
      CFStringRef result = ::CFURLGetString( anURL );
      if ( result == 0 )
         throw CFURLGetString_Failed();
      return result;
     }

   nucleus::owned<CFURLRef> CFURLCreateFromFSRef( CFAllocatorRef   allocator,
                                         const FSRef&     fsRef )
     {
      CFURLRef result = ::CFURLCreateFromFSRef( allocator, &fsRef );
      if ( result == 0 )
         throw CFURLCreateFromFSRef_Failed();
      return nucleus::owned<CFURLRef>::seize( result );
     }

   FSRef CFURLGetFSRef( CFURLRef url )
     {
      FSRef result;
      if ( !CFURLGetFSRef( url, &result ) )
         throw CFURLGetFSRef_Failed();
      return result;
     }
   }

namespace nucleus
   {
   FSRef converter< FSRef, CFURLRef >::operator()( CFURLRef url ) const
     {
      return Nitrogen::CFURLGetFSRef( url );
     }
  }
