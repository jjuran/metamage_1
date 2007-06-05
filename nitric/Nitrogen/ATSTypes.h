// Nitrogen/ATSTypes.h
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_ATSTYPES_H
#define NITROGEN_ATSTYPES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __ATSTYPES__
#include FRAMEWORK_HEADER(ATS,ATSTypes.h)
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_MIXEDMODE_H
#include "Nitrogen/MixedMode.h"
#endif
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif

namespace Nitrogen
  {
   typedef Nucleus::ID< class FMFontFamily_Tag, ::FMFontFamily >::Type FMFontFamily;
   typedef FMFontFamily FontID;
    
   typedef Nucleus::ID< class FMFontStyle_Tag, ::FMFontStyle >::Type FMFontStyle;
  }

#endif
