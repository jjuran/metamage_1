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


#ifndef NITROGEN_ADDRESSBOOKC_HH
#define NITROGEN_ADDRESSBOOKC_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#include <AddressBook/ABAddressBookC.h>

#ifndef NITROGEN_CFARRAY_HH
#include "Nitrogen/CFArray.hh"
#endif

#ifndef NITROGEN_CFSTRING_HH
#include "Nitrogen/CFString.hh"
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

namespace Nucleus
  {
   template <> struct Disposer< Nitrogen::ABRecordRef >: public std::unary_function< Nitrogen::ABRecordRef, void >
     {
      void operator()( Nitrogen::ABRecordRef rec ) const
        {
         ::CFRelease ( rec );
        }
     };

   template <> struct Disposer< Nitrogen::ABPersonRef >: public std::unary_function< Nitrogen::ABPersonRef, void >
     {
      void operator()( Nitrogen::ABPersonRef pers ) const
        {
         ::CFRelease ( pers );
        }
     };

   template <> struct Disposer< Nitrogen::ABSearchElementRef >: public std::unary_function< Nitrogen::ABSearchElementRef, void >
     {
      void operator()( Nitrogen::ABSearchElementRef se ) const
        {
         ::CFRelease ( se );
        }
     };

   template <> struct Disposer< Nitrogen::ABGroupRef >: public std::unary_function< Nitrogen::ABGroupRef, void >
     {
      void operator()( Nitrogen::ABGroupRef gr ) const
        {
         ::CFRelease ( gr );
        }
     };

   template <> struct Disposer< Nitrogen::ABMultiValueRef >: public std::unary_function< Nitrogen::ABMultiValueRef, void >
     {
      void operator()( Nitrogen::ABMultiValueRef mv ) const
        {
         ::CFRelease ( mv );
        }
     };

   template <> struct Disposer< Nitrogen::ABMutableMultiValueRef >: public std::unary_function< Nitrogen::ABMutableMultiValueRef, void >
     {
      void operator()( Nitrogen::ABMutableMultiValueRef mv ) const
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
    inline Nucleus::Owned<CFArrayRef> ABCopyArrayOfMatchingRecords ( ABAddressBookRef addressBook, ABSearchElementRef search ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABCopyArrayOfMatchingRecords ( addressBook, search );
		return Nucleus::Owned<CFArrayRef>::Seize( result );
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

// Returns the record class Name for a particular uniqueId
	inline Nucleus::Owned<CFStringRef> ABCopyRecordTypeFromUniqueId ( ABAddressBookRef addressBook, CFStringRef uniqueId ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABCopyRecordTypeFromUniqueId ( addressBook, uniqueId );
		return Nucleus::Owned<CFStringRef>::Seize( result );
    	}
    

// --- Properties
//	Property names must be unique for a record type
//	extern int ABAddPropertiesAndTypes ( ABAddressBookRef addressBook, CFStringRef recordType, CFDictionaryRef propertiesAnTypes );
	using ::ABAddPropertiesAndTypes;
	
//	extern int ABRemoveProperties ( ABAddressBookRef addressBook, CFStringRef recordType, CFArrayRef properties );
	using ::ABRemoveProperties;
	
	inline Nucleus::Owned<CFArrayRef> ABCopyArrayOfPropertiesForRecordType ( ABAddressBookRef addressBook, CFStringRef recordType ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABCopyArrayOfPropertiesForRecordType ( addressBook, recordType );
		return Nucleus::Owned<CFArrayRef>::Seize( result );
    	}
    
//	extern ABPropertyType ABTypeOfProperty ( ABAddressBookRef addressBook, CFStringRef recordType, CFStringRef property );
	using ::ABTypeOfProperty;

// --- Records (Person, Group)
    inline Nucleus::Owned<ABRecordRef> ABCopyRecordForUniqueId ( ABAddressBookRef addressBook, CFStringRef uniqueId ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABRecordRef result = ::ABCopyRecordForUniqueId ( addressBook, uniqueId );
		return Nucleus::Owned<ABRecordRef>::Seize( result );
    	}

//	extern bool ABAddRecord		( ABAddressBookRef addressBook, ABRecordRef record );
//	extern bool ABRemoveRecord  ( ABAddressBookRef addressBook, ABRecordRef record );
	using ::ABAddRecord;
	using ::ABRemoveRecord;


// --- People
	inline Nucleus::Owned<CFArrayRef> ABCopyArrayOfAllPeople ( ABAddressBookRef addressBook ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABCopyArrayOfAllPeople ( addressBook );
		return Nucleus::Owned<CFArrayRef>::Seize( result );
    	}

// --- Groups
    inline Nucleus::Owned<CFArrayRef> ABCopyArrayOfAllGroups ( ABAddressBookRef addressBook ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABCopyArrayOfAllGroups ( addressBook );
		return Nucleus::Owned<CFArrayRef>::Seize( result );
    	}

// --------------------------------------------------------------------------------
//      ABRecord
// --------------------------------------------------------------------------------
// --- Record Type
	inline Nucleus::Owned<CFStringRef> ABRecordCopyRecordType ( ABRecordRef record ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABRecordCopyRecordType ( record );
		return Nucleus::Owned<CFStringRef>::Seize( result );
    	}

// --- Property value
	inline Nucleus::Owned<CFTypeRef> ABRecordCopyValue ( ABRecordRef record, CFStringRef property ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFTypeRef result = ::ABRecordCopyValue ( record, property );
		return Nucleus::Owned<CFTypeRef>::Seize( result );
    	}

//	returns a CFDictionary for multi-value properties
//	extern bool ABRecordSetValue(ABRecordRef record, CFStringRef property, CFTypeRef value);
	using ::ABRecordSetValue;
	
//	takes a CFDictionary for multi-value properties
//	extern bool ABRecordRemoveValue(ABRecordRef record, CFStringRef property);
	using ::ABRecordRemoveValue;
	
// ---- Unique ID access convenience
	inline Nucleus::Owned<CFStringRef> ABRecordCopyUniqueId ( ABRecordRef record ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABRecordCopyUniqueId ( record );
		return Nucleus::Owned<CFStringRef>::Seize( result );
    	}

// --------------------------------------------------------------------------------
//      ABPerson
// --------------------------------------------------------------------------------
	inline Nucleus::Owned<ABPersonRef> ABPersonCreate ( void ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABPersonRef result = ::ABPersonCreate ();
		return Nucleus::Owned<ABPersonRef>::Seize( result );
		}
	
	inline Nucleus::Owned<ABPersonRef> ABPersonCreateWithVCardRepresentation ( CFDataRef vCard ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABPersonRef result = ::ABPersonCreateWithVCardRepresentation ( vCard );
		return Nucleus::Owned<ABPersonRef>::Seize( result );
		}

	inline Nucleus::Owned<CFDataRef> ABPersonCopyVCardRepresentation ( ABPersonRef person ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFDataRef result = ::ABPersonCopyVCardRepresentation ( person );
		return Nucleus::Owned<CFDataRef>::Seize( result );
		}

//	Groups this person belongs to
 	inline Nucleus::Owned<CFArrayRef> ABPersonCopyParentGroups ( ABPersonRef person ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABPersonCopyParentGroups ( person );
		return Nucleus::Owned<CFArrayRef>::Seize( result );
		}

// --- Search elements
 	inline Nucleus::Owned<ABSearchElementRef> ABPersonCreateSearchElement ( CFStringRef property, CFStringRef label, CFStringRef key, CFTypeRef value, ABSearchComparison comparison ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABSearchElementRef result = ::ABPersonCreateSearchElement ( property, label, key, value, comparison );
		return Nucleus::Owned<ABSearchElementRef>::Seize( result );
		}

// --------------------------------------------------------------------------------
//      ABGroups
// --------------------------------------------------------------------------------

 	inline Nucleus::Owned<ABGroupRef> ABGroupCreate ( void ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABGroupRef result = ::ABGroupCreate ();
		return Nucleus::Owned<ABGroupRef>::Seize( result );
		}

// --- Dealing with Persons
 	inline Nucleus::Owned<CFArrayRef> ABGroupCopyArrayOfAllMembers ( ABGroupRef group ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABGroupCopyArrayOfAllMembers ( group );
		return Nucleus::Owned<CFArrayRef>::Seize( result );
		}

//	extern bool ABGroupAddMember 	( ABGroupRef group, ABPersonRef personToAdd );
//	extern bool ABGroupRemoveMember ( ABGroupRef group, ABPersonRef personToRemove );
	using ::ABGroupAddMember;
	using ::ABGroupRemoveMember;


// --- Dealing with Groups
 	inline Nucleus::Owned<CFArrayRef> ABGroupCopyArrayOfAllSubgroups ( ABGroupRef group ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABGroupCopyArrayOfAllSubgroups ( group );
		return Nucleus::Owned<CFArrayRef>::Seize( result );
		}

//	extern bool ABGroupAddGroup		( ABGroupRef group, ABGroupRef groupToAdd );
//	extern bool ABGroupRemoveGroup  ( ABGroupRef group, ABGroupRef groupToRemove );
	using ::ABGroupAddGroup;
	using ::ABGroupRemoveGroup;

// --- Dealong with Parents
 	inline Nucleus::Owned<CFArrayRef> ABGroupCopyParentGroups ( ABGroupRef group ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFArrayRef result = ::ABGroupCopyParentGroups ( group );
		return Nucleus::Owned<CFArrayRef>::Seize( result );
		}

// --- Distribution list
//	extern bool ABGroupSetDistributionIdentifier ( ABGroupRef group, ABPersonRef person, CFStringRef property, CFStringRef identifier );
	using ::ABGroupSetDistributionIdentifier;
	
 	inline Nucleus::Owned<CFStringRef> ABGroupCopyDistributionIdentifier ( ABGroupRef group, ABPersonRef person, CFStringRef property ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABGroupCopyDistributionIdentifier ( group, person, property );
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}

// --- Search elements
 	inline Nucleus::Owned<ABSearchElementRef> ABGroupCreateSearchElement ( CFStringRef property, CFStringRef label, CFStringRef key, CFTypeRef value, ABSearchComparison comparison ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABSearchElementRef result = ::ABGroupCreateSearchElement ( property, label, key, value, comparison );
		return Nucleus::Owned<ABSearchElementRef>::Seize( result );
		}

// --------------------------------------------------------------------------------
//      ABSearchElement
// --------------------------------------------------------------------------------
 	inline Nucleus::Owned<ABSearchElementRef> ABSearchElementCreateWithConjunction ( ABSearchConjunction conjunction, CFArrayRef childrenSearchElement ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABSearchElementRef result = ::ABSearchElementCreateWithConjunction ( conjunction, childrenSearchElement );
		return Nucleus::Owned<ABSearchElementRef>::Seize( result );
		}

//	extern bool ABSearchElementMatchesRecord ( ABSearchElementRef searchElement, ABRecordRef record );
	using ::ABSearchElementMatchesRecord;

// --------------------------------------------------------------------------------
//      ABMultiValue
// --------------------------------------------------------------------------------
 	inline Nucleus::Owned<ABMultiValueRef> ABMultiValueCreate ( void ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABMultiValueRef result = ::ABMultiValueCreate ();
		return Nucleus::Owned<ABMultiValueRef>::Seize( result );
		}

//	extern unsigned ABMultiValueCount ( ABMultiValueRef multiValue );
//	unsigned? Sheesh!
	using ::ABMultiValueCount;

//	And if ABMultiValueCount returns an unsigned here, why is index an 'int' ????
	inline Nucleus::Owned<CFTypeRef> ABMultiValueCopyValueAtIndex ( ABMultiValueRef multiValue, int index ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFTypeRef result = ::ABMultiValueCopyValueAtIndex ( multiValue, index );
		return Nucleus::Owned<CFTypeRef>::Seize( result );
		}

	inline Nucleus::Owned<CFStringRef> ABMultiValueCopyLabelAtIndex ( ABMultiValueRef multiValue, int index ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABMultiValueCopyLabelAtIndex ( multiValue, index );
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}

	inline Nucleus::Owned<CFStringRef> ABMultiValueCopyPrimaryIdentifier ( ABMultiValueRef multiValue ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABMultiValueCopyPrimaryIdentifier ( multiValue );
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}

//	extern int ABMultiValueIndexForIdentifier ( ABMultiValueRef multiValue, CFStringRef identifier );
	using ::ABMultiValueIndexForIdentifier;
	
	inline Nucleus::Owned<CFStringRef> ABMultiValueCopyIdentifierAtIndex ( ABMultiValueRef multiValue, int index ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABMultiValueCopyIdentifierAtIndex ( multiValue, index );
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}

//	extern ABPropertyType ABMultiValuePropertyType ( ABMultiValueRef multiValue );
	using ::ABMultiValuePropertyType;
	
 	inline Nucleus::Owned<ABMultiValueRef> ABMultiValueCreateCopy ( ABMultiValueRef multiValue ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABMultiValueRef result = ::ABMultiValueCreateCopy ( multiValue );
		return Nucleus::Owned<ABMultiValueRef>::Seize( result );
		}

// --------------------------------------------------------------------------------
//      ABMutableMultiValue
// --------------------------------------------------------------------------------
 	inline Nucleus::Owned<ABMutableMultiValueRef> ABMultiValueCreateMutable ( void ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABMutableMultiValueRef result = ::ABMultiValueCreateMutable ();
		return Nucleus::Owned<ABMutableMultiValueRef>::Seize( result );
		}

	inline Nucleus::Owned<CFStringRef> ABMultiValueAdd ( ABMutableMultiValueRef multiValue, CFTypeRef value, CFStringRef label ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result;
		if ( !::ABMultiValueAdd ( multiValue, value, label, &result ))
			throw MemFullErr ();
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}

	inline Nucleus::Owned<CFStringRef> ABMultiValueInsert ( ABMutableMultiValueRef multiValue, CFTypeRef value, CFStringRef label, int index ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result;
		if ( !::ABMultiValueInsert ( multiValue, value, label, index, &result ))
			throw MemFullErr ();
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}

//	extern bool ABMultiValueRemove ( ABMutableMultiValueRef multiValue, int index );
	using ::ABMultiValueRemove;
	
//	extern bool ABMultiValueReplaceValue ( ABMutableMultiValueRef multiValue, CFTypeRef value, int index );
//	extern bool ABMultiValueReplaceLabel ( ABMutableMultiValueRef multiValue, CFStringRef label, int index );
	using ::ABMultiValueReplaceValue;
	using ::ABMultiValueReplaceLabel;

//	extern bool ABMultiValueSetPrimaryIdentifier (ABMutableMultiValueRef multiValue, CFStringRef identifier );
	using ::ABMultiValueReplaceLabel;
	
	inline Nucleus::Owned<ABMutableMultiValueRef> ABMultiValueCreateMutableCopy ( ABMultiValueRef multiValue ) {
		(void) AddressBookErrorsRegistrationDependency();
		ABMutableMultiValueRef result = ::ABMultiValueCreateMutableCopy ( multiValue );
		return Nucleus::Owned<ABMutableMultiValueRef>::Seize( result );
		}

// --------------------------------------------------------------------------------
//      Localization of properties or labels
// --------------------------------------------------------------------------------
	inline Nucleus::Owned<CFStringRef> ABCopyLocalizedPropertyOrLabel ( CFStringRef labelOrProperty ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABCopyLocalizedPropertyOrLabel ( labelOrProperty );
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}

// --- Address formatting
	inline Nucleus::Owned<CFStringRef> ABCreateFormattedAddressFromDictionary ( ABAddressBookRef addressBook, CFDictionaryRef address ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABCreateFormattedAddressFromDictionary ( addressBook, address );
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}

	inline Nucleus::Owned<CFStringRef> ABCopyDefaultCountryCode ( ABAddressBookRef addressBook ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFStringRef result = ::ABCopyDefaultCountryCode ( addressBook );
		return Nucleus::Owned<CFStringRef>::Seize( result );
		}

// --------------------------------------------------------------------------------
//      Person Image Loading
// --------------------------------------------------------------------------------
//	extern bool ABPersonSetImageData ( ABPersonRef person, CFDataRef imageData );
	using ::ABPersonSetImageData;

	inline Nucleus::Owned<CFDataRef> ABPersonCopyImageData ( ABPersonRef person ) {
		(void) AddressBookErrorsRegistrationDependency();
		CFDataRef result = ::ABPersonCopyImageData ( person );
		return Nucleus::Owned<CFDataRef>::Seize( result );
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
