// Files.h

#ifndef NITROGEN_FILES_H
#define NITROGEN_FILES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __FILES__
#include FRAMEWORK_HEADER(CarbonCore,Files.h)
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif
#ifndef NITROGEN_STR_H
#include "Nitrogen/Str.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
// Needed for SMSystemScript()
#ifndef NITROGEN_SCRIPT_H
#include "Nitrogen/Script.h"
#endif
#ifndef NITROGEN_TEXTCOMMON_H
#include "Nitrogen/TextCommon.h"
#endif
#ifndef NITROGEN_INDEXUNTILFAILURECONTAINER_H
#include "Nitrogen/IndexUntilFailureContainer.h"
#endif
#ifndef NITROGEN_INDEXTYPE_H
#include "Nitrogen/IndexType.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_ARRAYCONTAINERFUNCTIONS_H
#include "Nitrogen/ArrayContainerFunctions.h"
#endif

#include <vector>

namespace Nitrogen
  {
   void RegisterFileManagerErrors();

   class FSDirID     // A one-off: like IDType< FSDirIDTag, long, 0 >, but sometimes it's signed, sometimes unsigned
     {
      private:
         unsigned long value;
         
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         
         // Forbidden constructors, unimplemented:
            FSDirID(          bool       );
            FSDirID(          char       );
            FSDirID(   signed char       );
            FSDirID(   signed short      );
            FSDirID(   signed int        );
            FSDirID(   signed long long  );
            FSDirID( unsigned char       );
            FSDirID( unsigned short      );
            FSDirID( unsigned int        );
            FSDirID( unsigned long long  );
            
         // Forbidden conversions, unimplemented:
            // CodeWarrior 7 has a bug which allows conversion operators to be called even
            // when they're private.  So it will give link errors, not compile errors, for these.
            operator          bool     () const;
            operator          char     () const;
            operator   signed char     () const;
            operator   signed short    () const;
            operator   signed int      () const;
            operator   signed long long() const;
            operator unsigned char     () const;
            operator unsigned short    () const;
            operator unsigned int      () const;
            operator unsigned long long() const;

         // Forbidden comparisons, unimplemented:
            friend void operator==( FSDirID,          bool      );
            friend void operator==( FSDirID,          char      );
            friend void operator==( FSDirID,   signed char      );
            friend void operator==( FSDirID,   signed short     );
            friend void operator==( FSDirID,   signed int       );
            friend void operator==( FSDirID,   signed long long );
            friend void operator==( FSDirID, unsigned char      );
            friend void operator==( FSDirID, unsigned short     );
            friend void operator==( FSDirID, unsigned int       );
            friend void operator==( FSDirID, unsigned long long );

            friend void operator==(          bool     , FSDirID );
            friend void operator==(          char     , FSDirID );
            friend void operator==(   signed char     , FSDirID );
            friend void operator==(   signed short    , FSDirID );
            friend void operator==(   signed int      , FSDirID );
            friend void operator==(   signed long long, FSDirID );
            friend void operator==( unsigned char     , FSDirID );
            friend void operator==( unsigned short    , FSDirID );
            friend void operator==( unsigned int      , FSDirID );
            friend void operator==( unsigned long long, FSDirID );

            friend void operator!=( FSDirID,          bool      );
            friend void operator!=( FSDirID,          char      );
            friend void operator!=( FSDirID,   signed char      );
            friend void operator!=( FSDirID,   signed short     );
            friend void operator!=( FSDirID,   signed int       );
            friend void operator!=( FSDirID,   signed long long );
            friend void operator!=( FSDirID, unsigned char      );
            friend void operator!=( FSDirID, unsigned short     );
            friend void operator!=( FSDirID, unsigned int       );
            friend void operator!=( FSDirID, unsigned long long );

            friend void operator!=(          bool     , FSDirID );
            friend void operator!=(          char     , FSDirID );
            friend void operator!=(   signed char     , FSDirID );
            friend void operator!=(   signed short    , FSDirID );
            friend void operator!=(   signed int      , FSDirID );
            friend void operator!=(   signed long long, FSDirID );
            friend void operator!=( unsigned char     , FSDirID );
            friend void operator!=( unsigned short    , FSDirID );
            friend void operator!=( unsigned int      , FSDirID );
            friend void operator!=( unsigned long long, FSDirID );
         #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL

            unsigned long GetUnsigned() const                  { return value; }
            signed long GetSigned() const                      { return static_cast<signed long>( value ); }
            
      public:
         FSDirID()                                             : value( 0 )               {}
         FSDirID( unsigned long theValue )                     : value( theValue )        {}
         FSDirID(   signed long theValue )                     : value( static_cast<unsigned long>( theValue ) )        {}
         
         operator unsigned long() const                        { return value; }
         operator   signed long() const                        { return GetSigned(); }

         static FSDirID Make( unsigned long v )                { return FSDirID( v ); }
         static FSDirID Make(   signed long v )                { return FSDirID( v ); }
         
         template < class T > T Get() const;
         
         friend bool operator==( FSDirID a, FSDirID b )        { return a.GetUnsigned() == b.GetUnsigned(); }
         
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         friend bool operator==( FSDirID a, unsigned long b )  { return a.GetUnsigned() == b; }
         friend bool operator==( unsigned long a, FSDirID b )  { return a == b.GetUnsigned(); }
         friend bool operator==( FSDirID a,   signed long b )  { return a.GetSigned() == b; }
         friend bool operator==(   signed long a, FSDirID b )  { return a == b.GetSigned(); }
      #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL

         friend bool operator!=( FSDirID a, FSDirID b )        { return a.GetUnsigned() != b.GetUnsigned(); }
         
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         friend bool operator!=( FSDirID a, unsigned long b )  { return a.GetUnsigned() != b; }
         friend bool operator!=( unsigned long a, FSDirID b )  { return a != b.GetUnsigned(); }
         friend bool operator!=( FSDirID a,   signed long b )  { return a.GetSigned() != b; }
         friend bool operator!=(   signed long a, FSDirID b )  { return a != b.GetSigned(); }
      #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
     };

   template <> inline unsigned long FSDirID::Get<unsigned long>() const  { return GetUnsigned(); }
   template <> inline   signed long FSDirID::Get<  signed long>() const  { return GetSigned(); }
   
   class FSNodeFlagsTag {};
   typedef FlagType< FSNodeFlagsTag, UInt16, 0 > FSNodeFlags;
   
   class FSNodeIDTag {};
   typedef IDType< FSNodeIDTag, UInt32, 0 > FSNodeID;
   
   class FSVolumeRefNumTag {};
   typedef IDType< FSVolumeRefNumTag, ::FSVolumeRefNum, 0 > FSVolumeRefNum;

   class FSSharingFlags  // Another one-off: like FlagType< FSSharingFlagsTag, UInt8, 0 > but with both signs
     {
      private:
         unsigned char value;
         
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         
         // Forbidden constructors, unimplemented:
            FSSharingFlags(          bool      );
            FSSharingFlags(          char      );
            FSSharingFlags(   signed short     );
            FSSharingFlags(   signed long      );
            FSSharingFlags(   signed long long );
            FSSharingFlags( unsigned short     );
            FSSharingFlags( unsigned int       );
            FSSharingFlags( unsigned long      );
            FSSharingFlags( unsigned long long );
            
         // Forbidden conversions, unimplemented:
            // CodeWarrior 7 has a bug which allows conversion operators to be called even
            // when they're private.  So it will give link errors, not compile errors, for these.
            operator          char     () const;
            operator   signed short    () const;
            operator   signed int      () const;
            operator   signed long     () const;
            operator   signed long long() const;
            operator unsigned short    () const;
            operator unsigned int      () const;
            operator unsigned long     () const;
            operator unsigned long long() const;

         // Forbidden comparisons, unimplemented:
            friend void operator==( FSSharingFlags,          bool      );
            friend void operator==( FSSharingFlags,          char      );
            friend void operator==( FSSharingFlags,   signed short     );
            friend void operator==( FSSharingFlags,   signed long      );
            friend void operator==( FSSharingFlags,   signed long long );
            friend void operator==( FSSharingFlags, unsigned short     );
            friend void operator==( FSSharingFlags, unsigned int       );
            friend void operator==( FSSharingFlags, unsigned long      );
            friend void operator==( FSSharingFlags, unsigned long long );

            friend void operator==(          bool     , FSSharingFlags );
            friend void operator==(          char     , FSSharingFlags );
            friend void operator==(   signed short    , FSSharingFlags );
            friend void operator==(   signed long     , FSSharingFlags );
            friend void operator==(   signed long long, FSSharingFlags );
            friend void operator==( unsigned short    , FSSharingFlags );
            friend void operator==( unsigned int      , FSSharingFlags );
            friend void operator==( unsigned long     , FSSharingFlags );
            friend void operator==( unsigned long long, FSSharingFlags );

            friend void operator!=( FSSharingFlags,          bool      );
            friend void operator!=( FSSharingFlags,          char      );
            friend void operator!=( FSSharingFlags,   signed short     );
            friend void operator!=( FSSharingFlags,   signed long      );
            friend void operator!=( FSSharingFlags,   signed long long );
            friend void operator!=( FSSharingFlags, unsigned short     );
            friend void operator!=( FSSharingFlags, unsigned int       );
            friend void operator!=( FSSharingFlags, unsigned long      );
            friend void operator!=( FSSharingFlags, unsigned long long );

            friend void operator!=(          bool     , FSSharingFlags );
            friend void operator!=(          char     , FSSharingFlags );
            friend void operator!=(   signed short    , FSSharingFlags );
            friend void operator!=(   signed long     , FSSharingFlags );
            friend void operator!=(   signed long long, FSSharingFlags );
            friend void operator!=( unsigned short    , FSSharingFlags );
            friend void operator!=( unsigned int      , FSSharingFlags );
            friend void operator!=( unsigned long     , FSSharingFlags );
            friend void operator!=( unsigned long long, FSSharingFlags );

         // Forbidden bitwise operations, unimplemented:
            friend void operator|( FSSharingFlags,          bool      );
            friend void operator|( FSSharingFlags,          char      );
            friend void operator|( FSSharingFlags,   signed short     );
            friend void operator|( FSSharingFlags,   signed long      );
            friend void operator|( FSSharingFlags,   signed long long );
            friend void operator|( FSSharingFlags, unsigned short     );
            friend void operator|( FSSharingFlags, unsigned int       );
            friend void operator|( FSSharingFlags, unsigned long      );
            friend void operator|( FSSharingFlags, unsigned long long );

            friend void operator|(          bool     , FSSharingFlags );
            friend void operator|(          char     , FSSharingFlags );
            friend void operator|(   signed short    , FSSharingFlags );
            friend void operator|(   signed long     , FSSharingFlags );
            friend void operator|(   signed long long, FSSharingFlags );
            friend void operator|( unsigned short    , FSSharingFlags );
            friend void operator|( unsigned int      , FSSharingFlags );
            friend void operator|( unsigned long     , FSSharingFlags );
            friend void operator|( unsigned long long, FSSharingFlags );

            friend void operator&( FSSharingFlags,          bool      );
            friend void operator&( FSSharingFlags,          char      );
            friend void operator&( FSSharingFlags,   signed short     );
            friend void operator&( FSSharingFlags,   signed long      );
            friend void operator&( FSSharingFlags,   signed long long );
            friend void operator&( FSSharingFlags, unsigned short     );
            friend void operator&( FSSharingFlags, unsigned int       );
            friend void operator&( FSSharingFlags, unsigned long      );
            friend void operator&( FSSharingFlags, unsigned long long );

            friend void operator&(          bool     , FSSharingFlags );
            friend void operator&(          char     , FSSharingFlags );
            friend void operator&(   signed short    , FSSharingFlags );
            friend void operator&(   signed long     , FSSharingFlags );
            friend void operator&(   signed long long, FSSharingFlags );
            friend void operator&( unsigned short    , FSSharingFlags );
            friend void operator&( unsigned int      , FSSharingFlags );
            friend void operator&( unsigned long     , FSSharingFlags );
            friend void operator&( unsigned long long, FSSharingFlags );

            friend void operator^( FSSharingFlags,          bool      );
            friend void operator^( FSSharingFlags,          char      );
            friend void operator^( FSSharingFlags,   signed short     );
            friend void operator^( FSSharingFlags,   signed long      );
            friend void operator^( FSSharingFlags,   signed long long );
            friend void operator^( FSSharingFlags, unsigned short     );
            friend void operator^( FSSharingFlags, unsigned int       );
            friend void operator^( FSSharingFlags, unsigned long      );
            friend void operator^( FSSharingFlags, unsigned long long );

            friend void operator^(          bool     , FSSharingFlags );
            friend void operator^(          char     , FSSharingFlags );
            friend void operator^(   signed short    , FSSharingFlags );
            friend void operator^(   signed long     , FSSharingFlags );
            friend void operator^(   signed long long, FSSharingFlags );
            friend void operator^( unsigned short    , FSSharingFlags );
            friend void operator^( unsigned int      , FSSharingFlags );
            friend void operator^( unsigned long     , FSSharingFlags );
            friend void operator^( unsigned long long, FSSharingFlags );

         // Forbidden bitwise assignments, unimplemented:
            void operator|=(          bool      );
            void operator|=(          char      );
            void operator|=(   signed short     );
            void operator|=(   signed long      );
            void operator|=(   signed long long );
            void operator|=( unsigned short     );
            void operator|=( unsigned int       );
            void operator|=( unsigned long      );
            void operator|=( unsigned long long );

            void operator&=(          bool      );
            void operator&=(          char      );
            void operator&=(   signed short     );
            void operator&=(   signed long      );
            void operator&=(   signed long long );
            void operator&=( unsigned short     );
            void operator&=( unsigned int       );
            void operator&=( unsigned long      );
            void operator&=( unsigned long long );

            void operator^=(          bool      );
            void operator^=(          char      );
            void operator^=(   signed short     );
            void operator^=(   signed long      );
            void operator^=(   signed long long );
            void operator^=( unsigned short     );
            void operator^=( unsigned int       );
            void operator^=( unsigned long      );
            void operator^=( unsigned long long );
            
         #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
            
            UInt8 GetUInt8() const                                               { return UInt8( value ); }
            SInt8 GetSInt8() const                                               { return static_cast<SInt8>( value ); }

      public:
         FSSharingFlags()                                                        : value( 0 )    {}

         static FSSharingFlags Make( UInt8 v )                                   { return FSSharingFlags( v ); }
         static FSSharingFlags Make( SInt8 v )                                   { return FSSharingFlags( static_cast<UInt8>( v ) ); }
         
         template < class T > T Get() const;
         
         bool operator!() const                                                  { return !value; }
         FSSharingFlags operator~() const                                        { return static_cast<UInt8>( ~value ); }
         
         friend bool operator==( FSSharingFlags a, FSSharingFlags b )            { return a.GetUInt8() == b.GetUInt8(); }
         friend bool operator!=( FSSharingFlags a, FSSharingFlags b )            { return a.GetUInt8() != b.GetUInt8(); }

         friend FSSharingFlags operator|( FSSharingFlags a, FSSharingFlags b )   { return static_cast<UInt8>( a.GetUInt8() | b.GetUInt8() ); }
         friend FSSharingFlags operator&( FSSharingFlags a, FSSharingFlags b )   { return static_cast<UInt8>( a.GetUInt8() & b.GetUInt8() ); }
         friend FSSharingFlags operator^( FSSharingFlags a, FSSharingFlags b )   { return static_cast<UInt8>( a.GetUInt8() ^ b.GetUInt8() ); }

         FSSharingFlags& operator|=( FSSharingFlags b )                          { value |= b.GetUInt8(); return *this; }
         FSSharingFlags& operator&=( FSSharingFlags b )                          { value &= b.GetUInt8(); return *this; }
         FSSharingFlags& operator^=( FSSharingFlags b )                          { value ^= b.GetUInt8(); return *this; }
      
         // Allowed converting constructors:
            FSSharingFlags(   signed char v )                               : value( static_cast<UInt8>( v ) )  {}
            FSSharingFlags(   signed int  v )                               : value( static_cast<UInt8>( v ) )  {}
            FSSharingFlags( unsigned char v )                               : value( static_cast<UInt8>( v ) )  {}
         
         // Allowed conversions:
            operator          bool() const                                  { return value; }
            operator   signed char() const                                  { return static_cast<SInt8>( value ); }
            operator unsigned char() const                                  { return value; }

      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         // Allowed comparisons:
            friend bool operator==( FSSharingFlags a,   signed char  b )    { return a.GetSInt8() == b; }
            friend bool operator==( FSSharingFlags a,   signed int   b )    { return a.GetUInt8() == b; }
            friend bool operator==( FSSharingFlags a, unsigned char  b )    { return a.GetUInt8() == b; }

            friend bool operator==(   signed char  a, FSSharingFlags b )    { return a == b.GetSInt8(); }
            friend bool operator==(   signed int   a, FSSharingFlags b )    { return a == b.GetUInt8(); }
            friend bool operator==( unsigned char  a, FSSharingFlags b )    { return a == b.GetUInt8(); }

            friend bool operator!=( FSSharingFlags a,   signed char  b )    { return a.GetSInt8() != b; }
            friend bool operator!=( FSSharingFlags a,   signed int   b )    { return a.GetUInt8() != b; }
            friend bool operator!=( FSSharingFlags a, unsigned char  b )    { return a.GetUInt8() != b; }

            friend bool operator!=(   signed char  a, FSSharingFlags b )    { return a != b.GetSInt8(); }
            friend bool operator!=(   signed int   a, FSSharingFlags b )    { return a != b.GetUInt8(); }
            friend bool operator!=( unsigned char  a, FSSharingFlags b )    { return a != b.GetUInt8(); }

         // Allowed bitwise operators:
            friend FSSharingFlags operator|( FSSharingFlags a,   signed char  b )    { return FSSharingFlags( a.GetSInt8() | b ); }
            friend FSSharingFlags operator|( FSSharingFlags a,   signed int   b )    { return FSSharingFlags( a.GetUInt8() | b ); }
            friend FSSharingFlags operator|( FSSharingFlags a, unsigned char  b )    { return FSSharingFlags( a.GetUInt8() | b ); }

            friend FSSharingFlags operator|(   signed char  a, FSSharingFlags b )    { return FSSharingFlags( a | b.GetSInt8() ); }
            friend FSSharingFlags operator|(   signed int   a, FSSharingFlags b )    { return FSSharingFlags( a | b.GetUInt8() ); }
            friend FSSharingFlags operator|( unsigned char  a, FSSharingFlags b )    { return FSSharingFlags( a | b.GetUInt8() ); }

            friend FSSharingFlags operator&( FSSharingFlags a,   signed char  b )    { return FSSharingFlags( a.GetSInt8() & b ); }
            friend FSSharingFlags operator&( FSSharingFlags a,   signed int   b )    { return FSSharingFlags( a.GetUInt8() & b ); }
            friend FSSharingFlags operator&( FSSharingFlags a, unsigned char  b )    { return FSSharingFlags( a.GetUInt8() & b ); }

            friend FSSharingFlags operator&(   signed char  a, FSSharingFlags b )    { return FSSharingFlags( a & b.GetSInt8() ); }
            friend FSSharingFlags operator&(   signed int   a, FSSharingFlags b )    { return FSSharingFlags( a & b.GetUInt8() ); }
            friend FSSharingFlags operator&( unsigned char  a, FSSharingFlags b )    { return FSSharingFlags( a & b.GetUInt8() ); }

            friend FSSharingFlags operator^( FSSharingFlags a,   signed char  b )    { return FSSharingFlags( a.GetSInt8() ^ b ); }
            friend FSSharingFlags operator^( FSSharingFlags a,   signed int   b )    { return FSSharingFlags( a.GetUInt8() ^ b ); }
            friend FSSharingFlags operator^( FSSharingFlags a, unsigned char  b )    { return FSSharingFlags( a.GetUInt8() ^ b ); }

            friend FSSharingFlags operator^(   signed char  a, FSSharingFlags b )    { return FSSharingFlags( a ^ b.GetSInt8() ); }
            friend FSSharingFlags operator^(   signed int   a, FSSharingFlags b )    { return FSSharingFlags( a ^ b.GetUInt8() ); }
            friend FSSharingFlags operator^( unsigned char  a, FSSharingFlags b )    { return FSSharingFlags( a ^ b.GetUInt8() ); }

         // Allowed bitwise assignments:
            FSSharingFlags& operator|=(   signed char b )     { value |= b; return *this; }
            FSSharingFlags& operator|=(   signed int  b )     { value |= b; return *this; }
            FSSharingFlags& operator|=( unsigned char b )     { value |= b; return *this; }

            FSSharingFlags& operator&=(   signed char b )     { value &= b; return *this; }
            FSSharingFlags& operator&=(   signed int  b )     { value &= b; return *this; }
            FSSharingFlags& operator&=( unsigned char b )     { value &= b; return *this; }

            FSSharingFlags& operator^=(   signed char b )     { value ^= b; return *this; }
            FSSharingFlags& operator^=(   signed int  b )     { value ^= b; return *this; }
            FSSharingFlags& operator^=( unsigned char b )     { value ^= b; return *this; }
      #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
     };

   template <> inline UInt8 FSSharingFlags::Get<UInt8>() const  { return GetUInt8(); }
   template <> inline SInt8 FSSharingFlags::Get<SInt8>() const  { return GetSInt8(); }

   //class FSSharingFlagsTag {};
   //typedef FlagType< FSSharingFlagsTag, UInt8, 0 > FSSharingFlags;
   typedef FSSharingFlags FSIOFileAttributes;
   typedef FSSharingFlags FSIOFlAttrib;
   
   class FSUserPrivileges  // Another one-off: like FlagType< FSUserPrivilegesTag, UInt8, 0 > but with both signs
     {
      private:
         unsigned char value;
         
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         
         // Forbidden constructors, unimplemented:
            FSUserPrivileges(          bool      );
            FSUserPrivileges(          char      );
            FSUserPrivileges(   signed short     );
            FSUserPrivileges(   signed long      );
            FSUserPrivileges(   signed long long );
            FSUserPrivileges( unsigned short     );
            FSUserPrivileges( unsigned int       );
            FSUserPrivileges( unsigned long      );
            FSUserPrivileges( unsigned long long );
            
         // Forbidden conversions, unimplemented:
            // CodeWarrior 7 has a bug which allows conversion operators to be called even
            // when they're private.  So it will give link errors, not compile errors, for these.
            operator          char     () const;
            operator   signed short    () const;
            operator   signed int      () const;
            operator   signed long     () const;
            operator   signed long long() const;
            operator unsigned short    () const;
            operator unsigned int      () const;
            operator unsigned long     () const;
            operator unsigned long long() const;

         // Forbidden comparisons, unimplemented:
            friend void operator==( FSUserPrivileges,          bool      );
            friend void operator==( FSUserPrivileges,          char      );
            friend void operator==( FSUserPrivileges,   signed short     );
            friend void operator==( FSUserPrivileges,   signed long      );
            friend void operator==( FSUserPrivileges,   signed long long );
            friend void operator==( FSUserPrivileges, unsigned short     );
            friend void operator==( FSUserPrivileges, unsigned int       );
            friend void operator==( FSUserPrivileges, unsigned long      );
            friend void operator==( FSUserPrivileges, unsigned long long );

            friend void operator==(          bool     , FSUserPrivileges );
            friend void operator==(          char     , FSUserPrivileges );
            friend void operator==(   signed short    , FSUserPrivileges );
            friend void operator==(   signed long     , FSUserPrivileges );
            friend void operator==(   signed long long, FSUserPrivileges );
            friend void operator==( unsigned short    , FSUserPrivileges );
            friend void operator==( unsigned int      , FSUserPrivileges );
            friend void operator==( unsigned long     , FSUserPrivileges );
            friend void operator==( unsigned long long, FSUserPrivileges );

            friend void operator!=( FSUserPrivileges,          bool      );
            friend void operator!=( FSUserPrivileges,          char      );
            friend void operator!=( FSUserPrivileges,   signed short     );
            friend void operator!=( FSUserPrivileges,   signed long      );
            friend void operator!=( FSUserPrivileges,   signed long long );
            friend void operator!=( FSUserPrivileges, unsigned short     );
            friend void operator!=( FSUserPrivileges, unsigned int       );
            friend void operator!=( FSUserPrivileges, unsigned long      );
            friend void operator!=( FSUserPrivileges, unsigned long long );

            friend void operator!=(          bool     , FSUserPrivileges );
            friend void operator!=(          char     , FSUserPrivileges );
            friend void operator!=(   signed short    , FSUserPrivileges );
            friend void operator!=(   signed long     , FSUserPrivileges );
            friend void operator!=(   signed long long, FSUserPrivileges );
            friend void operator!=( unsigned short    , FSUserPrivileges );
            friend void operator!=( unsigned int      , FSUserPrivileges );
            friend void operator!=( unsigned long     , FSUserPrivileges );
            friend void operator!=( unsigned long long, FSUserPrivileges );

         // Forbidden bitwise operations, unimplemented:
            friend void operator|( FSUserPrivileges,          bool      );
            friend void operator|( FSUserPrivileges,          char      );
            friend void operator|( FSUserPrivileges,   signed short     );
            friend void operator|( FSUserPrivileges,   signed long      );
            friend void operator|( FSUserPrivileges,   signed long long );
            friend void operator|( FSUserPrivileges, unsigned short     );
            friend void operator|( FSUserPrivileges, unsigned int       );
            friend void operator|( FSUserPrivileges, unsigned long      );
            friend void operator|( FSUserPrivileges, unsigned long long );

            friend void operator|(          bool     , FSUserPrivileges );
            friend void operator|(          char     , FSUserPrivileges );
            friend void operator|(   signed short    , FSUserPrivileges );
            friend void operator|(   signed long     , FSUserPrivileges );
            friend void operator|(   signed long long, FSUserPrivileges );
            friend void operator|( unsigned short    , FSUserPrivileges );
            friend void operator|( unsigned int      , FSUserPrivileges );
            friend void operator|( unsigned long     , FSUserPrivileges );
            friend void operator|( unsigned long long, FSUserPrivileges );

            friend void operator&( FSUserPrivileges,          bool      );
            friend void operator&( FSUserPrivileges,          char      );
            friend void operator&( FSUserPrivileges,   signed short     );
            friend void operator&( FSUserPrivileges,   signed long      );
            friend void operator&( FSUserPrivileges,   signed long long );
            friend void operator&( FSUserPrivileges, unsigned short     );
            friend void operator&( FSUserPrivileges, unsigned int       );
            friend void operator&( FSUserPrivileges, unsigned long      );
            friend void operator&( FSUserPrivileges, unsigned long long );

            friend void operator&(          bool     , FSUserPrivileges );
            friend void operator&(          char     , FSUserPrivileges );
            friend void operator&(   signed short    , FSUserPrivileges );
            friend void operator&(   signed long     , FSUserPrivileges );
            friend void operator&(   signed long long, FSUserPrivileges );
            friend void operator&( unsigned short    , FSUserPrivileges );
            friend void operator&( unsigned int      , FSUserPrivileges );
            friend void operator&( unsigned long     , FSUserPrivileges );
            friend void operator&( unsigned long long, FSUserPrivileges );

            friend void operator^( FSUserPrivileges,          bool      );
            friend void operator^( FSUserPrivileges,          char      );
            friend void operator^( FSUserPrivileges,   signed short     );
            friend void operator^( FSUserPrivileges,   signed long      );
            friend void operator^( FSUserPrivileges,   signed long long );
            friend void operator^( FSUserPrivileges, unsigned short     );
            friend void operator^( FSUserPrivileges, unsigned int       );
            friend void operator^( FSUserPrivileges, unsigned long      );
            friend void operator^( FSUserPrivileges, unsigned long long );

            friend void operator^(          bool     , FSUserPrivileges );
            friend void operator^(          char     , FSUserPrivileges );
            friend void operator^(   signed short    , FSUserPrivileges );
            friend void operator^(   signed long     , FSUserPrivileges );
            friend void operator^(   signed long long, FSUserPrivileges );
            friend void operator^( unsigned short    , FSUserPrivileges );
            friend void operator^( unsigned int      , FSUserPrivileges );
            friend void operator^( unsigned long     , FSUserPrivileges );
            friend void operator^( unsigned long long, FSUserPrivileges );

         // Forbidden bitwise assignments, unimplemented:
            void operator|=(          bool      );
            void operator|=(          char      );
            void operator|=(   signed short     );
            void operator|=(   signed long      );
            void operator|=(   signed long long );
            void operator|=( unsigned short     );
            void operator|=( unsigned int       );
            void operator|=( unsigned long      );
            void operator|=( unsigned long long );

            void operator&=(          bool      );
            void operator&=(          char      );
            void operator&=(   signed short     );
            void operator&=(   signed long      );
            void operator&=(   signed long long );
            void operator&=( unsigned short     );
            void operator&=( unsigned int       );
            void operator&=( unsigned long      );
            void operator&=( unsigned long long );

            void operator^=(          bool      );
            void operator^=(          char      );
            void operator^=(   signed short     );
            void operator^=(   signed long      );
            void operator^=(   signed long long );
            void operator^=( unsigned short     );
            void operator^=( unsigned int       );
            void operator^=( unsigned long      );
            void operator^=( unsigned long long );
         #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL

            UInt8 GetUInt8() const                                               { return value; }
            SInt8 GetSInt8() const                                               { return static_cast<SInt8>( value ); }

      public:
         FSUserPrivileges()                                                            : value( 0 )    {}

         static FSUserPrivileges Make( UInt8 v )                                       { return FSUserPrivileges( v ); }
         static FSUserPrivileges Make( SInt8 v )                                       { return FSUserPrivileges( static_cast<UInt8>( v ) ); }
         
         template < class T > T Get() const;
         
         bool operator!() const                                                        { return !value; }
         FSUserPrivileges operator~() const                                            { return static_cast<UInt8>( ~value ); }
         
         friend bool operator==( FSUserPrivileges a, FSUserPrivileges b )              { return a.GetUInt8() == b.GetUInt8(); }
         friend bool operator!=( FSUserPrivileges a, FSUserPrivileges b )              { return a.GetUInt8() != b.GetUInt8(); }

         friend FSUserPrivileges operator|( FSUserPrivileges a, FSUserPrivileges b )   { return static_cast<UInt8>( a.GetUInt8() | b.GetUInt8() ); }
         friend FSUserPrivileges operator&( FSUserPrivileges a, FSUserPrivileges b )   { return static_cast<UInt8>( a.GetUInt8() & b.GetUInt8() ); }
         friend FSUserPrivileges operator^( FSUserPrivileges a, FSUserPrivileges b )   { return static_cast<UInt8>( a.GetUInt8() ^ b.GetUInt8() ); }

         FSUserPrivileges& operator|=( FSUserPrivileges b )                            { value |= b.GetUInt8(); return *this; }
         FSUserPrivileges& operator&=( FSUserPrivileges b )                            { value &= b.GetUInt8(); return *this; }
         FSUserPrivileges& operator^=( FSUserPrivileges b )                            { value ^= b.GetUInt8(); return *this; }
      
         // Allowed converting constructors:
            FSUserPrivileges(   signed char v )                               : value( static_cast<UInt8>( v ) )  {}
            FSUserPrivileges(   signed int  v )                               : value( static_cast<UInt8>( v ) )  {}
            FSUserPrivileges( unsigned char v )                               : value( static_cast<UInt8>( v ) )  {}
         
         // Allowed conversions:
            operator          bool() const                                    { return value; }
            operator   signed char() const                                    { return GetSInt8(); }
            operator unsigned char() const                                    { return GetUInt8(); }

      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         // Allowed comparisons:
            friend bool operator==( FSUserPrivileges a,   signed char  b )    { return a.GetSInt8() == b; }
            friend bool operator==( FSUserPrivileges a,   signed int   b )    { return a.GetUInt8() == b; }
            friend bool operator==( FSUserPrivileges a, unsigned char  b )    { return a.GetUInt8() == b; }

            friend bool operator==(   signed char  a, FSUserPrivileges b )    { return a == b.GetSInt8(); }
            friend bool operator==(   signed int   a, FSUserPrivileges b )    { return a == b.GetUInt8(); }
            friend bool operator==( unsigned char  a, FSUserPrivileges b )    { return a == b.GetUInt8(); }

            friend bool operator!=( FSUserPrivileges a,   signed char  b )    { return a.GetSInt8() != b; }
            friend bool operator!=( FSUserPrivileges a,   signed int   b )    { return a.GetUInt8() != b; }
            friend bool operator!=( FSUserPrivileges a, unsigned char  b )    { return a.GetUInt8() != b; }

            friend bool operator!=(   signed char  a, FSUserPrivileges b )    { return a != b.GetSInt8(); }
            friend bool operator!=(   signed int   a, FSUserPrivileges b )    { return a != b.GetUInt8(); }
            friend bool operator!=( unsigned char  a, FSUserPrivileges b )    { return a != b.GetUInt8(); }

         // Allowed bitwise operators:
            friend FSUserPrivileges operator|( FSUserPrivileges a,   signed char  b )    { return FSUserPrivileges( a.GetSInt8() | b ); }
            friend FSUserPrivileges operator|( FSUserPrivileges a,   signed int   b )    { return FSUserPrivileges( a.GetUInt8() | b ); }
            friend FSUserPrivileges operator|( FSUserPrivileges a, unsigned char  b )    { return FSUserPrivileges( a.GetUInt8() | b ); }

            friend FSUserPrivileges operator|(   signed char  a, FSUserPrivileges b )    { return FSUserPrivileges( a | b.GetSInt8() ); }
            friend FSUserPrivileges operator|(   signed int   a, FSUserPrivileges b )    { return FSUserPrivileges( a | b.GetUInt8() ); }
            friend FSUserPrivileges operator|( unsigned char  a, FSUserPrivileges b )    { return FSUserPrivileges( a | b.GetUInt8() ); }

            friend FSUserPrivileges operator&( FSUserPrivileges a,   signed char  b )    { return FSUserPrivileges( a.GetSInt8() & b ); }
            friend FSUserPrivileges operator&( FSUserPrivileges a,   signed int   b )    { return FSUserPrivileges( a.GetUInt8() & b ); }
            friend FSUserPrivileges operator&( FSUserPrivileges a, unsigned char  b )    { return FSUserPrivileges( a.GetUInt8() & b ); }

            friend FSUserPrivileges operator&(   signed char  a, FSUserPrivileges b )    { return FSUserPrivileges( a & b.GetSInt8() ); }
            friend FSUserPrivileges operator&(   signed int   a, FSUserPrivileges b )    { return FSUserPrivileges( a & b.GetUInt8() ); }
            friend FSUserPrivileges operator&( unsigned char  a, FSUserPrivileges b )    { return FSUserPrivileges( a & b.GetUInt8() ); }

            friend FSUserPrivileges operator^( FSUserPrivileges a,   signed char  b )    { return FSUserPrivileges( a.GetSInt8() ^ b ); }
            friend FSUserPrivileges operator^( FSUserPrivileges a,   signed int   b )    { return FSUserPrivileges( a.GetUInt8() ^ b ); }
            friend FSUserPrivileges operator^( FSUserPrivileges a, unsigned char  b )    { return FSUserPrivileges( a.GetUInt8() ^ b ); }

            friend FSUserPrivileges operator^(   signed char  a, FSUserPrivileges b )    { return FSUserPrivileges( a ^ b.GetSInt8() ); }
            friend FSUserPrivileges operator^(   signed int   a, FSUserPrivileges b )    { return FSUserPrivileges( a ^ b.GetUInt8() ); }
            friend FSUserPrivileges operator^( unsigned char  a, FSUserPrivileges b )    { return FSUserPrivileges( a ^ b.GetUInt8() ); }

         // Allowed bitwise assignments:
            FSUserPrivileges& operator|=(   signed char b )     { value |= b; return *this; }
            FSUserPrivileges& operator|=(   signed int  b )     { value |= b; return *this; }
            FSUserPrivileges& operator|=( unsigned char b )     { value |= b; return *this; }

            FSUserPrivileges& operator&=(   signed char b )     { value &= b; return *this; }
            FSUserPrivileges& operator&=(   signed int  b )     { value &= b; return *this; }
            FSUserPrivileges& operator&=( unsigned char b )     { value &= b; return *this; }

            FSUserPrivileges& operator^=(   signed char b )     { value ^= b; return *this; }
            FSUserPrivileges& operator^=(   signed int  b )     { value ^= b; return *this; }
            FSUserPrivileges& operator^=( unsigned char b )     { value ^= b; return *this; }
      #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
     };

   template <> inline UInt8 FSUserPrivileges::Get<UInt8>() const  { return GetUInt8(); }
   template <> inline SInt8 FSUserPrivileges::Get<SInt8>() const  { return GetSInt8(); }
   
   //class FSUserPrivilegesTag {};
   //typedef FlagType< FSUserPrivilegesTag, UInt8, 0 > FSUserPrivileges;
   typedef FSUserPrivileges FSIOACUser;

   class FSIteratorFlagsTag {};
   typedef FlagType< FSIteratorFlagsTag, ::FSIteratorFlags, 0 > FSIteratorFlags;
   
	class FSFileRefNum_Tag {};
	typedef IDType< FSFileRefNum_Tag, SInt16, 0 > FSFileRefNum;
	
   class FSForkRefNumTag {};
   typedef IDType< FSForkRefNumTag, SInt16, 0 > FSForkRefNum;
    
   class FSIOPermssnTag {};
   typedef FlagType< FSIOPermssnTag, SInt8, ::fsCurPerm > FSIOPermssn;
   typedef FSIOPermssn FSIOPermissions;
	
   class FSIOPosModeTag {};
   typedef FlagType< FSIOPosModeTag, UInt16, fsAtMark > FSIOPosMode;
   typedef FSIOPosMode FSIOPositioningMode;
	
   class FSAllocationFlagsTag {};
   typedef FlagType< FSAllocationFlagsTag, ::FSAllocationFlags, 0 > FSAllocationFlags;
  
   class FSForkIteratorTag {};
   typedef IDType< FSForkIteratorTag, SInt16, 0 > FSForkIterator;

   class FSVolumeInfoFlagsTag {};
   typedef FlagType< FSVolumeInfoFlagsTag, UInt16, 0 > FSVolumeInfoFlags;
   typedef FSVolumeInfoFlags FSIOVAtrb;
   typedef FSVolumeInfoFlags FSIOVolumeAttributes;
   
   struct FSVolumeIndex_Specifics
     {
      typedef ::ItemCount UnderlyingType;
      static const UnderlyingType defaultValue = 0;
     };

   typedef IndexType< FSVolumeIndex_Specifics > FSVolumeIndex;
   
   class FSFileSystemIDTag {};
   typedef IDType< FSFileSystemIDTag, UInt16, 0 > FSFileSystemID;
   typedef FSFileSystemID IOFSID;
   
   class DriverReferenceNumberTag {};
   typedef IDType< DriverReferenceNumberTag, SInt16, 0 > DriverReferenceNumber;
   typedef DriverReferenceNumber DRefNum;
   typedef DriverReferenceNumber DriverRefNum;

   class HFSCatalogNodeIDTag {};
   typedef IDType< HFSCatalogNodeIDTag, UInt32, 0 > HFSCatalogNodeID;
	
	inline FSIOPermssn FSCurPerm   ()  { return FSIOPermssn::Make( fsCurPerm    ); }
	inline FSIOPermssn FSRdPerm    ()  { return FSIOPermssn::Make( fsRdPerm     ); }
	inline FSIOPermssn FSWrPerm    ()  { return FSIOPermssn::Make( fsWrPerm     ); }
	inline FSIOPermssn FSRdWrPerm  ()  { return FSIOPermssn::Make( fsRdWrPerm   ); }
	inline FSIOPermssn FSRdWrShPerm()  { return FSIOPermssn::Make( fsRdWrShPerm ); }
	inline FSIOPermssn FSRdDenyPerm()  { return FSIOPermssn::Make( fsRdDenyPerm ); }
	inline FSIOPermssn FSWrDenyPerm()  { return FSIOPermssn::Make( fsWrDenyPerm ); }
	
	inline FSDirID FSRtParID()  { return FSDirID( long( fsRtParID ) ); }
	inline FSDirID FSRtDirID()  { return FSDirID( long( fsRtDirID ) ); }
	
	inline FSIOPosMode FSAtMark   ()  { return FSIOPosMode::Make( fsAtMark    ); }
	inline FSIOPosMode FSFromStart()  { return FSIOPosMode::Make( fsFromStart ); }
	inline FSIOPosMode FSFromLEOF ()  { return FSIOPosMode::Make( fsFromLEOF  ); }
	inline FSIOPosMode FSFromMark ()  { return FSIOPosMode::Make( fsFromMark  ); }
	
	inline FSIOPosMode PleaseCacheMask()  { return FSIOPosMode::Make( pleaseCacheMask ); }
	inline FSIOPosMode NoCacheMask    ()  { return FSIOPosMode::Make( noCacheMask     ); }
	inline FSIOPosMode RdVerifyMask   ()  { return FSIOPosMode::Make( rdVerifyMask    ); }
	inline FSIOPosMode RdVerify       ()  { return FSIOPosMode::Make( rdVerify        ); }
	inline FSIOPosMode ForceReadMask  ()  { return FSIOPosMode::Make( forceReadMask   ); }
	inline FSIOPosMode NewLineMask    ()  { return FSIOPosMode::Make( newLineMask     ); }
	inline FSIOPosMode NewLineCharMask()  { return FSIOPosMode::Make( newLineCharMask ); }

   template <> struct Converter< Owned<CFStringRef>, HFSUniStr255 >: public std::unary_function< HFSUniStr255, Owned<CFStringRef> >
     {
      Owned<CFStringRef> operator()( const HFSUniStr255& in ) const
        {
         return CFStringCreateWithCharacters( in.unicode, in.length );
        }
     };
   
   template <> struct Converter< HFSUniStr255, CFStringRef >: public std::unary_function< CFStringRef, HFSUniStr255 >
     {
      HFSUniStr255 operator()( CFStringRef in ) const
        {
         CFIndex length = Nitrogen::CFStringGetLength( in );
         if ( length > 255 )
            throw StringTooLong();
         HFSUniStr255 result;
         result.length = Convert<UInt16>( length );
         Nitrogen::CFStringGetCharacters( in, CFRangeMake( 0, length ), result.unicode );
         return result;
        }
     };

   template <> struct Converter< UniString, HFSUniStr255 >: public std::unary_function< HFSUniStr255, UniString >
     {
      UniString operator()( const HFSUniStr255& in ) const
        {
         return UniString( in.unicode, in.unicode+in.length );
        }
     };
  
   template <> struct Converter< HFSUniStr255, UniString >: public std::unary_function< UniString, HFSUniStr255 >
     {
      HFSUniStr255 operator()( const UniString& in ) const
        {
         if ( in.size() > 255 )
            throw StringTooLong();
         HFSUniStr255 result;
         result.length = Convert<UInt16>( in.size() );
         std::copy( in.begin(), in.end(), result.unicode );
         return result;
        }
     };
	
	template <> struct Disposer< FSFileRefNum > : public std::unary_function< FSFileRefNum, void >,
	                                              private DefaultDestructionOSStatusPolicy
	{
		void operator()( FSFileRefNum file ) const
		{
			OnlyOnce< RegisterFileManagerErrors >();
			HandleDestructionOSStatus( ::FSClose( file ) );
		}
	};
	
	// 2142
	void FSClose( Owned< FSFileRefNum > fileRefNum );
	
	// 2154
	SInt32 FSRead( FSFileRefNum file,
	               SInt32       requestCount,
	               void *       buffer );
	
	template < class Element, std::size_t count >
	SInt32 FSRead( FSFileRefNum file,
	               Element      (&buffer)[count] )
	{
		return FSRead( file, count * sizeof (Element), buffer );
	}
	
	// 2169
	SInt32 FSWrite( FSFileRefNum file,
	                SInt32       requestCount,
	                const void * buffer );
	
	template < class Element, std::size_t count >
	SInt32 FSWrite( FSFileRefNum  file,
	                const Element (&buffer)[count] )
	{
		return FSWrite( file, count * sizeof (Element), buffer );
	}
	
	// 2335
	SInt32 Allocate( FSFileRefNum      fileRefNum,
	                 SInt32            requestCount );
	
	// 2349
	SInt32 GetEOF( FSFileRefNum fileRefNum );
	
	// 2363
	void SetEOF( FSFileRefNum fileRefNum,
	             SInt32       positionOffset );
	
	// 2377
	SInt32 GetFPos( FSFileRefNum fileRefNum );
	
	// 2391
	void SetFPos( FSFileRefNum fileRefNum,
	              FSIOPosMode  positionMode,
	              SInt32       positionOffset );
	
	struct FSDirSpec
	{
		FSVolumeRefNum vRefNum;
		FSDirID dirID;
	};
	
	template <>
	struct Maker< FSDirSpec >
	{
		FSDirSpec operator()( FSVolumeRefNum vRefNum, FSDirID dirID ) const
		{
			FSDirSpec result;
			result.vRefNum = vRefNum;
			result.dirID = dirID;
			return result;
		}
	};
	
	inline FSDirSpec GetFileParent( const FSSpec& file )
	{
		return Make< FSDirSpec >( file.vRefNum, file.parID );
	}
	
	// 2872
	CInfoPBRec& PBGetCatInfoSync( CInfoPBRec& paramBlock );
	
	CInfoPBRec& FSpGetCatInfo( const FSSpec&   item, CInfoPBRec& paramBlock );
	
	CInfoPBRec& FSpGetCatInfo( const FSDirSpec&  dir,
	                           CInfoPBRec&       paramBlock,
	                           StringPtr         name        = NULL );
	
	CInfoPBRec& FSpGetCatInfo( const FSDirSpec&  dir,
	                           UInt16            index,
	                           CInfoPBRec&       paramBlock,
	                           StringPtr         name        = NULL );
	
	inline bool TestIsDirectory( const CInfoPBRec& paramBlock )
	{
		return paramBlock.hFileInfo.ioFlAttrib & char( kioFlAttribDirMask );
	}
	
	inline bool TestIsFile( const CInfoPBRec& paramBlock )
	{
		return !TestIsDirectory( paramBlock );
	}
	
	bool TestFSItemExists( const FSSpec& item, CInfoPBRec& paramBlock );
	
	inline bool TestDirectoryExists( const FSSpec& file, CInfoPBRec& paramBlock )
	{
		return TestFSItemExists( file, paramBlock ) && TestIsDirectory( paramBlock );
	}
	
	inline bool TestFileExists( const FSSpec& file, CInfoPBRec& paramBlock )
	{
		return TestFSItemExists( file, paramBlock ) && TestIsFile( paramBlock );
	}
	
	inline bool TestFSItemExists   ( const FSSpec& item )  { CInfoPBRec pb;  return TestFSItemExists   ( item, pb ); }
	inline bool TestDirectoryExists( const FSSpec& dir  )  { CInfoPBRec pb;  return TestDirectoryExists( dir,  pb ); }
	inline bool TestFileExists     ( const FSSpec& file )  { CInfoPBRec pb;  return TestFileExists     ( file, pb ); }
	
	// 3690
	void PBHGetVolParmsSync( HParamBlockRec& paramBlock );
	GetVolParmsInfoBuffer PBHGetVolParmsSync( FSVolumeRefNum vRefNum );
	
	// 4167
	void PBDTGetPath( DTPBRec& pb );
	
	// 4279
	void PBDTGetAPPLSync( DTPBRec& pb );
	
	FSSpec DTGetAPPL( OSType signature, FSVolumeRefNum vRefNum );
	FSSpec DTGetAPPL( OSType signature );
	
	// 4617
	FSSpec FSMakeFSSpec( FSVolumeRefNum vRefNum, FSDirID dirID, ConstStr255Param name );
	
	inline FSSpec FSMakeFSSpec( const FSDirSpec& dir, ConstStr255Param name )
	{
		return FSMakeFSSpec( dir.vRefNum, dir.dirID, name );
	}
	
	template <> struct Converter< FSSpec, FSDirSpec >: public std::unary_function< FSDirSpec, FSSpec >
	{
		FSSpec operator()( const FSDirSpec& dir ) const
		{
			return FSMakeFSSpec( dir, NULL );
		}
	};
	
	template <> struct Converter< FSDirSpec, FSSpec >: public std::unary_function< FSSpec, FSDirSpec >
	{
		FSDirSpec operator()( const FSSpec& dir ) const;
	};
	
	inline FSDirSpec operator<<( const FSDirSpec& dir, ConstStr255Param name )
	{
		return Convert< FSDirSpec >( FSMakeFSSpec( dir, name ) );
	}
	
	inline FSDirSpec operator<<( const FSDirSpec& dir, std::string name )
	{
		return dir << Str255( name );
	}
	
	inline FSSpec operator&( const FSDirSpec& dir, const unsigned char* name )
	{
		FSSpec result;
		OSErr err = ::FSMakeFSSpec( dir.vRefNum, dir.dirID, name, &result );
		if ( err != OSErr( fnfErr ) )
		{
			ThrowOSStatus( err );
		}
		return result;
	}
	
	inline FSSpec operator&( const FSDirSpec& dir, const std::string& name )
	{
		return dir & Str255( name );
	}
	
	FSSpec FSMakeFSSpec( ConstStr255Param pathname );
	
	// 4633
	Owned< FSFileRefNum > FSpOpenDF( const FSSpec&   spec,
	                                 FSIOPermssn     permissions );
	
	// 4648
	Owned< FSFileRefNum > FSpOpenRF( const FSSpec&   spec,
	                                 FSIOPermssn     permissions );
	
	// 4663
	FSSpec FSpCreate( const FSSpec&  file, 
	                  OSType         creator, 
	                  OSType         type, 
	                  ScriptCode     scriptTag = SMSystemScript() );
	
	// 4679
	FSDirSpec FSpDirCreate( const FSSpec&  dir, 
	                        ScriptCode     scriptTag = SMSystemScript() );
	
	// 4694
	void FSpDelete( const FSSpec& item );
	
	// 4706
	FInfo FSpGetFInfo( const FSSpec& file );
	
	// 4772
	// dest is the directory to move source *into*, not the actual new location of source.
	void FSpCatMove( const FSSpec& source, const FSSpec& dest );
	
	inline void FSpCatMove( const FSSpec& source, FSDirSpec dest )
	{
		FSpCatMove( source, Convert< FSSpec >( dest ) );
	}
	
	inline void FSpCatMove( const FSSpec& source, FSDirID dest )
	{
		FSpCatMove( source, Make< FSDirSpec >( FSVolumeRefNum( source.vRefNum ), dest ) );
	}
	
   // 5414
   FSRef FSpMakeFSRef( const FSSpec& );

   template <> struct Converter< FSRef, FSSpec >: public std::unary_function< FSSpec, FSRef >
     {
      FSRef operator()( const FSSpec& spec ) const
        {
         return FSpMakeFSRef( spec );
        }
     };

   FSRef FSMakeFSRefUnicode( const FSRef&   parentRef,
                             UniCharCount   nameLength,
                             const UniChar *name,
                             TextEncoding   textEncodingHint );

   inline FSRef FSMakeFSRefUnicode( const FSRef&        parentRef,
                                    const HFSUniStr255& name,
                                    TextEncoding        textEncodingHint )
     {
      return FSMakeFSRefUnicode( parentRef, name.length, name.unicode, textEncodingHint );
     }

   FSRef FSMakeFSRefUnicode( const FSRef&     parentRef,
                             const UniString& name,
                             TextEncoding     textEncodingHint );
   
   bool FSCompareFSRefs( const FSRef& ref1, const FSRef& ref2 );

   inline bool operator==( const FSRef& ref1, const FSRef& ref2 )   { return FSCompareFSRefs( ref1, ref2 ); }
   inline bool operator!=( const FSRef& ref1, const FSRef& ref2 )   { return !( ref1 == ref2 ); }


   struct FSRefSpec
     {
      FSRef fsRef;
      FSSpec fsSpec;
      
      operator const FSRef&() const   { return fsRef; }
      operator const FSSpec&() const  { return fsSpec; }
     };

   struct FSRefSpecDirID: public FSRefSpec
     {
      FSDirID dirID;

      operator FSDirID() const          { return dirID; }
     };

   class FileSystemDisposer: private DefaultDestructionOSStatusPolicy
     {
      public:
         void operator()( const FSRef& ) const;
         void operator()( const FSSpec& ) const;
         void operator()( const FSRefSpec& r ) const          { operator()( r.fsRef ); }
     };
   
   template <> struct OwnedDefaults< FSRef >           { typedef FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< FSSpec >          { typedef FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< FSDirID >         { typedef FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< FSRefSpec >       { typedef FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< FSRefSpecDirID >  { typedef FileSystemDisposer Disposer; };
   
   template <> struct LivelinessTraits< FSRef,          FileSystemDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
   template <> struct LivelinessTraits< FSSpec,         FileSystemDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
   template <> struct LivelinessTraits< FSRefSpec,      FileSystemDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
   template <> struct LivelinessTraits< FSRefSpecDirID, FileSystemDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };


   typedef Owned<FSRefSpec> FSCreateFileUnicode_Result;
   
   // 5588
   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&         parentRef,
                                                   UniCharCount         nameLength,
                                                   const UniChar *      name,
                                                   FSCatalogInfoBitmap  whichInfo,
                                                   const FSCatalogInfo& catalogInfo );

   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&   parentRef,
                                                   UniCharCount   nameLength,
                                                   const UniChar *name );

   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&         parentRef,
                                                   const UniString&     name,
                                                   FSCatalogInfoBitmap  whichInfo,
                                                   const FSCatalogInfo& catalogInfo );

   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&     parentRef,
                                                   const UniString& name );
   
   typedef Owned<FSRefSpecDirID> FSCreateDirectoryUnicode_Result;
   
   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&         parentRef,
                                                             UniCharCount         nameLength,
                                                             const UniChar *      name,
                                                             FSCatalogInfoBitmap  whichInfo,
                                                             const FSCatalogInfo& catalogInfo );

   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&   parentRef,
                                                             UniCharCount   nameLength,
                                                             const UniChar *name );

   inline FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&        parentRef,
                                                                    const HFSUniStr255& name )
     {
      return FSCreateDirectoryUnicode( parentRef, name.length, name.unicode );
     }
   
   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&         parentRef,
                                                             const UniString&     name,
                                                             FSCatalogInfoBitmap  whichInfo,
                                                             const FSCatalogInfo& catalogInfo );

   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&     parentRef,
                                                             const UniString& name );

   void FSDeleteObject( Owned<FSRef> );

   void FSMoveObject( Owned<FSRef>&, const FSRef& destDirectory );

   void FSExchangeObjects( const FSRef& ref, const FSRef& destRef );
   
   void FSRenameUnicode( Owned<FSRef>&  ref,
                         UniCharCount   nameLength,
                         const UniChar *name,
                         TextEncoding   textEncodingHint );

   void FSRenameUnicode( Owned<FSRef>&    ref,
                         const UniString& name,
                         TextEncoding     textEncodingHint );

   struct FSGetCatalogInfo_Result
     {
      FSCatalogInfo catalogInfo;
      HFSUniStr255  outName;
      FSSpec        fsSpec;
      FSRef         parentRef;
     };
   
   struct ForkSizes
     {
      UInt64 logicalSize;
      UInt64 physicalSize;
     };
   
   union FinderInfo
     {
      FileInfo   fileInfo;
      FolderInfo folderInfo;
      UInt8      finderInfo[16];
      
      FinderInfo()                                     { fileInfo = FileInfo(); }
      FinderInfo( const FileInfo& info )               { fileInfo = info; }
      FinderInfo( const FolderInfo& info )             { folderInfo = info; }
         
      FinderInfo& operator=( const FileInfo& info )    { fileInfo = info;   return *this; } 
      FinderInfo& operator=( const FolderInfo& info )  { folderInfo = info; return *this; }
      
      operator const FileInfo&() const                 { return fileInfo; }
      operator const FolderInfo&() const               { return folderInfo; }
      
      operator FileInfo&()                             { return fileInfo; }
      operator FolderInfo&()                           { return folderInfo; }
     };

   union ExtFinderInfo
     {
      ExtendedFileInfo   extFileInfo;
      ExtendedFolderInfo extFolderInfo;
      UInt8              extFinderInfo[16];

      ExtFinderInfo()                                             { extFileInfo = ExtendedFileInfo(); }
      ExtFinderInfo( const ExtendedFileInfo& info )               { extFileInfo = info; }
      ExtFinderInfo( const ExtendedFolderInfo& info )             { extFolderInfo = info; }
         
      ExtFinderInfo& operator=( const ExtendedFileInfo& info )    { extFileInfo = info;   return *this; } 
      ExtFinderInfo& operator=( const ExtendedFolderInfo& info )  { extFolderInfo = info; return *this; }
      
      operator const ExtendedFileInfo&() const                    { return extFileInfo; }
      operator const ExtendedFolderInfo&() const                  { return extFolderInfo; }
      
      operator ExtendedFileInfo&()                                { return extFileInfo; }
      operator ExtendedFolderInfo&()                              { return extFolderInfo; }
     };
   
   struct Permissions
     {
      UInt32 permissions[4];
     };
   
#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template < class DesiredType, class MemberType, MemberType FSCatalogInfo::*member >
   struct Basic_FSCatalogInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSCatalogInfo& info )                { return info.*member; }
      static void Set( FSCatalogInfo& info, const Type& value )   { info.*member = value; }
     };

   template < class DesiredType, class MemberType, MemberType FSCatalogInfo::*member >
   struct Const_FSCatalogInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSCatalogInfo& info )                { return info.*member; }
     };

   
   template < UInt64 FSCatalogInfo::*logical, UInt64 FSCatalogInfo::*physical >
   struct ForkSizes_FSCatalogInfoBit_Traits
     {
      typedef ForkSizes Type;
      
      static Type Get( const FSCatalogInfo& info )
        {
         ForkSizes result;
         result.logicalSize  = info.*logical;
         result.physicalSize = info.*physical;
         return result;
        }
     };

   template < class DesiredType,
              class MemberType,
              std::size_t size,
              MemberType (FSCatalogInfo::*infoMember)[size],
              MemberType (DesiredType::*desiredMember)[size] >
   struct Array_FSCatalogInfoBit_Traits
     {
      typedef DesiredType Type;
      
      static Type Get( const FSCatalogInfo& info )
        {
         DesiredType result;
         for ( std::size_t i = 0; i < size; ++i )
            (result.*desiredMember)[i] = (info.*infoMember)[i];
         return result;
        }
      
      static void Set( FSCatalogInfo& info, const Type& value )
        {
         for ( std::size_t i = 0; i < size; ++i )
            (info.*infoMember)[i] = (value.*desiredMember)[i];
        }
     };
   
   
   template < ::FSCatalogInfoBitmap bit > struct FSCatalogInfoBit_Traits;
   
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoTextEncoding > : Basic_FSCatalogInfoBit_Traits< TextEncoding,     ::TextEncoding,   &FSCatalogInfo::textEncodingHint > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoNodeFlags    > : Basic_FSCatalogInfoBit_Traits< FSNodeFlags,      ::UInt16,         &FSCatalogInfo::nodeFlags        > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoVolume       > : Const_FSCatalogInfoBit_Traits< FSVolumeRefNum,   ::FSVolumeRefNum, &FSCatalogInfo::volume           > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoParentDirID  > : Const_FSCatalogInfoBit_Traits< FSDirID,          ::UInt32,         &FSCatalogInfo::parentDirID      > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoNodeID       > : Const_FSCatalogInfoBit_Traits< FSNodeID,         ::UInt32,         &FSCatalogInfo::nodeID           > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoCreateDate   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::createDate       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoContentMod   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::contentModDate   > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoAttrMod      > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::attributeModDate > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoAccessDate   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::accessDate       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoBackupDate   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::backupDate       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoValence      > : Const_FSCatalogInfoBit_Traits< UInt32,           ::UInt32,         &FSCatalogInfo::valence          > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoSharingFlags > : Const_FSCatalogInfoBit_Traits< FSSharingFlags,   ::UInt8,          &FSCatalogInfo::sharingFlags     > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoUserPrivs    > : Const_FSCatalogInfoBit_Traits< FSUserPrivileges, ::UInt8,          &FSCatalogInfo::userPrivileges   > {};

   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoDataSizes > : ForkSizes_FSCatalogInfoBit_Traits< &FSCatalogInfo::dataLogicalSize, &FSCatalogInfo::dataPhysicalSize > {}; 
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoRsrcSizes > : ForkSizes_FSCatalogInfoBit_Traits< &FSCatalogInfo::rsrcLogicalSize, &FSCatalogInfo::rsrcPhysicalSize > {};

   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoPermissions  > : Array_FSCatalogInfoBit_Traits< Permissions,   ::UInt32, 4, &FSCatalogInfo::permissions,   &Permissions::permissions     > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoFinderInfo   > : Array_FSCatalogInfoBit_Traits< FinderInfo,    ::UInt8, 16, &FSCatalogInfo::finderInfo,    &FinderInfo::finderInfo       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoFinderXInfo  > : Array_FSCatalogInfoBit_Traits< ExtFinderInfo, ::UInt8, 16, &FSCatalogInfo::extFinderInfo, &ExtFinderInfo::extFinderInfo > {};
#endif

   void FSGetCatalogInfo( const FSRef&        ref,
                          FSCatalogInfoBitmap whichInfo,
                          FSCatalogInfo *     catalogInfo,
                          HFSUniStr255 *      outName,
                          FSSpec *            fsSpec,
                          FSRef *             parentRef );
   
   FSGetCatalogInfo_Result FSGetCatalogInfo( const FSRef& ref, FSCatalogInfoBitmap whichInfo );
   
#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template< ::FSCatalogInfoBitmap bit >
   struct FSGetCatalogInfo_Traits
     {
      typedef typename FSCatalogInfoBit_Traits<bit>::Type Result;
     };
   
   template< ::FSCatalogInfoBitmap bit >
   typename FSGetCatalogInfo_Traits<bit>::Result FSGetCatalogInfo( const FSRef& ref )
     {
      FSCatalogInfo info;
      FSGetCatalogInfo( ref, bit, &info, 0, 0, 0 );
      return FSCatalogInfoBit_Traits<bit>::Get( info );
     }
#endif

   void FSSetCatalogInfo( const FSRef&         ref,
                          FSCatalogInfoBitmap  whichInfo,
                          const FSCatalogInfo& catalogInfo );
   
   
   template <> struct Converter< FSSpec, FSRef >: public std::unary_function< FSRef, FSSpec >
     {
      FSSpec operator()( const FSRef& ref ) const
        {
         FSSpec result;
         FSGetCatalogInfo( ref, 0, 0, 0, &result, 0 );
         return result;
        }
     };


#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template< ::FSCatalogInfoBitmap bit >
   struct FSSetCatalogInfo_Traits
     {
      typedef const typename FSCatalogInfoBit_Traits<bit>::Type& ParameterType;
     };

   template< ::FSCatalogInfoBitmap bit >
   void FSSetCatalogInfo( const FSRef& ref, typename FSSetCatalogInfo_Traits<bit>::ParameterType value )
     {
      FSCatalogInfo info; 
      FSCatalogInfoBit_Traits<bit>::Set( info, value );
      FSSetCatalogInfo( ref, bit, info );
     }
#endif

   template <> struct Disposer<FSIterator>: public std::unary_function< FSIterator, void >,
                                            private DefaultDestructionOSStatusPolicy
     {
      void operator()( FSIterator iterator ) const
        {
         OnlyOnce<RegisterFileManagerErrors>();
         DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::FSCloseIterator( iterator ) );
        }
     };

   Owned<FSIterator> FSOpenIterator( const FSRef& container, FSIteratorFlags iteratorFlags );
    
   void FSCloseIterator( Owned<FSIterator> );

#if 0
   template < class Specifics >
   class FSIteratedContainer: private Specifics
     {
      private:
          FSRef root;
         
         // not implemented:
            FSIteratedContainer& operator=( const FSIteratedContainer& );

      public:
         typedef typename Specifics::value_type value_type;
         typedef typename Specifics::size_type size_type;
         typedef typename Specifics::difference_type difference_type;

         typedef value_type& reference;
         typedef const value_type& const_reference;
         
         class const_iterator
           {
            friend class FSIteratedContainer;
 
            public:
               typedef IndexUntilFailureContainer::difference_type difference_type;
               typedef IndexUntilFailureContainer::value_type value_type;
               typedef const value_type *pointer;
               typedef const value_type& reference;
               typedef std::input_iterator_tag iterator_category;
           
            private:
               Shared<FSIterator> fsIterator;
               value_type value;
               
               void GetValue()
                 {
                  FSGetCatalogInfoBulk_Result infoResult;
                  do
                     infoResult = Specifics::GetValue( fsIterator, &value );
                  while ( infoResult.actualObjects == 0 && !infoResult.noMoreItems );
                  if ( infoResult.noMoreItems )
                     fsIterator = Shared<FSIterator>();
                 }
               
               const_iterator( const FSRef& root, FSIteratorFlags flags )
                 : fsIterator( FSOpenIterator( root, flags ) )
                 {
                  GetValue();
                 }
               
            public:
               const_iterator()                          {}
               
               const_iterator& operator++()              { GetValue(); return *this; }
               const_iterator operator++(int)            { GetValue(); return *this; }
               
               reference operator*() const               { return value; }
               pointer operator->() const                { return &value; }
               
               friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.fsIterator == b.fsIterator; }
               friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
           };
      
         FSIteratedContainer( const Specifics& base )
           : Specifics( base )
           {}
         
         const_iterator begin() const                    { return const_iterator( *this, Specifics::begin_position() ); }
         const_iterator end() const                      { return const_iterator(); }
     };
#endif
   
   struct FSGetCatalogInfoBulk_Result
     {
      ItemCount actualObjects;
      bool      noMoreItems;
     };
   
   class FSContainterChanged {};
   
   FSGetCatalogInfoBulk_Result FSGetCatalogInfoBulk( FSIterator          iterator,
                                                     ItemCount           maximumObjects,
                                                     FSCatalogInfoBitmap whichInfo,
                                                     FSCatalogInfo *     catalogInfos,
                                                     FSRef *             refs = 0,
                                                     FSSpec *            specs = 0,
                                                     HFSUniStr255 *      names = 0 );
   
   void FSGetCatalogInfoBulk( FSIterator                  iterator,
                              FSCatalogInfoBitmap         whichInfo,
                              std::vector<FSCatalogInfo> *catalogInfos,
                              std::vector<FSRef>         *refs = 0,
                              std::vector<FSSpec>        *specs = 0,
                              std::vector<HFSUniStr255>  *names = 0 );
                              
#if 0  
   template < ::FSCatalogInfoBitmap whichInfo >
   class FSCatalogInfo_ContainerSpecifics
     {
      public:
         typedef FSCatalogInfo value_type;
         typedef FSCatalogIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef ErrFSNoMoreItems EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetCatalogInfo( position, whichInfo );
           }
     };

   template < ::FSCatalogInfoBitmap whichInfo > class FSCatalogInfo_Container;
   template < ::FSCatalogInfoBitmap whichInfo > FSCatalogInfo_Container<whichInfo> FSCatalogInfos();
   
   template < ::FSCatalogInfoBitmap whichInfo >
   class FSCatalogInfo_Container: public IndexUntilFailureContainer< FSCatalogInfo_ContainerSpecifics<whichInfo> >
     {
      friend FSCatalogInfo_Container<whichInfo> FSCatalogInfos<whichInfo>();

      private:
         FSCatalogInfo_Container()
           : IndexUntilFailureContainer< FSCatalogInfo_ContainerSpecifics<whichInfo> >( FSCatalogInfo_ContainerSpecifics<whichInfo>() )
           {}
     };
   
   template < ::FSCatalogInfoBitmap whichInfo >
   FSCatalogInfo_Container<whichInfo> FSCatalogInfos()
     {
      return FSCatalogInfo_Container<whichInfo>();
     }
   
   
   template < ::FSCatalogInfoBitmap whichInfo >
   CatalogInfo_Container<whichInfo> CatalogInfos( const FSRef& container );
   CatalogRef_Container  CatalogRefs( const FSRef& container );
   CatalogSpec_Container CatalogSpecs( const FSRef& container );
   CatalogName_Container CatalogNames( const FSRef& container );
#endif

   typedef FSGetCatalogInfoBulk_Result FSCatalogSearch_Result;

   FSCatalogSearch_Result FSCatalogSearch( FSIterator            iterator,
                                           const FSSearchParams& searchCriteria,
                                           ItemCount             maximumObjects,
                                           FSCatalogInfoBitmap   whichInfo,
                                           FSCatalogInfo *       catalogInfos,
                                           FSRef *               refs,
                                           FSSpec *              specs,
                                           HFSUniStr255 *        names );

   class FSForkRef
     {
      private:
         FSRef file;
         UniString name;
      
      public:
         FSForkRef()
           : file(),
             name()
           {}
         
         FSForkRef( const FSRef& theFile, const UniString& theName )
           : file( theFile ),
             name( theName )
           {}
         
         FSForkRef( const FSRef& f, UniCharCount nameLength, const UniChar *theName )
           : file( f ),
             name( theName, theName+nameLength )
           {}
         
         const FSRef& File() const            { return file; }
         const UniString& Name() const    { return name; }
     };
   
   inline bool operator==( const FSForkRef& a, const FSForkRef& b )
     {
      return a.Name() == b.Name() && a.File() == b.File();
     }
   
   inline bool operator!=( const FSForkRef& a, const FSForkRef& b )
     {
      return !( a == b );
     }
   
   template <> struct Disposer<FSForkRef>: public std::unary_function< FSForkRef, void >,
                                           private DefaultDestructionOSStatusPolicy
     {
      void operator()( const FSForkRef& fork ) const
        {
         OnlyOnce<RegisterFileManagerErrors>();
         DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::FSDeleteFork( &fork.File(),
                                                                                      fork.Name().size(),
                                                                                      fork.Name().data() ) );
        }
     };

   Owned<FSForkRef> FSCreateFork( const FSRef&   ref,
                                  UniCharCount   forkNameLength,
                                  const UniChar *forkName );

   Owned<FSForkRef> FSCreateFork( const FSRef&     ref,
                                  const UniString& forkName );

   void FSDeleteFork( Owned<FSForkRef> );
   
   struct FSIterateForks_Result
     {
      HFSUniStr255 forkName;
      UInt64       forkSize;
      UInt64       forkPhysicalSize;
     };
   
   FSIterateForks_Result FSIterateForks( const FSRef&    ref,
                                         CatPositionRec& forkIterator );
   
   template <> struct Disposer<FSForkRefNum>: public std::unary_function< FSForkRefNum, void >,
                                              private DefaultDestructionOSStatusPolicy
     {
      void operator()( FSForkRefNum fork ) const
        {
         OnlyOnce<RegisterFileManagerErrors>();
         HandleDestructionOSStatus( ::FSCloseFork( fork ) );
        }
     };
   
   Owned<FSForkRefNum> FSOpenFork( const FSRef&   ref,
                                   UniCharCount   forkNameLength,
                                   const UniChar *forkName,
                                   FSIOPermssn    permissions );

   Owned<FSForkRefNum> FSOpenFork( const FSRef&    ref,
                                   const UniString forkName,
                                   FSIOPermssn     permissions );

   Owned<FSForkRefNum> FSOpenFork( const FSForkRef& fork,
                                   FSIOPermssn      permissions );
   
   ByteCount FSReadFork( FSForkRefNum fork,
                         FSIOPosMode  positionMode,
                         SInt64       positionOffset,
                         ByteCount    requestCount,
                         void *       buffer );

   inline ByteCount FSReadFork( FSForkRefNum fork,
                                ByteCount    requestCount,
                                void *       buffer )
     {
      return FSReadFork( fork, FSAtMark(), 0, requestCount, buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSReadFork( FSForkRefNum fork,
                         FSIOPosMode  positionMode,
                         SInt64       positionOffset,
                         Element      (&buffer)[count] )
     {
      return FSReadFork( fork, positionMode, positionOffset, count*sizeof(Element), buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSReadFork( FSForkRefNum fork,
                         Element      (&buffer)[count] )
     {
      return FSReadFork( fork, count*sizeof(Element), buffer );
     }

   ByteCount FSWriteFork( FSForkRefNum fork,
                          FSIOPosMode  positionMode,
                          SInt64       positionOffset,
                          ByteCount    requestCount,
                          const void * buffer );

   inline ByteCount FSWriteFork( FSForkRefNum fork,
                                 ByteCount    requestCount,
                                 const void * buffer )
     {
      return FSWriteFork( fork, FSAtMark(), 0, requestCount, buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSWriteFork( FSForkRefNum  fork,
                          FSIOPosMode   positionMode,
                          SInt64        positionOffset,
                          const Element (&buffer)[count] )
     {
      return FSWriteFork( fork, positionMode, positionOffset, count*sizeof(Element), buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSWriteFork( FSForkRefNum  fork,
                          const Element (&buffer)[count] )
     {
      return FSWriteFork( fork, count*sizeof(Element), buffer );
     }
   
   SInt64 FSGetForkPosition( FSForkRefNum forkRefNum );
   
   void FSSetForkPosition( FSForkRefNum forkRefNum,
                           FSIOPosMode  positionMode,
                           SInt64       positionOffset );
   
   SInt64 FSGetForkSize( FSForkRefNum forkRefNum );

   void FSSetForkSize( FSForkRefNum forkRefNum,
                       FSIOPosMode  positionMode,
                       SInt64       positionOffset );
   
   UInt64 FSAllocateFork( FSForkRefNum      forkRefNum,
                          FSAllocationFlags flags,
                          FSIOPosMode       positionMode,
                          SInt64            positionOffset,
                          UInt64            requestCount );
   
   void FSFlushFork( FSForkRefNum forkRefNum );
   
   void FSCloseFork( Owned<FSForkRefNum> forkRefNum );
   
   struct FSGetForkCBInfo_Result
     {
      FSForkRefNum  actualRefNum;
      FSForkInfo    forkInfo;
      FSRef         ref;
      HFSUniStr255  outForkName;
     };
   
   FSGetForkCBInfo_Result FSGetForkCBInfo( FSForkRefNum desiredRefNum );

   FSGetForkCBInfo_Result FSGetForkCBInfo( FSVolumeRefNum  volume,
                                           FSForkIterator& iterator );

   FSGetForkCBInfo_Result FSGetForkCBInfo( FSForkIterator& iterator );


#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template < class DesiredType, class MemberType, MemberType FSVolumeInfo::*member >
   struct Basic_FSVolumeInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSVolumeInfo& info )                { return info.*member; }
      static void Set( FSVolumeInfo& info, const Type& value )   { info.*member = value; }
     };

   template < class DesiredType, class MemberType, MemberType FSVolumeInfo::*member >
   struct Const_FSVolumeInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSVolumeInfo& info )                { return info.*member; }
     };
   
   template < ::FSVolumeInfoBitmap bit > struct FSVolumeInfoBit_Traits;
   
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoCreateDate  > : Const_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::createDate     > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoModDate     > : Const_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::modifyDate     > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoBackupDate  > : Basic_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::backupDate     > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoCheckedDate > : Const_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::checkedDate    > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFileCount   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::fileCount      > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoDirCount    > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::folderCount    > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoNextAlloc   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::nextAllocation > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoRsrcClump   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::rsrcClumpSize  > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoDataClump   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::dataClumpSize  > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoNextID      > : Const_FSVolumeInfoBit_Traits< HFSCatalogNodeID,  ::UInt32,        &FSVolumeInfo::nextCatalogID  > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFlags       > : Basic_FSVolumeInfoBit_Traits< FSVolumeInfoFlags, ::UInt16,        &FSVolumeInfo::flags          > {};
#endif

   struct FSVolumeInfoSizes
     {
      UInt64 totalBytes;
      UInt64 freeBytes;
     };

#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoSizes >
     {
      typedef FSVolumeInfoSizes Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoSizes result;
         result.totalBytes = info.totalBytes;
         result.freeBytes = info.freeBytes;
         return result;
        }
     };
#endif

   struct FSVolumeInfoBlocks
     {
      UInt32 blockSize;
      UInt32 totalBlocks;
      UInt32 freeBlocks;
     };

#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoBlocks >
     {
      typedef FSVolumeInfoBlocks Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoBlocks result;
         result.blockSize = info.blockSize;
         result.totalBlocks = info.totalBlocks;
         result.freeBlocks = info.freeBlocks;
         return result;
        }
     };
#endif
   
   struct FSVolumeInfoFinderInfo
     {
      UInt8 finderInfo[32];
     };
   
#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFinderInfo >
     {
      typedef FSVolumeInfoFinderInfo Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoFinderInfo result;
         ArrayAssign( result.finderInfo, info.finderInfo );
         return result;
        }

      static void Set( FSVolumeInfo& info, const FSVolumeInfoFinderInfo& value )
        {
         ArrayAssign( info.finderInfo, value.finderInfo );
        }      
     };
#endif
   
   struct FSVolumeInfoFSInfo
     {
      FSFileSystemID filesystemID;
      UInt16 signature;
     };

#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFSInfo >
     {
      typedef FSVolumeInfoFSInfo Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoFSInfo result;
         result.filesystemID = FSFileSystemID( info.filesystemID );
         result.signature = info.signature;
         return result;
        }
     };
#endif
   
   struct FSVolumeInfoDriveInfo
     {
      UInt16 driveNumber;
      DriverReferenceNumber driverRefNum;
     };

#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoDriveInfo >
     {
      typedef FSVolumeInfoDriveInfo Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoDriveInfo result;
         result.driveNumber = info.driveNumber;
         result.driverRefNum = DriverReferenceNumber( info.driverRefNum );
         return result;
        }
     };
#endif


   struct FSGetVolumeInfo_Result
     {
      FSVolumeRefNum actualVolume;
      FSVolumeInfo   info;
      HFSUniStr255   volumeName;
      FSRef          rootDirectory;
     };

   void FSGetVolumeInfo( FSVolumeRefNum     volume,
                         FSVolumeIndex      volumeIndex,
                         FSVolumeRefNum *   actualVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory );

   void FSGetVolumeInfo( FSVolumeRefNum     volume,
                         FSVolumeRefNum *   actualVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory );

   void FSGetVolumeInfo( FSVolumeIndex      volumeIndex,
                         FSVolumeRefNum *   actualVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory );
   
   FSGetVolumeInfo_Result FSGetVolumeInfo( FSVolumeRefNum volume,
                                           FSVolumeInfoBitmap whichInfo );
   
   FSGetVolumeInfo_Result FSGetVolumeInfo( FSVolumeIndex volumeIndex,
                                           FSVolumeInfoBitmap whichInfo);

#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template< ::FSVolumeInfoBitmap bit >
   struct FSGetVolumeInfo_Traits
     {
      typedef typename FSVolumeInfoBit_Traits<bit>::Type Result;
     };
   
   template< ::FSVolumeInfoBitmap bit >
   typename FSGetVolumeInfo_Traits<bit>::Result FSGetVolumeInfo( FSVolumeRefNum volume )
     {
      FSVolumeInfo info;
      FSGetVolumeInfo( volume, 0, bit, &info, 0, 0 );
      return FSVolumeInfoBit_Traits<bit>::Get( info );
     }
   
   template< ::FSVolumeInfoBitmap bit >
   typename FSGetVolumeInfo_Traits<bit>::Result FSGetVolumeInfo( FSVolumeIndex volume )
     {
      FSVolumeInfo info;
      FSGetVolumeInfo( volume, 0, bit, &info, 0, 0 );
      return FSVolumeInfoBit_Traits<bit>::Get( info );
     }
#endif
     
   FSVolumeRefNum FSGetVolumeRefNum( FSVolumeIndex volumeIndex );
   
   HFSUniStr255 FSGetVolumeName( FSVolumeRefNum volume );
   HFSUniStr255 FSGetVolumeName( FSVolumeIndex volumeIndex );
   
   FSRef FSGetVolumeRootDirectory( FSVolumeRefNum volume );
   FSRef FSGetVolumeRootDirectory( FSVolumeIndex volumeIndex );

   class Volume_ContainerSpecifics
     {
      public:
         typedef FSVolumeRefNum value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeRefNum( position );
           }
     };

   class Volume_Container: public IndexUntilFailureContainer< Volume_ContainerSpecifics >
     {
      friend Volume_Container Volumes();
      private:
         Volume_Container()
           : IndexUntilFailureContainer< Volume_ContainerSpecifics >( Volume_ContainerSpecifics() )
           {}
     };

   inline Volume_Container Volumes()
     {
      return Volume_Container();
     }

   typedef Volume_Container FSVolumeRefNum_Container;
   
   inline FSVolumeRefNum_Container VolumeRefNums()
     {
      return Volumes();
     }


   typedef FSGetVolumeInfo_Result VolumeInfo;

   template < ::FSVolumeInfoBitmap whichInfo >
   class VolumeInfo_ContainerSpecifics
     {
      public:
         typedef VolumeInfo value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeInfo( position, whichInfo );
           }
     };

   template < ::FSVolumeInfoBitmap whichInfo > class VolumeInfo_Container;
   template < ::FSVolumeInfoBitmap whichInfo > VolumeInfo_Container<whichInfo> VolumeInfos();
   
   template < ::FSVolumeInfoBitmap whichInfo >
   class VolumeInfo_Container: public IndexUntilFailureContainer< VolumeInfo_ContainerSpecifics<whichInfo> >
     {
      friend VolumeInfo_Container<whichInfo> VolumeInfos<whichInfo>();

      private:
         VolumeInfo_Container()
           : IndexUntilFailureContainer< VolumeInfo_ContainerSpecifics<whichInfo> >( VolumeInfo_ContainerSpecifics<whichInfo>() )
           {}
     };
   
   template < ::FSVolumeInfoBitmap whichInfo >
   VolumeInfo_Container<whichInfo> VolumeInfos()
     {
      return VolumeInfo_Container<whichInfo>();
     }

   class VolumeName_ContainerSpecifics
     {
      public:
         typedef HFSUniStr255 value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeName( position );
           }
     };

   class VolumeName_Container: public IndexUntilFailureContainer< VolumeName_ContainerSpecifics >
     {
      friend VolumeName_Container VolumeNames();
      private:
         VolumeName_Container()
           : IndexUntilFailureContainer< VolumeName_ContainerSpecifics >( VolumeName_ContainerSpecifics() )
           {}
     };

   inline VolumeName_Container VolumeNames()
     {
      return VolumeName_Container();
     }

   class VolumeRootDirectory_ContainerSpecifics
     {
      public:
         typedef FSRef value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeRootDirectory( position );
           }
     };

   class VolumeRootDirectory_Container: public IndexUntilFailureContainer< VolumeRootDirectory_ContainerSpecifics >
     {
      friend VolumeRootDirectory_Container VolumeRootDirectories();
      private:
         VolumeRootDirectory_Container()
           : IndexUntilFailureContainer< VolumeRootDirectory_ContainerSpecifics >( VolumeRootDirectory_ContainerSpecifics() )
           {}
     };

   inline VolumeRootDirectory_Container VolumeRootDirectories()
     {
      return VolumeRootDirectory_Container();
     }

   void FSSetVolumeInfo( FSVolumeRefNum volume,
                         FSVolumeInfoBitmap  whichInfo,
                         const FSVolumeInfo& volumeInfo );
   
#ifndef NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
   template< ::FSVolumeInfoBitmap bit >
   struct FSSetVolumeInfo_Traits
     {
      typedef const typename FSVolumeInfoBit_Traits<bit>::Type& ParameterType;
     };

   template< ::FSVolumeInfoBitmap bit >
   void FSSetVolumeInfo( FSVolumeRefNum volume, typename FSSetVolumeInfo_Traits<bit>::ParameterType value )
     {
      FSVolumeInfo info;
      FSVolumeInfoBit_Traits<bit>::Set( info, value );
      FSSetVolumeInfo( volume, bit, info );
     }
#endif
   
   
   HFSUniStr255 FSGetDataForkName();
   
   HFSUniStr255 FSGetResourceForkName();
   
   void FSRefMakePath( const FSRef& ref,
                       UInt8 *      path,
                       UInt32       maxPathSize );

   template < UInt32 size >
   void FSRefMakePath( const FSRef& ref,
                       UInt8 (&path)[size] )
     {
      FSRefMakePath( ref, path, size );
     }

   std::string FSRefMakePath( const FSRef& ref );
   
   struct FSPathMakeRef_Result
     {
      FSRef ref;
      bool isDirectory;
      
      operator const FSRef&() const  { return ref; }
     };
   
   FSPathMakeRef_Result FSPathMakeRef( const UInt8 *path );
   FSPathMakeRef_Result FSPathMakeRef( const std::string& path );
   
   template <> struct Converter< FSRef, std::string >: public std::unary_function< std::string, FSRef >
     {
      FSRef operator()( const std::string& path ) const
        {
         return FSPathMakeRef( path );
        }
     };

   template <> struct Converter< std::string, FSRef >: public std::unary_function< FSRef, std::string >
     {
      std::string operator()( const FSRef& ref ) const
        {
         return FSRefMakePath( ref );
        }
     };
  }

#endif
