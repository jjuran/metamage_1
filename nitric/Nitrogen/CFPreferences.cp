// CFPreferences.cp

#ifndef NITROGEN_CFPREFERENCES_H
#include "Nitrogen/CFPreferences.h"
#endif

namespace Nitrogen
  {
   Owned< CFPropertyListRef > CFPreferencesCopyAppValue( CFStringRef key,
                                                         CFStringRef applicationID )
     {
      ::CFPropertyListRef result = ::CFPreferencesCopyAppValue( key, applicationID );
      if ( result == CFPropertyListRef() )
         throw CFPreferencesCopyAppValue_Failed();
      return Owned<CFPropertyListRef>::Seize( result );
     }
   
   bool CFPreferencesGetAppBooleanValue( CFStringRef key,
                                         CFStringRef applicationID )
     {
      ::Boolean keyExistsAndHasValidFormat;
      bool result = ::CFPreferencesGetAppBooleanValue( key, applicationID, &keyExistsAndHasValidFormat );
      if ( !keyExistsAndHasValidFormat )
         throw  CFPreferencesGetAppBooleanValue_Failed();
      return result;
     }

   CFIndex CFPreferencesGetAppIntegerValue( CFStringRef key,
                                            CFStringRef applicationID )
     {
      ::Boolean keyExistsAndHasValidFormat;
      CFIndex result = ::CFPreferencesGetAppIntegerValue( key, applicationID, &keyExistsAndHasValidFormat );
      if ( !keyExistsAndHasValidFormat )
         throw  CFPreferencesGetAppIntegerValue_Failed();
      return result;
     }
   
   void CFPreferencesAppSynchronize( CFStringRef applicationID )
     {
      if ( !::CFPreferencesAppSynchronize( applicationID ) )
         throw CFPreferencesAppSynchronize_Failed();
     }

   Owned< CFPropertyListRef > CFPreferencesCopyValue( CFStringRef key,
                                                      CFStringRef applicationID,
                                                      CFStringRef userName,
                                                      CFStringRef hostName )
     {
      CFPropertyListRef result = ::CFPreferencesCopyValue( key, applicationID, userName, hostName );
      if ( result == CFPropertyListRef() )
         throw CFPreferencesCopyValue_Failed();
      return Owned<CFPropertyListRef>::Seize( result );
     }

   Owned< CFDictionaryRef > CFPreferencesCopyMultiple( CFArrayRef  keysToFetch,
                                                       CFStringRef applicationID,
                                                       CFStringRef userName,
                                                       CFStringRef hostName )
     {
      CFDictionaryRef result = ::CFPreferencesCopyMultiple( keysToFetch, applicationID, userName, hostName );
      if ( result == 0 )
         throw CFPreferencesCopyMultiple_Failed();
      return Owned<CFDictionaryRef>::Seize( result );
     }

   Owned< CFArrayRef > CFPreferencesCopyApplicationList( CFStringRef userName,
                                                         CFStringRef hostName )
     {
      CFArrayRef result = ::CFPreferencesCopyApplicationList( userName, hostName );
      if ( result == 0 )
         throw CFPreferencesCopyApplicationList_Failed();
      return Owned<CFArrayRef>::Seize( result );
     }
   
   Owned< CFArrayRef > CFPreferencesCopyKeyList( CFStringRef applicationID,
                                                 CFStringRef userName,
                                                 CFStringRef hostName )
     {
      CFArrayRef result = ::CFPreferencesCopyKeyList( applicationID, userName, hostName );
      if ( result == 0 )
         throw CFPreferencesCopyKeyList_Failed();
      return Owned<CFArrayRef>::Seize( result );
     }
  }

