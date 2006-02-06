// ATSTypes.h

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
#ifndef NUCLEUS_IDTYPE_H
#include "Nucleus/IDType.h"
#endif

namespace Nitrogen
  {
   class FMFontFamilyTag {};
   typedef Nucleus::IDType< FMFontFamilyTag, ::FMFontFamily, 0 > FMFontFamily;
   typedef FMFontFamily FontID;
    
   class FMFontStyleTag {};
   typedef Nucleus::IDType< FMFontStyleTag, ::FMFontStyle, 0 > FMFontStyle;
  }

#endif
