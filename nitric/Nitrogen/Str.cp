// Nitrogen/Str.cp
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2006 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_STR_H
#include "Nitrogen/Str.h"
#endif
#ifndef __PLSTRINGFUNCS__
#include <PLStringFuncs.h>
#endif
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

#include <algorithm>

namespace Nitrogen
  {
   void CopyToPascalString( const unsigned char *source,
                            unsigned char *destination,
                            unsigned char destinationLength )
     {
      if ( source[0] > destinationLength )
         throw StringTooLong();
      
      PLstrcpy( destination, source ); 
     }

   void CopyToPascalString( const char *source,
                            unsigned char *destination,
                            unsigned char destinationLength )
     {
      if ( std::strlen( source ) > destinationLength )
         throw StringTooLong();
      
      ::CopyCStringToPascal( source, destination );
     }

   void CopyToPascalString( const std::string& source,
                            unsigned char *destination,
                            unsigned char destinationLength )
     {
      if ( source.size() > destinationLength )
         throw StringTooLong();
      
      destination[0] = static_cast< unsigned char >( source.length() );
      std::copy( source.begin(),
                 source.end(),
                 reinterpret_cast<char *>( destination + 1 ) );
     }
  }
