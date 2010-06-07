// Nitrogen/CFPreferences.hh
// -------------------------

// Part of the Nitrogen project.
//
// Written 2003-2004 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_CFPREFERENCES_HH
#define NITROGEN_CFPREFERENCES_HH

#ifndef __CFPREFERENCES__
#include <CFPreferences.h>
#endif
#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif
#ifndef NITROGEN_CFARRAY_HH
#include "Nitrogen/CFArray.hh"
#endif
#ifndef NITROGEN_CFPROPERTYLIST_HH
#include "Nitrogen/CFPropertyList.hh"
#endif


namespace Nitrogen
  {
   class CFPreferencesCopyAppValue_Failed {};
   nucleus::owned< CFPropertyListRef > CFPreferencesCopyAppValue( CFStringRef key,
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
   nucleus::owned< CFPropertyListRef > CFPreferencesCopyValue( CFStringRef key,
                                                               CFStringRef applicationID = kCFPreferencesCurrentApplication,
                                                               CFStringRef userName      = kCFPreferencesCurrentUser,
                                                               CFStringRef hostName      = kCFPreferencesCurrentHost );

   class CFPreferencesCopyMultiple_Failed {};
   nucleus::owned< CFDictionaryRef > CFPreferencesCopyMultiple( CFArrayRef  keysToFetch   = 0,
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
   nucleus::owned< CFArrayRef > CFPreferencesCopyApplicationList( CFStringRef userName = kCFPreferencesCurrentUser,
                                                                  CFStringRef hostName = kCFPreferencesCurrentHost );
   
   class CFPreferencesCopyKeyList_Failed {};
   nucleus::owned< CFArrayRef > CFPreferencesCopyKeyList( CFStringRef applicationID = kCFPreferencesCurrentApplication,
                                                          CFStringRef userName      = kCFPreferencesCurrentUser,
                                                          CFStringRef hostName      = kCFPreferencesCurrentHost );
  }

#endif
