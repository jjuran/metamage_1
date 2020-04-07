// Nitrogen/ABAddressBookC.hh
// --------------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_ABADDRESSBOOKC_HH
#define NITROGEN_ABADDRESSBOOKC_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#include <AddressBook/ABAddressBookC.h>

#ifndef NITROGEN_CFARRAY_HH
#include "Nitrogen/CFArray.hh"
#endif

#ifndef NITROGEN_CFDATA_HH
#include "Nitrogen/CFData.hh"
#endif

#ifndef NITROGEN_MACERRORS_HH
#include "Nitrogen/MacErrors.hh"
#endif

/*	Marshall sez:
		There aren't really any errors to be returned here.

		Most of these things return booleans indicating success, and
		maybe I should catch them and turn them into exceptions.

		I could also check for NULL returns on Create and Copy, and 
		throw MemFullErr in those cases. I probably should.		
*/

namespace Nitrogen {

	class AddressBookErrorsRegistrationDependency
	{
		public:
			AddressBookErrorsRegistrationDependency();
	};
	
	
//	typedef void                            *ABRecordRef;
//	typedef struct __ABPerson               *ABPersonRef;
//	typedef struct __ABGroup                *ABGroupRef;
//	typedef struct __ABSearchElementRef     *ABSearchElementRef;
//	typedef struct __ABAddressBookRef       *ABAddressBookRef;
//	typedef const struct __ABMultiValue     *ABMultiValueRef;
//	typedef struct __ABMultiValue           *ABMutableMultiValueRef;

//	I gotta figure there's a way to do this with a type-trait thingy.
	using ::ABRecordRef;
	using ::ABPersonRef;
	using ::ABSearchElementRef;
	using ::ABGroupRef;
	using ::ABMultiValueRef;
	using ::ABMutableMultiValueRef;
  }

namespace nucleus
  {
   template <> struct disposer< ABRecordRef >
     {
      typedef ABRecordRef  argument_type;
      typedef void         result_type;
      
      void operator()( ABRecordRef rec ) const
        {
         ::CFRelease ( rec );
        }
     };

   template <> struct disposer< ABPersonRef >
     {
      typedef ABPersonRef  argument_type;
      typedef void         result_type;
      
      void operator()( ABPersonRef pers ) const
        {
         ::CFRelease ( pers );
        }
     };

   template <> struct disposer< ABSearchElementRef >
     {
      typedef ABSearchElementRef  argument_type;
      typedef void                result_type;
      
      void operator()( ABSearchElementRef se ) const
        {
         ::CFRelease ( se );
        }
     };

   template <> struct disposer< ABGroupRef >
     {
      typedef ABGroupRef  argument_type;
      typedef void        result_type;
      
      void operator()( ABGroupRef gr ) const
        {
         ::CFRelease ( gr );
        }
     };

   template <> struct disposer< ABMultiValueRef >
     {
      typedef ABMultiValueRef  argument_type;
      typedef void             result_type;
      
      void operator()( ABMultiValueRef mv ) const
        {
         ::CFRelease ( mv );
        }
     };

   template <> struct disposer< ABMutableMultiValueRef >
     {
      typedef ABMutableMultiValueRef  argument_type;
      typedef void                    result_type;
      
      void operator()( ABMutableMultiValueRef mv ) const
        {
         ::CFRelease ( mv );
        }
     };
  }

namespace Nitrogen
  {

//	Get the address book - there is only one.
//	extern ABAddressBookRef ABGetSharedAddressBook ( void );
	using ::ABGetSharedAddressBook;
	
//	--- Searching
    inline nucleus::owned<CFArrayRef> ABCopyArrayOfMatchingRecords ( ABAddressBookRef addressBook, ABSearchElementRef search ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABCopyArrayOfMatchingRecords ( addressBook, search );
		return nucleus::owned<CFArrayRef>::seize( result );
    	}
    
// --- Saving
//	extern bool ABSave(ABAddressBookRef addressBook);
	using ::ABSave;
	
//	extern bool ABHasUnsavedChanges(ABAddressBookRef addressBook);
	using ::ABHasUnsavedChanges;
	
// --- Me
//	extern ABPersonRef ABGetMe(ABAddressBookRef addressBook); // Not retain???
//	Note that this can return NULL if the user has never specified a "me"
	using ::ABGetMe;
	
//	extern void ABSetMe(ABAddressBookRef addressBook, ABPersonRef moi);
	using ::ABSetMe;

#ifdef MAC_OS_X_VERSION_10_3

// Returns the record class Name for a particular uniqueId
	inline nucleus::owned<CFStringRef> ABCopyRecordTypeFromUniqueId ( ABAddressBookRef addressBook, CFStringRef uniqueId ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABCopyRecordTypeFromUniqueId ( addressBook, uniqueId );
		return nucleus::owned<CFStringRef>::seize( result );
    	}
    

#endif

// --- Properties
//	Property names must be unique for a record type
//	extern int ABAddPropertiesAndTypes ( ABAddressBookRef addressBook, CFStringRef recordType, CFDictionaryRef propertiesAnTypes );
	using ::ABAddPropertiesAndTypes;
	
//	extern int ABRemoveProperties ( ABAddressBookRef addressBook, CFStringRef recordType, CFArrayRef properties );
	using ::ABRemoveProperties;
	
	inline nucleus::owned<CFArrayRef> ABCopyArrayOfPropertiesForRecordType ( ABAddressBookRef addressBook, CFStringRef recordType ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABCopyArrayOfPropertiesForRecordType ( addressBook, recordType );
		return nucleus::owned<CFArrayRef>::seize( result );
    	}
    
//	extern ABPropertyType ABTypeOfProperty ( ABAddressBookRef addressBook, CFStringRef recordType, CFStringRef property );
	using ::ABTypeOfProperty;

// --- Records (Person, Group)
    inline nucleus::owned<ABRecordRef> ABCopyRecordForUniqueId ( ABAddressBookRef addressBook, CFStringRef uniqueId ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABRecordRef result = ::ABCopyRecordForUniqueId ( addressBook, uniqueId );
		return nucleus::owned<ABRecordRef>::seize( result );
    	}

//	extern bool ABAddRecord		( ABAddressBookRef addressBook, ABRecordRef record );
//	extern bool ABRemoveRecord  ( ABAddressBookRef addressBook, ABRecordRef record );
	using ::ABAddRecord;
	using ::ABRemoveRecord;


// --- People
	inline nucleus::owned<CFArrayRef> ABCopyArrayOfAllPeople ( ABAddressBookRef addressBook ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABCopyArrayOfAllPeople ( addressBook );
		return nucleus::owned<CFArrayRef>::seize( result );
    	}

// --- Groups
    inline nucleus::owned<CFArrayRef> ABCopyArrayOfAllGroups ( ABAddressBookRef addressBook ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABCopyArrayOfAllGroups ( addressBook );
		return nucleus::owned<CFArrayRef>::seize( result );
    	}

// --------------------------------------------------------------------------------
//      ABRecord
// --------------------------------------------------------------------------------
// --- Record Type
	inline nucleus::owned<CFStringRef> ABRecordCopyRecordType ( ABRecordRef record ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABRecordCopyRecordType ( record );
		return nucleus::owned<CFStringRef>::seize( result );
    	}

// --- Property value
	inline nucleus::owned<CFTypeRef> ABRecordCopyValue ( ABRecordRef record, CFStringRef property ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFTypeRef result = ::ABRecordCopyValue ( record, property );
		return nucleus::owned<CFTypeRef>::seize( result );
    	}

//	returns a CFDictionary for multi-value properties
//	extern bool ABRecordSetValue(ABRecordRef record, CFStringRef property, CFTypeRef value);
	using ::ABRecordSetValue;
	
//	takes a CFDictionary for multi-value properties
//	extern bool ABRecordRemoveValue(ABRecordRef record, CFStringRef property);
	using ::ABRecordRemoveValue;
	
// ---- Unique ID access convenience
	inline nucleus::owned<CFStringRef> ABRecordCopyUniqueId ( ABRecordRef record ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABRecordCopyUniqueId ( record );
		return nucleus::owned<CFStringRef>::seize( result );
    	}

// --------------------------------------------------------------------------------
//      ABPerson
// --------------------------------------------------------------------------------
	inline nucleus::owned<ABPersonRef> ABPersonCreate ( void ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABPersonRef result = ::ABPersonCreate ();
		return nucleus::owned<ABPersonRef>::seize( result );
		}
	
	inline nucleus::owned<ABPersonRef> ABPersonCreateWithVCardRepresentation ( CFDataRef vCard ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABPersonRef result = ::ABPersonCreateWithVCardRepresentation ( vCard );
		return nucleus::owned<ABPersonRef>::seize( result );
		}

	inline nucleus::owned<CFDataRef> ABPersonCopyVCardRepresentation ( ABPersonRef person ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFDataRef result = ::ABPersonCopyVCardRepresentation ( person );
		return nucleus::owned<CFDataRef>::seize( result );
		}

//	Groups this person belongs to
 	inline nucleus::owned<CFArrayRef> ABPersonCopyParentGroups ( ABPersonRef person ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABPersonCopyParentGroups ( person );
		return nucleus::owned<CFArrayRef>::seize( result );
		}

// --- Search elements
 	inline nucleus::owned<ABSearchElementRef> ABPersonCreateSearchElement ( CFStringRef property, CFStringRef label, CFStringRef key, CFTypeRef value, ABSearchComparison comparison ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABSearchElementRef result = ::ABPersonCreateSearchElement ( property, label, key, value, comparison );
		return nucleus::owned<ABSearchElementRef>::seize( result );
		}

// --------------------------------------------------------------------------------
//      ABGroups
// --------------------------------------------------------------------------------

 	inline nucleus::owned<ABGroupRef> ABGroupCreate ( void ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABGroupRef result = ::ABGroupCreate ();
		return nucleus::owned<ABGroupRef>::seize( result );
		}

// --- Dealing with Persons
 	inline nucleus::owned<CFArrayRef> ABGroupCopyArrayOfAllMembers ( ABGroupRef group ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABGroupCopyArrayOfAllMembers ( group );
		return nucleus::owned<CFArrayRef>::seize( result );
		}

//	extern bool ABGroupAddMember 	( ABGroupRef group, ABPersonRef personToAdd );
//	extern bool ABGroupRemoveMember ( ABGroupRef group, ABPersonRef personToRemove );
	using ::ABGroupAddMember;
	using ::ABGroupRemoveMember;


// --- Dealing with Groups
 	inline nucleus::owned<CFArrayRef> ABGroupCopyArrayOfAllSubgroups ( ABGroupRef group ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABGroupCopyArrayOfAllSubgroups ( group );
		return nucleus::owned<CFArrayRef>::seize( result );
		}

//	extern bool ABGroupAddGroup		( ABGroupRef group, ABGroupRef groupToAdd );
//	extern bool ABGroupRemoveGroup  ( ABGroupRef group, ABGroupRef groupToRemove );
	using ::ABGroupAddGroup;
	using ::ABGroupRemoveGroup;

// --- Dealong with Parents
 	inline nucleus::owned<CFArrayRef> ABGroupCopyParentGroups ( ABGroupRef group ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABGroupCopyParentGroups ( group );
		return nucleus::owned<CFArrayRef>::seize( result );
		}

// --- Distribution list
//	extern bool ABGroupSetDistributionIdentifier ( ABGroupRef group, ABPersonRef person, CFStringRef property, CFStringRef identifier );
	using ::ABGroupSetDistributionIdentifier;
	
 	inline nucleus::owned<CFStringRef> ABGroupCopyDistributionIdentifier ( ABGroupRef group, ABPersonRef person, CFStringRef property ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABGroupCopyDistributionIdentifier ( group, person, property );
		return nucleus::owned<CFStringRef>::seize( result );
		}

// --- Search elements
 	inline nucleus::owned<ABSearchElementRef> ABGroupCreateSearchElement ( CFStringRef property, CFStringRef label, CFStringRef key, CFTypeRef value, ABSearchComparison comparison ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABSearchElementRef result = ::ABGroupCreateSearchElement ( property, label, key, value, comparison );
		return nucleus::owned<ABSearchElementRef>::seize( result );
		}

// --------------------------------------------------------------------------------
//      ABSearchElement
// --------------------------------------------------------------------------------
 	inline nucleus::owned<ABSearchElementRef> ABSearchElementCreateWithConjunction ( ABSearchConjunction conjunction, CFArrayRef childrenSearchElement ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABSearchElementRef result = ::ABSearchElementCreateWithConjunction ( conjunction, childrenSearchElement );
		return nucleus::owned<ABSearchElementRef>::seize( result );
		}

//	extern bool ABSearchElementMatchesRecord ( ABSearchElementRef searchElement, ABRecordRef record );
	using ::ABSearchElementMatchesRecord;

// --------------------------------------------------------------------------------
//      ABMultiValue
// --------------------------------------------------------------------------------
 	inline nucleus::owned<ABMultiValueRef> ABMultiValueCreate ( void ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABMultiValueRef result = ::ABMultiValueCreate ();
		return nucleus::owned<ABMultiValueRef>::seize( result );
		}

//	extern unsigned ABMultiValueCount ( ABMultiValueRef multiValue );
//	unsigned? Sheesh!
	using ::ABMultiValueCount;

//	And if ABMultiValueCount returns an unsigned here, why is index an 'int' ????
	inline nucleus::owned<CFTypeRef> ABMultiValueCopyValueAtIndex ( ABMultiValueRef multiValue, int index ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFTypeRef result = ::ABMultiValueCopyValueAtIndex ( multiValue, index );
		return nucleus::owned<CFTypeRef>::seize( result );
		}

	inline nucleus::owned<CFStringRef> ABMultiValueCopyLabelAtIndex ( ABMultiValueRef multiValue, int index ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABMultiValueCopyLabelAtIndex ( multiValue, index );
		return nucleus::owned<CFStringRef>::seize( result );
		}

	inline nucleus::owned<CFStringRef> ABMultiValueCopyPrimaryIdentifier ( ABMultiValueRef multiValue ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABMultiValueCopyPrimaryIdentifier ( multiValue );
		return nucleus::owned<CFStringRef>::seize( result );
		}

//	extern int ABMultiValueIndexForIdentifier ( ABMultiValueRef multiValue, CFStringRef identifier );
	using ::ABMultiValueIndexForIdentifier;
	
	inline nucleus::owned<CFStringRef> ABMultiValueCopyIdentifierAtIndex ( ABMultiValueRef multiValue, int index ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABMultiValueCopyIdentifierAtIndex ( multiValue, index );
		return nucleus::owned<CFStringRef>::seize( result );
		}

//	extern ABPropertyType ABMultiValuePropertyType ( ABMultiValueRef multiValue );
	using ::ABMultiValuePropertyType;
	
 	inline nucleus::owned<ABMultiValueRef> ABMultiValueCreateCopy ( ABMultiValueRef multiValue ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABMultiValueRef result = ::ABMultiValueCreateCopy ( multiValue );
		return nucleus::owned<ABMultiValueRef>::seize( result );
		}

// --------------------------------------------------------------------------------
//      ABMutableMultiValue
// --------------------------------------------------------------------------------
 	inline nucleus::owned<ABMutableMultiValueRef> ABMultiValueCreateMutable ( void ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABMutableMultiValueRef result = ::ABMultiValueCreateMutable ();
		return nucleus::owned<ABMutableMultiValueRef>::seize( result );
		}

	inline nucleus::owned<CFStringRef> ABMultiValueAdd ( ABMutableMultiValueRef multiValue, CFTypeRef value, CFStringRef label ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result;
		if ( !::ABMultiValueAdd ( multiValue, value, label, &result ))
			throw MemFullErr ();
		return nucleus::owned<CFStringRef>::seize( result );
		}

	inline nucleus::owned<CFStringRef> ABMultiValueInsert ( ABMutableMultiValueRef multiValue, CFTypeRef value, CFStringRef label, int index ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result;
		if ( !::ABMultiValueInsert ( multiValue, value, label, index, &result ))
			throw MemFullErr ();
		return nucleus::owned<CFStringRef>::seize( result );
		}

//	extern bool ABMultiValueRemove ( ABMutableMultiValueRef multiValue, int index );
	using ::ABMultiValueRemove;
	
//	extern bool ABMultiValueReplaceValue ( ABMutableMultiValueRef multiValue, CFTypeRef value, int index );
//	extern bool ABMultiValueReplaceLabel ( ABMutableMultiValueRef multiValue, CFStringRef label, int index );
	using ::ABMultiValueReplaceValue;
	using ::ABMultiValueReplaceLabel;

//	extern bool ABMultiValueSetPrimaryIdentifier (ABMutableMultiValueRef multiValue, CFStringRef identifier );
	using ::ABMultiValueReplaceLabel;
	
	inline nucleus::owned<ABMutableMultiValueRef> ABMultiValueCreateMutableCopy ( ABMultiValueRef multiValue ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABMutableMultiValueRef result = ::ABMultiValueCreateMutableCopy ( multiValue );
		return nucleus::owned<ABMutableMultiValueRef>::seize( result );
		}

// --------------------------------------------------------------------------------
//      Localization of properties or labels
// --------------------------------------------------------------------------------
	inline nucleus::owned<CFStringRef> ABCopyLocalizedPropertyOrLabel ( CFStringRef labelOrProperty ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABCopyLocalizedPropertyOrLabel ( labelOrProperty );
		return nucleus::owned<CFStringRef>::seize( result );
		}

#ifdef MAC_OS_X_VERSION_10_3

// --- Address formatting
	inline nucleus::owned<CFStringRef> ABCreateFormattedAddressFromDictionary ( ABAddressBookRef addressBook, CFDictionaryRef address ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABCreateFormattedAddressFromDictionary ( addressBook, address );
		return nucleus::owned<CFStringRef>::seize( result );
		}

	inline nucleus::owned<CFStringRef> ABCopyDefaultCountryCode ( ABAddressBookRef addressBook ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABCopyDefaultCountryCode ( addressBook );
		return nucleus::owned<CFStringRef>::seize( result );
		}

#endif

// --------------------------------------------------------------------------------
//      Person Image Loading
// --------------------------------------------------------------------------------
//	extern bool ABPersonSetImageData ( ABPersonRef person, CFDataRef imageData );
	using ::ABPersonSetImageData;

	inline nucleus::owned<CFDataRef> ABPersonCopyImageData ( ABPersonRef person ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFDataRef result = ::ABPersonCopyImageData ( person );
		return nucleus::owned<CFDataRef>::seize( result );
		}

//	shouldn't this be 'pascal' ??
//	typedef void (*ABImageClientCallback) ( CFDataRef imageData, int tag, void* refcon );

//	What needs to be done here?
//	extern int ABBeginLoadingImageDataForClient ( ABPersonRef person, ABImageClientCallback callback, void* refcon );
	using ::ABBeginLoadingImageDataForClient;
	
//	extern void ABCancelLoadingImageDataForTag ( int tag );
	using ::ABCancelLoadingImageDataForTag;
	}
	
#endif
