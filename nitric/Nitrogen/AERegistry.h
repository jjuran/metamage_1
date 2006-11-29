// AERegistry.h

#ifndef NITROGEN_AEREGISTRY_H
#define NITROGEN_AEREGISTRY_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEREGISTRY__
#include FRAMEWORK_HEADER(AE,AERegistry.h)
#endif
#ifndef __CARBONEVENTS__
#include FRAMEWORK_HEADER(HIToolbox,CarbonEvents.h)
#endif
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_APPLEEVENTS_H
#include "Nitrogen/AppleEvents.h"
#endif

namespace Nitrogen
  {
	
   template<> struct DescType_Traits< typeFixedPoint                >: Nucleus::PodFlattener< FixedPoint                > {};
   template<> struct DescType_Traits< typeFixedRectangle            >: Nucleus::PodFlattener< FixedRect                 > {};
   template<> struct DescType_Traits< typeIntlWritingCode           >: Nucleus::PodFlattener< ScriptCode                > {};
   template<> struct DescType_Traits< typePtr                       >: Nucleus::PodFlattener< void *                    > {};
   template<> struct DescType_Traits< typeQDPoint                   >: Nucleus::PodFlattener< Point                     > {};
   template<> struct DescType_Traits< typeRectangle                 >: Nucleus::PodFlattener< Rect                      > {};
   template<> struct DescType_Traits< typeRGBColor                  >: Nucleus::PodFlattener< RGBColor                  > {};
   template<> struct DescType_Traits< typeScript                    >: Nucleus::PodFlattener< ScriptCode                > {};
   template<> struct DescType_Traits< typeTextRange                 >: Nucleus::PodFlattener< TextRange                 > {};
   template<> struct DescType_Traits< typeComponentInstance         >: Nucleus::PodFlattener< ComponentInstance         > {};
   template<> struct DescType_Traits< typeEventRef                  >: Nucleus::PodFlattener< EventRef                  > {};

   template<> struct DescType_Traits< typeUnicodeText >: UnicodeFlattener< UniChar > {};
	
	using ::VersRec;
	
	inline std::size_t SizeOf_VersRec( const VersRec& version )
	{
		UInt8 shortVersionLength = version.shortVersion[ 0 ];
		
		// The long version string immediately follows the short version string.
		const UInt8* longVersion = version.shortVersion + 1 + shortVersionLength;
		UInt8 longVersionLength  = longVersion[ 0 ];
		
		return sizeof (::NumVersion)
		     + sizeof (SInt16)
		     + 1 + shortVersionLength
		     + 1 + longVersionLength;
	}
	
   using ::OffsetArray;

   inline std::size_t SizeOf_OffsetArray( const OffsetArray& array )
     {
      return sizeof( array ) - sizeof( array.fOffset ) + array.fNumOfOffsets * sizeof( long );
     }
   
   using ::TextRangeArray;

   inline std::size_t SizeOf_TextRangeArray( const TextRangeArray& array )
     {
      return sizeof( array ) - sizeof( array.fRange ) + array.fNumOfRanges * sizeof( TextRange );
     }
   
   template<> struct DescType_Traits< typeVersion        >: Nucleus::VariableLengthPodFlattener< VersRec,        SizeOf_VersRec        > {};
   template<> struct DescType_Traits< typeOffsetArray    >: Nucleus::VariableLengthPodFlattener< OffsetArray,    SizeOf_OffsetArray    > {};
   template<> struct DescType_Traits< typeTextRangeArray >: Nucleus::VariableLengthPodFlattener< TextRangeArray, SizeOf_TextRangeArray > {};
  }

#endif
