// Nitrogen/TextCommon.h
// ---------------------

// Part of the Nitrogen project.
//
// Written 2004-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_TEXTCOMMON_H
#define NITROGEN_TEXTCOMMON_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __TEXTCOMMON__
#include FRAMEWORK_HEADER(CarbonCore,TextCommon.h)
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#include "Nucleus/Enumeration.h"
#ifndef NUCLEUS_MAKE_H
#include "Nucleus/Make.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
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
		kTextEncodingBase_Max = Nucleus::Enumeration_Traits< ::TextEncodingBase >::max
	};
	
	enum TextEncodingVariant
	{
		kTextEncodingDefaultVariant = ::kTextEncodingDefaultVariant,
		
		kTextEncodingVariant_Max = Nucleus::Enumeration_Traits< ::TextEncodingVariant >::max
	};
	
	enum TextEncodingFormat
	{
		kTextEncodingFormat_Max = Nucleus::Enumeration_Traits< ::TextEncodingFormat >::max
	};
	
	enum TextEncoding
	{
		kTextEncodingUnknown = ::kTextEncodingUnknown,
		
		kTextEncoding_Max = Nucleus::Enumeration_Traits< ::TextEncoding >::max
	};
	
	enum TextEncodingNameSelector
	{
		kTextEncodingNameSelector_Max = Nucleus::Enumeration_Traits< ::TextEncodingNameSelector >::max
	};
	
   using ::TextEncodingRun;
   using ::ScriptCodeRun;
  }

namespace Nucleus
  {
   template <>
   struct Maker< Nitrogen::TextEncodingRun >
     {
      Nitrogen::TextEncodingRun operator()( ByteOffset offset, Nitrogen::TextEncoding textEncoding ) const
        {
         Nitrogen::TextEncodingRun result;
         result.offset = offset;
         result.textEncoding = textEncoding;
         return result;
        }
     };

   template <>
   struct Maker< Nitrogen::ScriptCodeRun >
     {
      Nitrogen::ScriptCodeRun operator()( ByteOffset offset, Nitrogen::ScriptCode script ) const
        {
         Nitrogen::ScriptCodeRun result;
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
		kUCCharPropertyType_Max = Nucleus::Enumeration_Traits< ::UCCharPropertyType >::max
	};
	
	enum UCCharPropertyValue
	{
		kUCCharPropertyValue_Max = Nucleus::Enumeration_Traits< ::UCCharPropertyValue >::max
	};
	
   // UCCharPropertyValue is really three types overloaded: UCCharGenlCategory, UCCharCombiningClass, and UCCharBidiCategory.
   
	enum UCCharGenlCategory
	{
		kUCCharGenlCategory_Max = Nucleus::Enumeration_Traits< ::UCCharPropertyValue >::max
	};
	
	enum UCCharCombiningClass
	{
		kUCCharCombiningClass_Max = Nucleus::Enumeration_Traits< ::UCCharPropertyValue >::max
	};
	  
	enum UCCharBidiCategory
	{
		kUCCharBidiCategory_Max = Nucleus::Enumeration_Traits< ::UCCharPropertyValue >::max
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
      std::string  oEncodingName;
      RegionCode   oActualRegion;
      TextEncoding oActualEncoding;
      
      operator const std::string&() const    { return oEncodingName; }
     };

   GetTextEncodingName_ResultWithString GetTextEncodingName( TextEncoding             iEncoding,
                                                             TextEncodingNameSelector iNamePartSelector,
                                                             RegionCode               iPreferredRegion,
                                                             TextEncoding             iPreferredEncoding );

   Nucleus::Owned< TECInfoHandle, Nucleus::Disposer<Handle> > TECGetInfo();

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
