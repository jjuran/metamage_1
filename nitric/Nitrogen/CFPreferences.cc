// Nitrogen/CFPreferences.cc
// -------------------------

// Part of the Nitrogen project.
//
// Written 2003 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/CFPreferences.hh"


namespace Nitrogen
  {
   nucleus::owned< CFPropertyListRef > CFPreferencesCopyAppValue( CFStringRef key,
                                                         CFStringRef applicationID )
     {
      ::CFPropertyListRef result = ::CFPreferencesCopyAppValue( key, applicationID );
      if ( result == CFPropertyListRef() )
         throw CFPreferencesCopyAppValue_Failed();
      return nucleus::owned<CFPropertyListRef>::seize( result );
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

   nucleus::owned< CFPropertyListRef > CFPreferencesCopyValue( CFStringRef key,
                                                      CFStringRef applicationID,
                                                      CFStringRef userName,
                                                      CFStringRef hostName )
     {
      CFPropertyListRef result = ::CFPreferencesCopyValue( key, applicationID, userName, hostName );
      if ( result == CFPropertyListRef() )
         throw CFPreferencesCopyValue_Failed();
      return nucleus::owned<CFPropertyListRef>::seize( result );
     }

   nucleus::owned< CFDictionaryRef > CFPreferencesCopyMultiple( CFArrayRef  keysToFetch,
                                                       CFStringRef applicationID,
                                                       CFStringRef userName,
                                                       CFStringRef hostName )
     {
      CFDictionaryRef result = ::CFPreferencesCopyMultiple( keysToFetch, applicationID, userName, hostName );
      if ( result == 0 )
         throw CFPreferencesCopyMultiple_Failed();
      return nucleus::owned<CFDictionaryRef>::seize( result );
     }

   nucleus::owned< CFArrayRef > CFPreferencesCopyApplicationList( CFStringRef userName,
                                                         CFStringRef hostName )
     {
      CFArrayRef result = ::CFPreferencesCopyApplicationList( userName, hostName );
      if ( result == 0 )
         throw CFPreferencesCopyApplicationList_Failed();
      return nucleus::owned<CFArrayRef>::seize( result );
     }
   
   nucleus::owned< CFArrayRef > CFPreferencesCopyKeyList( CFStringRef applicationID,
                                                 CFStringRef userName,
                                                 CFStringRef hostName )
     {
      CFArrayRef result = ::CFPreferencesCopyKeyList( applicationID, userName, hostName );
      if ( result == 0 )
         throw CFPreferencesCopyKeyList_Failed();
      return nucleus::owned<CFArrayRef>::seize( result );
     }
  }

