// Nitrogen/MacTypes.hh
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_MACTYPES_HH
#define NITROGEN_MACTYPES_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif
#ifndef __SCRIPT__
#include <Script.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_MAKE_HH
#include "nucleus/make.hh"
#endif
#ifndef NUCLEUS_SCRIBE_HH
#include "nucleus/scribe.hh"
#endif

// Nitrogen
#ifndef MAC_RESOURCES_TYPES_RESTYPE_HH
#include "Mac/Resources/Types/ResType.hh"
#endif
#ifndef MAC_SCRIPT_TYPES_LANGCODE_HH
#include "Mac/Script/Types/LangCode.hh"
#endif
#ifndef MAC_SCRIPT_TYPES_REGIONCODE_HH
#include "Mac/Script/Types/RegionCode.hh"
#endif
#ifndef MAC_SCRIPT_TYPES_SCRIPTCODE_HH
#include "Mac/Script/Types/ScriptCode.hh"
#endif
#ifndef MAC_TOOLBOX_TYPES_OSTYPE_HH
#include "Mac/Toolbox/Types/OSType.hh"
#endif

#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif


namespace Nitrogen
  {
	
   typedef ::std::size_t  Size;
	
	using Mac::ScriptCode;
	using Mac::smSystemScript;
	
	using Mac::LangCode;
	using Mac::langUnspecified;
	
	using Mac::RegionCode;
	using Mac::verUS;
	
	using Mac::OSType;
	using Mac::kUnknownType;
	
	using Mac::ResType;
	using Mac::kVersionResType;
	
	typedef bool Boolean;
	
  }

namespace nucleus
  {
   template <>
   struct maker< Point >
     {
      Point operator()( short v, short h ) const
        {
         Point result;
         result.v = v;
         result.h = h;
         return result;
        }
      
      Point operator()() const
        {
         return operator()( 0, 0 );
        }
     };

   template <>
   struct maker< Rect >
     {
      Rect operator()( short top, short left, short bottom, short right ) const
        {
         Rect result;
         result.top = top;
         result.left = left;
         result.bottom = bottom;
         result.right = right;
         return result;
        }
      
      Rect operator()() const
        {
         return operator()( 0, 0, 0, 0 );
        }
     };
  }

#endif
