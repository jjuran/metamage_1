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
	
   template<> struct DescType_Traits< typeFixedPoint                >: POD_DescType_Traits< FixedPoint                > {};
   template<> struct DescType_Traits< typeFixedRectangle            >: POD_DescType_Traits< FixedRect                 > {};
   template<> struct DescType_Traits< typeIntlWritingCode           >: POD_DescType_Traits< ScriptCode                > {};
   template<> struct DescType_Traits< typePtr                       >: POD_DescType_Traits< void *                    > {};
   template<> struct DescType_Traits< typeQDPoint                   >: POD_DescType_Traits< Point                     > {};
   template<> struct DescType_Traits< typeRectangle                 >: POD_DescType_Traits< Rect                      > {};
   template<> struct DescType_Traits< typeRGBColor                  >: POD_DescType_Traits< RGBColor                  > {};
   template<> struct DescType_Traits< typeScript                    >: POD_DescType_Traits< ScriptCode                > {};
   template<> struct DescType_Traits< typeTextRange                 >: POD_DescType_Traits< TextRange                 > {};
   template<> struct DescType_Traits< typeComponentInstance         >: POD_DescType_Traits< ComponentInstance         > {};
   template<> struct DescType_Traits< typeEventRef                  >: POD_DescType_Traits< EventRef                  > {};

   template<> struct DescType_Traits< typeUnicodeText >
     {
      typedef UniString Result;
      typedef const UniString& Parameter;
      
      typedef UniString        InputBuffer;
      typedef const UniString& OutputBuffer;

      static const bool inputHasVariableLength = true;
      static void SetInputBufferLength( InputBuffer& b, std::size_t s )     { b.resize( s/sizeof(UniChar) ); }

      static void *      InputBufferStart(  InputBuffer& buffer )           { return buffer.empty() ? 0 : &buffer[0]; }
      static std::size_t InputBufferLength( InputBuffer& buffer )           { return buffer.size() * sizeof(UniChar); }
      static Result  ProcessInputBuffer( InputBuffer& buffer )              { return buffer; }

      static const void *OutputBufferStart(  OutputBuffer buffer )          { return buffer.data(); }
      static std::size_t OutputBufferLength( OutputBuffer buffer )          { return buffer.size() * sizeof(UniChar); }
      static OutputBuffer PrepareOutputBuffer( Parameter output )           { return output; }
      static void ReleaseOutputBuffer( OutputBuffer )                       {}
     };
	
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
   
   template<> struct DescType_Traits< typeVersion        >: VariableLengthPOD_DescType_Traits< VersRec,        SizeOf_VersRec        > {};
   template<> struct DescType_Traits< typeOffsetArray    >: VariableLengthPOD_DescType_Traits< OffsetArray,    SizeOf_OffsetArray    > {};
   template<> struct DescType_Traits< typeTextRangeArray >: VariableLengthPOD_DescType_Traits< TextRangeArray, SizeOf_TextRangeArray > {};
  }

#endif
