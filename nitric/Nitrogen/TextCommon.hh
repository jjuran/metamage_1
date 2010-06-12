// Nitrogen/TextCommon.hh
// ----------------------

// Part of the Nitrogen project.
//
// Written 2004-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_TEXTCOMMON_HH
#define NITROGEN_TEXTCOMMON_HH

#ifndef __TEXTCOMMON__
#include <TextCommon.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/make.hh"
#include "nucleus/string.hh"

// Nitrogen
#include "Mac/Memory/Types/Handle.hh"

#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif


namespace Nitrogen
  {
  
	class TextEncodingConversionManagerErrorsRegistrationDependency
	{
		public:
			TextEncodingConversionManagerErrorsRegistrationDependency();
	};
	
   
   static const LangCode kTextLanguageDontCare = LangCode( ::kTextLanguageDontCare );
   static const RegionCode kTextRegionDontCare = RegionCode( ::kTextRegionDontCare );
   static const ScriptCode kTextScriptDontCare = ScriptCode( ::kTextScriptDontCare );
   
	enum TextEncodingBase
	{
		kTextEncodingBase_Max = nucleus::enumeration_traits< ::TextEncodingBase >::max
	};
	
	enum TextEncodingVariant
	{
		kTextEncodingDefaultVariant = ::kTextEncodingDefaultVariant,
		
		kTextEncodingVariant_Max = nucleus::enumeration_traits< ::TextEncodingVariant >::max
	};
	
	enum TextEncodingFormat
	{
		kTextEncodingFormat_Max = nucleus::enumeration_traits< ::TextEncodingFormat >::max
	};
	
	enum TextEncoding
	{
		kTextEncodingUnknown = ::kTextEncodingUnknown,
		
		kTextEncoding_Max = nucleus::enumeration_traits< ::TextEncoding >::max
	};
	
	enum TextEncodingNameSelector
	{
		kTextEncodingNameSelector_Max = nucleus::enumeration_traits< ::TextEncodingNameSelector >::max
	};
	
   using ::TextEncodingRun;
   using ::ScriptCodeRun;
  }

namespace nucleus
  {
   template <>
   struct maker< TextEncodingRun >
     {
      TextEncodingRun operator()( ByteOffset offset, TextEncoding textEncoding ) const
        {
         TextEncodingRun result;
         result.offset = offset;
         result.textEncoding = textEncoding;
         return result;
        }
     };

   template <>
   struct maker< ScriptCodeRun >
     {
      ScriptCodeRun operator()( ByteOffset offset, ScriptCode script ) const
        {
         ScriptCodeRun result;
         result.offset = offset;
         result.script = script;
         return result;
        }
     };
  }

namespace Nitrogen
  {
	enum UCCharPropertyType
	{
		kUCCharPropertyType_Max = nucleus::enumeration_traits< ::UCCharPropertyType >::max
	};
	
	enum UCCharPropertyValue
	{
		kUCCharPropertyValue_Max = nucleus::enumeration_traits< ::UCCharPropertyValue >::max
	};
	
   // UCCharPropertyValue is really three types overloaded: UCCharGenlCategory, UCCharCombiningClass, and UCCharBidiCategory.
   
	enum UCCharGenlCategory
	{
		kUCCharGenlCategory_Max = nucleus::enumeration_traits< ::UCCharPropertyValue >::max
	};
	
	enum UCCharCombiningClass
	{
		kUCCharCombiningClass_Max = nucleus::enumeration_traits< ::UCCharPropertyValue >::max
	};
	  
	enum UCCharBidiCategory
	{
		kUCCharBidiCategory_Max = nucleus::enumeration_traits< ::UCCharPropertyValue >::max
	};
	

   /* TECInfo... */
   
   inline TextEncoding CreateTextEncoding( TextEncodingBase encodingBase,
                                           TextEncodingVariant encodingVariant,
                                           TextEncodingFormat encodingFormat )
     {
      return TextEncoding( ::CreateTextEncoding( encodingBase, encodingVariant, encodingFormat ) );
     }
   
   inline TextEncodingBase GetTextEncodingBase( TextEncoding encoding )
     {
      return TextEncodingBase( ::GetTextEncodingBase( encoding ) );
     }
   
   inline TextEncodingVariant GetTextEncodingVariant( TextEncoding encoding )
     {
      return TextEncodingVariant( ::GetTextEncodingVariant( encoding ) );
     }
   
   inline TextEncodingFormat GetTextEncodingFormat( TextEncoding encoding )
     {
      return TextEncodingFormat( ::GetTextEncodingFormat( encoding ) );
     }
   
   inline TextEncoding ResolveDefaultTextEncoding( TextEncoding encoding )
     {
      return TextEncoding( ::ResolveDefaultTextEncoding( encoding ) );
     }
   
   struct GetTextEncodingName_Result
     {
      ByteCount    oNameLength;
      RegionCode   oActualRegion;
      TextEncoding oActualEncoding;
     };
   
   GetTextEncodingName_Result GetTextEncodingName( TextEncoding             iEncoding,
                                                   TextEncodingNameSelector iNamePartSelector,
                                                   RegionCode               iPreferredRegion,
                                                   TextEncoding             iPreferredEncoding,
                                                   ByteCount                iOutputBufLen,
                                                   UInt8                   *oEncodingName );
   
   template <ByteCount iOutputBufLen>
   inline GetTextEncodingName_Result GetTextEncodingName( TextEncoding             iEncoding,
                                                          TextEncodingNameSelector iNamePartSelector,
                                                          RegionCode               iPreferredRegion,
                                                          TextEncoding             iPreferredEncoding,
                                                          UInt8                    (&oEncodingName)[iOutputBufLen] )
     {
      return GetTextEncoding( iEncoding, iNamePartSelector, iPreferredRegion, iPreferredEncoding, iOutputBufLen, oEncodingName );
     }
   
   struct GetTextEncodingName_ResultWithString
     {
      nucleus::mutable_string  oEncodingName;
      RegionCode               oActualRegion;
      TextEncoding             oActualEncoding;
      
      operator const nucleus::string&() const    { return oEncodingName; }
     };

   GetTextEncodingName_ResultWithString GetTextEncodingName( TextEncoding             iEncoding,
                                                             TextEncodingNameSelector iNamePartSelector,
                                                             RegionCode               iPreferredRegion,
                                                             TextEncoding             iPreferredEncoding );

   nucleus::owned< TECInfoHandle, nucleus::disposer< Mac::Handle > > TECGetInfo();

   TextEncoding UpgradeScriptInfoToTextEncoding( ScriptCode       iTextScriptID,
                                                 LangCode         iTextLanguageID = kTextLanguageDontCare,
                                                 RegionCode       iRegionID       = kTextRegionDontCare,
                                                 ConstStr255Param iTextFontname   = 0 );

   inline TextEncoding UpgradeScriptInfoToTextEncoding( ScriptCode       iTextScriptID,
                                                        RegionCode       iRegionID,
                                                        ConstStr255Param iTextFontname = 0 )
     {
      return UpgradeScriptInfoToTextEncoding( iTextScriptID, kTextLanguageDontCare, iRegionID, iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( ScriptCode       iTextScriptID,
                                                        LangCode         iTextLanguageID,
                                                        ConstStr255Param iTextFontname )
     {
      return UpgradeScriptInfoToTextEncoding( iTextScriptID, iTextLanguageID, kTextRegionDontCare, iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( ScriptCode       iTextScriptID,
                                                        ConstStr255Param iTextFontname )
     {
      return UpgradeScriptInfoToTextEncoding( iTextScriptID, kTextLanguageDontCare, kTextRegionDontCare, iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( LangCode         iTextLanguageID,
                                                        RegionCode       iRegionID       = kTextRegionDontCare,
                                                        ConstStr255Param iTextFontname   = 0 )
     {
      return UpgradeScriptInfoToTextEncoding( kTextScriptDontCare, iTextLanguageID, iRegionID, iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( LangCode         iTextLanguageID,
                                                        ConstStr255Param iTextFontname )
     {
      return UpgradeScriptInfoToTextEncoding( kTextScriptDontCare, iTextLanguageID, kTextRegionDontCare, iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( RegionCode       iRegionID,
                                                        ConstStr255Param iTextFontname = 0 )
     {
      return UpgradeScriptInfoToTextEncoding( kTextScriptDontCare, kTextLanguageDontCare, iRegionID, iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( ConstStr255Param iTextFontname )
     {
      return UpgradeScriptInfoToTextEncoding( kTextScriptDontCare, kTextLanguageDontCare, kTextRegionDontCare, iTextFontname );
     }

   struct RevertTextEncodingToScriptInfo_Result
     {
      ScriptCode oTextScriptID;
      LangCode   oTextLanguageID;
      Str255     oTextFontname;
     };
   
   RevertTextEncodingToScriptInfo_Result RevertTextEncodingToScriptInfo( TextEncoding iEncoding );

   struct NearestMacTextEncodings_Result
     {
      TextEncoding bestMacEncoding;
      TextEncoding alternateMacEncoding;
     };

   NearestMacTextEncodings_Result NearestMacTextEncodings( TextEncoding generalEncoding );

   UCCharPropertyValue UCGetCharProperty( const UniChar     *charPtr,
                                          UniCharCount       textLength,
                                          UCCharPropertyType propType );

   UCCharPropertyValue UCGetCharProperty( const UniString&   text,
                                          UCCharPropertyType propType );
   
   template < ::UCCharPropertyType propType >
   struct UCGetCharProperty_Traits;

   template <> struct UCGetCharProperty_Traits< kUCCharPropTypeGenlCategory   > { typedef UCCharGenlCategory   Result; };
   template <> struct UCGetCharProperty_Traits< kUCCharPropTypeCombiningClass > { typedef UCCharCombiningClass Result; };
   template <> struct UCGetCharProperty_Traits< kUCCharPropTypeBidiCategory   > { typedef UCCharBidiCategory   Result; };
   
   template < ::UCCharPropertyType propType >
   typename UCGetCharProperty_Traits<propType>::Result UCGetCharProperty( const UniChar *charPtr,
                                                                          UniCharCount   textLength )
     {
      return UCGetCharProperty( charPtr, textLength, propType );
     }
   
   template < ::UCCharPropertyType propType >
   typename UCGetCharProperty_Traits<propType>::Result UCGetCharProperty( const UniString& text )
     {
      return UCGetCharProperty( text, propType );
     }
  }

#endif
