// Nitrogen/Fonts.hh
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_FONTS_HH
#define NITROGEN_FONTS_HH

#ifndef __FONTS__
#include <Fonts.h>
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_ATSTYPES_HH
#include "Nitrogen/ATSTypes.hh"
#endif
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif
#ifndef NITROGEN_QUICKDRAW_HH
#include "Nitrogen/Quickdraw.hh"
#endif
#ifndef NITROGEN_TEXTCOMMON_HH
#include "Nitrogen/TextCommon.hh"
#endif


namespace Nitrogen
  {
	
	FontID GetFNum( ConstStr255Param name );
	FontID GetFNum( const std::string& name );
	
  }

#endif

