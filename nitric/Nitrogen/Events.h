// Events.h

#ifndef NITROGEN_EVENTS_H
#define NITROGEN_EVENTS_H

#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef NITROGEN_OSUTILS_H
#include "Nitrogen/OSUtils.h"
#endif
#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif

namespace Nitrogen
  {
   class EventModifiersTag {};
   typedef FlagType< EventModifiersTag, ::EventModifiers, 0 > EventModifiers;
  }

#endif
