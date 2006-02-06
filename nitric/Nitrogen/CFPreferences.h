// CFPreferences.h

#ifndef NITROGEN_CFPREFERENCES_H
#define NITROGEN_CFPREFERENCES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFPREFERENCES__
#include FRAMEWORK_HEADER(CoreFoundation,CFPreferences.h)
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif
#ifndef NITROGEN_CFARRAY_H
#include "Nitrogen/CFArray.h"
#endif
#ifndef NITROGEN_CFPROPERTYLIST_H
#include "Nitrogen/CFPropertyList.h"
#endif
#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif

namespace Nitrogen
  {
   class CFPreferencesCopyAppValue_Failed {};
   Nucleus::Owned< CFPropertyListRef > CFPreferencesCopyAppValue( CFStringRef key,
                                                         CFStringRef applicationID = kCFPreferencesCurrentApplication );
   
   class CFPreferencesGetAppBooleanValue_Failed {};
   bool CFPreferencesGetAppBooleanValue( CFStringRef key,
                                         CFStringRef applicationID = kCFPreferencesCurrentApplication );
   
   

   class CFPreferencesGetAppIntegerValue_Failed {};
   CFIndex CFPreferencesGetAppIntegerValue( CFStringRef key,
                                            CFStringRef applicationID = kCFPreferencesCurrentApplication );
   
   inline void CFPreferencesSetAppValue( CFStringRef       key,
                                         CFPropertyListRef value,
                                         CFStringRef       applicationID = kCFPreferencesCurrentApplication )
     {
      ::CFPreferencesSetAppValue( key, value, applicationID );
     }

   using ::CFPreferencesAddSuitePreferencesToApp;
   inline void CFPreferencesAddSuitePreferencesToApp( CFStringRef suiteID )
     {
      CFPreferencesAddSuitePreferencesToApp( kCFPreferencesCurrentApplication, suiteID );
     }
   
   using ::CFPreferencesRemoveSuitePreferencesFromApp;
   inline void CFPreferencesRemoveSuitePreferencesFromApp( CFStringRef suiteID )
     {
      CFPreferencesRemoveSuitePreferencesFromApp( kCFPreferencesCurrentApplication, suiteID );
     }
   
   class CFPreferencesAppSynchronize_Failed {};
   void CFPreferencesAppSynchronize( CFStringRef applicationID = kCFPreferencesCurrentApplication );

   class CFPreferencesCopyValue_Failed {};
   Nucleus::Owned< CFPropertyListRef > CFPreferencesCopyValue( CFStringRef key,
                                                      CFStringRef applicationID = kCFPreferencesCurrentApplication,
                                                      CFStringRef userName      = kCFPreferencesCurrentUser,
                                                      CFStringRef hostName      = kCFPreferencesCurrentHost );

   class CFPreferencesCopyMultiple_Failed {};
   Nucleus::Owned< CFDictionaryRef > CFPreferencesCopyMultiple( CFArrayRef  keysToFetch   = 0,
                                                       CFStringRef applicationID = kCFPreferencesCurrentApplication,
                                                       CFStringRef userName      = kCFPreferencesCurrentUser,
                                                       CFStringRef hostName      = kCFPreferencesCurrentHost );

   inline void CFPreferencesSetValue( CFStringRef       key,
                                      CFPropertyListRef value,
                                      CFStringRef       applicationID = kCFPreferencesCurrentApplication,
                                      CFStringRef       userName      = kCFPreferencesCurrentUser,
                                      CFStringRef       hostName      = kCFPreferencesCurrentHost )
     {
      ::CFPreferencesSetValue( key, value, applicationID, userName, hostName );
     }

   inline void CFPreferencesSetMultiple( CFDictionaryRef keysToSet,
                                         CFArrayRef      keysToRemove,
                                         CFStringRef     applicationID = kCFPreferencesCurrentApplication,
                                         CFStringRef     userName      = kCFPreferencesCurrentUser,
                                         CFStringRef     hostName      = kCFPreferencesCurrentHost )
     {
      ::CFPreferencesSetMultiple( keysToSet, keysToRemove, applicationID, userName, hostName );
     }

   inline void CFPreferencesSynchronize( CFStringRef applicationID = kCFPreferencesCurrentApplication,
                                         CFStringRef userName      = kCFPreferencesCurrentUser,
                                         CFStringRef hostName      = kCFPreferencesCurrentHost )
     {
      ::CFPreferencesSynchronize( applicationID, userName, hostName );
     }
   
   class CFPreferencesCopyApplicationList_Failed {};
   Nucleus::Owned< CFArrayRef > CFPreferencesCopyApplicationList( CFStringRef userName = kCFPreferencesCurrentUser,
                                                         CFStringRef hostName = kCFPreferencesCurrentHost );
   
   class CFPreferencesCopyKeyList_Failed {};
   Nucleus::Owned< CFArrayRef > CFPreferencesCopyKeyList( CFStringRef applicationID = kCFPreferencesCurrentApplication,
                                                 CFStringRef userName      = kCFPreferencesCurrentUser,
                                                 CFStringRef hostName      = kCFPreferencesCurrentHost );
  }

#endif
