// EnumerationIsPromotable.h

/*
   This traits class answers the age-old question:
   
      If E is an enumerated type all of whose values can
      be represented in the integral type UnderlyingType,
      to which integral types might E be promoted?
   
   The question arises because, in an effort to avoid
   requiring compilers to agree on the sizes of enumerations,
   people write funny non-enumerations like this:
   
      typedef long OSStatus;
      enum { noErr = 0 };
      enum { fnfErr = -43 };
      
   This class tells which integral types the nameless
   OSStatus-enums may promote to. 
*/

#ifndef NITROGEN_ENUMERATIONISPROMOTABLE_H
#define NITROGEN_ENUMERATIONISPROMOTABLE_H

#include <limits>

namespace Nitrogen
  {
   template < class UnderlyingType, class PromotedType >
   struct EnumerationIsPromotable
     {
      static const bool promotable = false;
     };
   
   template < class UnderlyingType >
   struct EnumerationIsPromotable< UnderlyingType, int >
     {
      static const bool promotable = true;
     };

   template < class UnderlyingType >
   struct EnumerationIsPromotable< UnderlyingType, unsigned int >
     {
      static const bool promotable =
            std::numeric_limits< unsigned int   >::digits  > std::numeric_limits< int >::digits
         && std::numeric_limits< UnderlyingType >::digits  > std::numeric_limits< int >::digits;
     };

   template < class UnderlyingType >
   struct EnumerationIsPromotable< UnderlyingType, long >
     {
      static const bool promotable =
         std::numeric_limits<UnderlyingType>::is_signed
          ? (    std::numeric_limits< long           >::digits  > std::numeric_limits<          int >::digits
              && std::numeric_limits< UnderlyingType >::digits  > std::numeric_limits<          int >::digits )
          : (    std::numeric_limits< long           >::digits  > std::numeric_limits< unsigned int >::digits
              && std::numeric_limits< UnderlyingType >::digits  > std::numeric_limits< unsigned int >::digits );
     };

   template < class UnderlyingType >
   struct EnumerationIsPromotable< UnderlyingType, unsigned long >
     {
      static const bool promotable =
            std::numeric_limits< unsigned long  >::digits  > std::numeric_limits< unsigned int >::digits
         && std::numeric_limits< UnderlyingType >::digits  > std::numeric_limits< unsigned int >::digits
         && std::numeric_limits< unsigned long  >::digits  > std::numeric_limits<         long >::digits
         && std::numeric_limits< UnderlyingType >::digits  > std::numeric_limits<         long >::digits;
     };
  }

#endif
