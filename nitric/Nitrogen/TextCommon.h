// TextCommon.h

#ifndef NITROGEN_TEXTCOMMON_H
#define NITROGEN_TEXTCOMMON_H

#ifndef __TEXTCOMMON__
#include <TextCommon.h>
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_SELECTORTYPE_H
#include "Nitrogen/SelectorType.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_MAKE_H
#include "Nitrogen/Make.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif

namespace Nitrogen
  {
   void RegisterTextEncodingConversionManagerErrors();
   
   class TextEncodingBaseTag {};
   typedef SelectorType< TextEncodingBaseTag, ::TextEncodingBase, kTextEncodingUnknown > TextEncodingBase;

   class TextEncodingVariantTag {};
   typedef SelectorType< TextEncodingVariantTag, ::TextEncodingVariant, kTextEncodingDefaultVariant > TextEncodingVariant;

   class TextEncodingFormatTag {};
   typedef SelectorType< TextEncodingFormatTag, ::TextEncodingFormat, 0 > TextEncodingFormat;

   class TextEncodingTag {};
   typedef IDType< TextEncodingTag, ::TextEncoding, 0 > TextEncoding;

   class TextEncodingNameSelectorTag {};
   typedef SelectorType< TextEncodingNameSelectorTag, ::TextEncodingNameSelector, 0 > TextEncodingNameSelector;

   using ::TextEncodingRun;

   template <>
   struct Maker< TextEncodingRun >
     {
      TextEncodingRun operator()( ByteOffset offset, TextEncoding textEncoding ) const
        {
         TextEncodingRun result;
         result.offset = offset;
         result.textEncoding = textEncoding;
         return result;
        }
     };

   using ::ScriptCodeRun;

   template <>
   struct Maker< ScriptCodeRun >
     {
      ScriptCodeRun operator()( ByteOffset offset, ScriptCode script ) const
        {
         ScriptCodeRun result;
         result.offset = offset;
         result.script = script;
         return result;
        }
     };
   
   class UCCharPropertyTypeTag {};
   typedef SelectorType< UCCharPropertyTypeTag, ::UCCharPropertyType, 0 > UCCharPropertyType;

   class UCCharPropertyValueTag {};
   typedef SelectorType< UCCharPropertyValueTag, ::UCCharPropertyValue, 0 > UCCharPropertyValue;
   
   // UCCharPropertyValue is really three types overloaded: UCCharGenlCategory, UCCharCombiningClass, and UCCharBidiCategory.
   
      class UCCharGenlCategoryTag {};
      typedef SelectorType< UCCharGenlCategoryTag, ::UCCharPropertyValue, 0 > UCCharGenlCategory;
      
      class UCCharCombiningClassTag {};
      typedef SelectorType< UCCharCombiningClassTag, ::UCCharPropertyValue, 0 > UCCharCombiningClass;
      
      class UCCharBidiCategoryTag {};
      typedef SelectorType< UCCharBidiCategoryTag, ::UCCharPropertyValue, 0 > UCCharBidiCategory;


   /* TECInfo... */
   
   inline TextEncoding CreateTextEncoding( TextEncodingBase encodingBase,
                                           TextEncodingVariant encodingVariant,
                                           TextEncodingFormat encodingFormat )
     {
      return ::CreateTextEncoding( encodingBase, encodingVariant, encodingFormat );
     }
   
   inline TextEncodingBase GetTextEncodingBase( TextEncoding encoding )
     {
      return ::GetTextEncodingBase( encoding );
     }
   
   inline TextEncodingVariant GetTextEncodingVariant( TextEncoding encoding )
     {
      return ::GetTextEncodingVariant( encoding );
     }
   
   inline TextEncodingFormat GetTextEncodingFormat( TextEncoding encoding )
     {
      return ::GetTextEncodingFormat( encoding );
     }
   
   inline TextEncoding ResolveDefaultTextEncoding( TextEncoding encoding )
     {
      return ::ResolveDefaultTextEncoding( encoding );
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

   Owned< TECInfoHandle, Disposer<Handle> > TECGetInfo();

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
      return UCGetCharProperty( charPtr, textLength, propType ).Get();
     }
   
   template < ::UCCharPropertyType propType >
   typename UCGetCharProperty_Traits<propType>::Result UCGetCharProperty( const UniString& text )
     {
      return UCGetCharProperty( text, propType ).Get();
     }
  }

#endif
