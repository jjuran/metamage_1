// FrameworkHeader.h

#ifndef NITROGEN_FRAMEWORKHEADER_H
#define NITROGEN_FRAMEWORKHEADER_H

#ifndef NITROGEN_USE_FRAMEWORK_INCLUDES
   #ifdef __CF_USE_FRAMEWORK_INCLUDES__
      #define NITROGEN_USE_FRAMEWORK_INCLUDES    1
   #elif __MACH__
      #define NITROGEN_USE_FRAMEWORK_INCLUDES    1
   #else
      #define NITROGEN_USE_FRAMEWORK_INCLUDES    0
   #endif
#endif

#if NITROGEN_USE_FRAMEWORK_INCLUDES
   #define FRAMEWORK_HEADER(framework,file)             <framework/file>
#else
   #define FRAMEWORK_HEADER(framework,file)             <file>
#endif

#endif
