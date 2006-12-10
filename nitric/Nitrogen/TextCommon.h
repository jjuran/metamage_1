// TextCommon.h

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
#ifndef NUCLEUS_SELECTORTYPE_H
#include "Nucleus/SelectorType.h"
#endif
#ifndef NUCLEUS_FLAGTYPE_H
#include "Nucleus/FlagType.h"
#endif
#ifndef NUCLEUS_IDTYPE_H
#include "Nucleus/IDType.h"
#endif
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
	
   
   inline LangCode TextLanguageDontCare()  { return LangCode::Make( kTextLanguageDontCare ); }
   inline RegionCode TextRegionDontCare()  { return RegionCode::Make( kTextRegionDontCare ); }
   inline ScriptCode TextScriptDontCare()  { return ScriptCode::Make( kTextScriptDontCare ); }
   
   class TextEncodingBaseTag {};
   typedef Nucleus::SelectorType< TextEncodingBaseTag, ::TextEncodingBase, kTextEncodingUnknown > TextEncodingBase;

   class TextEncodingVariantTag {};
   typedef Nucleus::SelectorType< TextEncodingVariantTag, ::TextEncodingVariant, kTextEncodingDefaultVariant > TextEncodingVariant;

   class TextEncodingFormatTag {};
   typedef Nucleus::SelectorType< TextEncodingFormatTag, ::TextEncodingFormat, 0 > TextEncodingFormat;

   class TextEncodingTag {};
   typedef Nucleus::IDType< TextEncodingTag, ::TextEncoding, 0 > TextEncoding;

   class TextEncodingNameSelectorTag {};
   typedef Nucleus::SelectorType< TextEncodingNameSelectorTag, ::TextEncodingNameSelector, 0 > TextEncodingNameSelector;

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
   class UCCharPropertyTypeTag {};
   typedef Nucleus::SelectorType< UCCharPropertyTypeTag, ::UCCharPropertyType, 0 > UCCharPropertyType;

   class UCCharPropertyValueTag {};
   typedef Nucleus::SelectorType< UCCharPropertyValueTag, ::UCCharPropertyValue, 0 > UCCharPropertyValue;
   
   // UCCharPropertyValue is really three types overloaded: UCCharGenlCategory, UCCharCombiningClass, and UCCharBidiCategory.
   
      class UCCharGenlCategoryTag {};
      typedef Nucleus::SelectorType< UCCharGenlCategoryTag, ::UCCharPropertyValue, 0 > UCCharGenlCategory;
      
      class UCCharCombiningClassTag {};
      typedef Nucleus::SelectorType< UCCharCombiningClassTag, ::UCCharPropertyValue, 0 > UCCharCombiningClass;
      
      class UCCharBidiCategoryTag {};
      typedef Nucleus::SelectorType< UCCharBidiCategoryTag, ::UCCharPropertyValue, 0 > UCCharBidiCategory;


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
                                                 LangCode         iTextLanguageID = TextLanguageDontCare(),
                                                 RegionCode       iRegionID       = TextRegionDontCare(),
                                                 ConstStr255Param iTextFontname   = 0 );

   inline TextEncoding UpgradeScriptInfoToTextEncoding( ScriptCode       iTextScriptID,
                                                        RegionCode       iRegionID,
                                                        ConstStr255Param iTextFontname = 0 )
     {
      return UpgradeScriptInfoToTextEncoding( iTextScriptID, TextLanguageDontCare(), iRegionID, iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( ScriptCode       iTextScriptID,
                                                        LangCode         iTextLanguageID,
                                                        ConstStr255Param iTextFontname )
     {
      return UpgradeScriptInfoToTextEncoding( iTextScriptID, iTextLanguageID, TextRegionDontCare(), iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( ScriptCode       iTextScriptID,
                                                        ConstStr255Param iTextFontname )
     {
      return UpgradeScriptInfoToTextEncoding( iTextScriptID, TextLanguageDontCare(), TextRegionDontCare(), iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( LangCode         iTextLanguageID,
                                                        RegionCode       iRegionID       = TextRegionDontCare(),
                                                        ConstStr255Param iTextFontname   = 0 )
     {
      return UpgradeScriptInfoToTextEncoding( TextScriptDontCare(), iTextLanguageID, iRegionID, iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( LangCode         iTextLanguageID,
                                                        ConstStr255Param iTextFontname )
     {
      return UpgradeScriptInfoToTextEncoding( TextScriptDontCare(), iTextLanguageID, TextRegionDontCare(), iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( RegionCode       iRegionID,
                                                        ConstStr255Param iTextFontname = 0 )
     {
      return UpgradeScriptInfoToTextEncoding( TextScriptDontCare(), TextLanguageDontCare(), iRegionID, iTextFontname );
     }

   inline TextEncoding UpgradeScriptInfoToTextEncoding( ConstStr255Param iTextFontname )
     {
      return UpgradeScriptInfoToTextEncoding( TextScriptDontCare(), TextLanguageDontCare(), TextRegionDontCare(), iTextFontname );
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
