//  ABAddressBookC.h

#ifndef NITROGEN_ADDRESSBOOKC_H
#define NITROGEN_ADDRESSBOOKC_H

#if	!TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#include <AddressBook/ABAddressBookC.h>

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

#ifndef NITROGEN_CFARRAY_H
#include "Nitrogen/CFArray.h"
#endif

#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif

/*	Marshall sez:
		There aren't really any errors to be returned here.

		Most of these things return booleans indicating success, and
		maybe I should catch them and turn them into exceptions.

		I could also check for NULL returns on Create and Copy, and 
		throw MemFullErr in those cases. I probably should.		
*/

namespace Nitrogen {

	void RegisterAddressBookErrors ( void );
	
//	typedef void                            *ABRecordRef;
//	typedef struct __ABPerson               *ABPersonRef;
//	typedef struct __ABGroup                *ABGroupRef;
//	typedef struct __ABSearchElementRef     *ABSearchElementRef;
//	typedef struct __ABAddressBookRef       *ABAddressBookRef;
//	typedef const struct __ABMultiValue     *ABMultiValueRef;
//	typedef struct __ABMultiValue           *ABMutableMultiValueRef;

//	I gotta figure there's a way to do this with a type-trait thingy.
	using ::ABRecordRef;
   template <> struct Disposer< ABRecordRef >: public std::unary_function< ABRecordRef, void >
     {
      void operator()( ABRecordRef rec ) const
        {
         ::CFRelease ( rec );
        }
     };

	using ::ABPersonRef;
   template <> struct Disposer< ABPersonRef >: public std::unary_function< ABPersonRef, void >\
     {
      void operator()( ABPersonRef pers ) const
        {
         ::CFRelease ( pers );
        }
     };

	using ::ABSearchElementRef;
   template <> struct Disposer< ABSearchElementRef >: public std::unary_function< ABSearchElementRef, void >
     {
      void operator()( ABSearchElementRef se ) const
        {
         ::CFRelease ( se );
        }
     };

	using ::ABGroupRef;
   template <> struct Disposer< ABGroupRef >: public std::unary_function< ABGroupRef, void >
     {
      void operator()( ABGroupRef gr ) const
        {
         ::CFRelease ( gr );
        }
     };

	using ::ABMultiValueRef;
   template <> struct Disposer< ABMultiValueRef >: public std::unary_function< ABMultiValueRef, void >
     {
      void operator()( ABMultiValueRef mv ) const
        {
         ::CFRelease ( mv );
        }
     };

	using ::ABMutableMultiValueRef;
   template <> struct Disposer< ABMutableMultiValueRef >: public std::unary_function< ABMutableMultiValueRef, void >
     {
      void operator()( ABMutableMultiValueRef mv ) const
        {
         ::CFRelease ( mv );
        }
     };

//	Get the address book - there is only one.
//	extern ABAddressBookRef ABGetSharedAddressBook ( void );
	using ::ABGetSharedAddressBook;
	
//	--- Searching
    inline Owned<CFArrayRef> ABCopyArrayOfMatchingRecords ( ABAddressBookRef addressBook, ABSearchElementRef search ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFArrayRef result = ::ABCopyArrayOfMatchingRecords ( addressBook, search );
		return Owned<CFArrayRef>::Seize( result );
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
	inline Owned<CFStringRef> ABCopyRecordTypeFromUniqueId ( ABAddressBookRef addressBook, CFStringRef uniqueId ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result = ::ABCopyRecordTypeFromUniqueId ( addressBook, uniqueId );
		return Owned<CFStringRef>::Seize( result );
    	}
    

// --- Properties
//	Property names must be unique for a record type
//	extern int ABAddPropertiesAndTypes ( ABAddressBookRef addressBook, CFStringRef recordType, CFDictionaryRef propertiesAnTypes );
	using ::ABAddPropertiesAndTypes;
	
//	extern int ABRemoveProperties ( ABAddressBookRef addressBook, CFStringRef recordType, CFArrayRef properties );
	using ::ABRemoveProperties;
	
	inline Owned<CFArrayRef> ABCopyArrayOfPropertiesForRecordType ( ABAddressBookRef addressBook, CFStringRef recordType ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFArrayRef result = ::ABCopyArrayOfPropertiesForRecordType ( addressBook, recordType );
		return Owned<CFArrayRef>::Seize( result );
    	}
    
//	extern ABPropertyType ABTypeOfProperty ( ABAddressBookRef addressBook, CFStringRef recordType, CFStringRef property );
	using ::ABTypeOfProperty;

// --- Records (Person, Group)
    inline Owned<ABRecordRef> ABCopyRecordForUniqueId ( ABAddressBookRef addressBook, CFStringRef uniqueId ) {
		OnlyOnce<RegisterAddressBookErrors>();
		ABRecordRef result = ::ABCopyRecordForUniqueId ( addressBook, uniqueId );
		return Owned<ABRecordRef>::Seize( result );
    	}

//	extern bool ABAddRecord		( ABAddressBookRef addressBook, ABRecordRef record );
//	extern bool ABRemoveRecord  ( ABAddressBookRef addressBook, ABRecordRef record );
	using ::ABAddRecord;
	using ::ABRemoveRecord;


// --- People
	inline Owned<CFArrayRef> ABCopyArrayOfAllPeople ( ABAddressBookRef addressBook ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFArrayRef result = ::ABCopyArrayOfAllPeople ( addressBook );
		return Owned<CFArrayRef>::Seize( result );
    	}

// --- Groups
    inline Owned<CFArrayRef> ABCopyArrayOfAllGroups ( ABAddressBookRef addressBook ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFArrayRef result = ::ABCopyArrayOfAllGroups ( addressBook );
		return Owned<CFArrayRef>::Seize( result );
    	}

// --------------------------------------------------------------------------------
//      ABRecord
// --------------------------------------------------------------------------------
// --- Record Type
	inline Owned<CFStringRef> ABRecordCopyRecordType ( ABRecordRef record ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result = ::ABRecordCopyRecordType ( record );
		return Owned<CFStringRef>::Seize( result );
    	}

// --- Property value
	inline Owned<CFTypeRef> ABRecordCopyValue ( ABRecordRef record, CFStringRef property ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFTypeRef result = ::ABRecordCopyValue ( record, property );
		return Owned<CFTypeRef>::Seize( result );
    	}

//	returns a CFDictionary for multi-value properties
//	extern bool ABRecordSetValue(ABRecordRef record, CFStringRef property, CFTypeRef value);
	using ::ABRecordSetValue;
	
//	takes a CFDictionary for multi-value properties
//	extern bool ABRecordRemoveValue(ABRecordRef record, CFStringRef property);
	using ::ABRecordRemoveValue;
	
// ---- Unique ID access convenience
	inline Owned<CFStringRef> ABRecordCopyUniqueId ( ABRecordRef record ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result = ::ABRecordCopyUniqueId ( record );
		return Owned<CFStringRef>::Seize( result );
    	}

// --------------------------------------------------------------------------------
//      ABPerson
// --------------------------------------------------------------------------------
	inline Owned<ABPersonRef> ABPersonCreate ( void ) {
		OnlyOnce<RegisterAddressBookErrors>();
		ABPersonRef result = ::ABPersonCreate ();
		return Owned<ABPersonRef>::Seize( result );
		}
	
	inline Owned<ABPersonRef> ABPersonCreateWithVCardRepresentation ( CFDataRef vCard ) {
		OnlyOnce<RegisterAddressBookErrors>();
		ABPersonRef result = ::ABPersonCreateWithVCardRepresentation ( vCard );
		return Owned<ABPersonRef>::Seize( result );
		}

	inline Owned<CFDataRef> ABPersonCopyVCardRepresentation ( ABPersonRef person ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFDataRef result = ::ABPersonCopyVCardRepresentation ( person );
		return Owned<CFDataRef>::Seize( result );
		}

//	Groups this person belongs to
 	inline Owned<CFArrayRef> ABPersonCopyParentGroups ( ABPersonRef person ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFArrayRef result = ::ABPersonCopyParentGroups ( person );
		return Owned<CFArrayRef>::Seize( result );
		}

// --- Search elements
 	inline Owned<ABSearchElementRef> ABPersonCreateSearchElement ( CFStringRef property, CFStringRef label, CFStringRef key, CFTypeRef value, ABSearchComparison comparison ) {
		OnlyOnce<RegisterAddressBookErrors>();
		ABSearchElementRef result = ::ABPersonCreateSearchElement ( property, label, key, value, comparison );
		return Owned<ABSearchElementRef>::Seize( result );
		}

// --------------------------------------------------------------------------------
//      ABGroups
// --------------------------------------------------------------------------------

 	inline Owned<ABGroupRef> ABGroupCreate ( void ) {
		OnlyOnce<RegisterAddressBookErrors>();
		ABGroupRef result = ::ABGroupCreate ();
		return Owned<ABGroupRef>::Seize( result );
		}

// --- Dealing with Persons
 	inline Owned<CFArrayRef> ABGroupCopyArrayOfAllMembers ( ABGroupRef group ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFArrayRef result = ::ABGroupCopyArrayOfAllMembers ( group );
		return Owned<CFArrayRef>::Seize( result );
		}

//	extern bool ABGroupAddMember 	( ABGroupRef group, ABPersonRef personToAdd );
//	extern bool ABGroupRemoveMember ( ABGroupRef group, ABPersonRef personToRemove );
	using ::ABGroupAddMember;
	using ::ABGroupRemoveMember;


// --- Dealing with Groups
 	inline Owned<CFArrayRef> ABGroupCopyArrayOfAllSubgroups ( ABGroupRef group ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFArrayRef result = ::ABGroupCopyArrayOfAllSubgroups ( group );
		return Owned<CFArrayRef>::Seize( result );
		}

//	extern bool ABGroupAddGroup		( ABGroupRef group, ABGroupRef groupToAdd );
//	extern bool ABGroupRemoveGroup  ( ABGroupRef group, ABGroupRef groupToRemove );
	using ::ABGroupAddGroup;
	using ::ABGroupRemoveGroup;

// --- Dealong with Parents
 	inline Owned<CFArrayRef> ABGroupCopyParentGroups ( ABGroupRef group ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFArrayRef result = ::ABGroupCopyParentGroups ( group );
		return Owned<CFArrayRef>::Seize( result );
		}

// --- Distribution list
//	extern bool ABGroupSetDistributionIdentifier ( ABGroupRef group, ABPersonRef person, CFStringRef property, CFStringRef identifier );
	using ::ABGroupSetDistributionIdentifier;
	
 	inline Owned<CFStringRef> ABGroupCopyDistributionIdentifier ( ABGroupRef group, ABPersonRef person, CFStringRef property ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result = ::ABGroupCopyDistributionIdentifier ( group, person, property );
		return Owned<CFStringRef>::Seize( result );
		}

// --- Search elements
 	inline Owned<ABSearchElementRef> ABGroupCreateSearchElement ( CFStringRef property, CFStringRef label, CFStringRef key, CFTypeRef value, ABSearchComparison comparison ) {
		OnlyOnce<RegisterAddressBookErrors>();
		ABSearchElementRef result = ::ABGroupCreateSearchElement ( property, label, key, value, comparison );
		return Owned<ABSearchElementRef>::Seize( result );
		}

// --------------------------------------------------------------------------------
//      ABSearchElement
// --------------------------------------------------------------------------------
 	inline Owned<ABSearchElementRef> ABSearchElementCreateWithConjunction ( ABSearchConjunction conjunction, CFArrayRef childrenSearchElement ) {
		OnlyOnce<RegisterAddressBookErrors>();
		ABSearchElementRef result = ::ABSearchElementCreateWithConjunction ( conjunction, childrenSearchElement );
		return Owned<ABSearchElementRef>::Seize( result );
		}

//	extern bool ABSearchElementMatchesRecord ( ABSearchElementRef searchElement, ABRecordRef record );
	using ::ABSearchElementMatchesRecord;

// --------------------------------------------------------------------------------
//      ABMultiValue
// --------------------------------------------------------------------------------
 	inline Owned<ABMultiValueRef> ABMultiValueCreate ( void ) {
		OnlyOnce<RegisterAddressBookErrors>();
		ABMultiValueRef result = ::ABMultiValueCreate ();
		return Owned<ABMultiValueRef>::Seize( result );
		}

//	extern unsigned ABMultiValueCount ( ABMultiValueRef multiValue );
//	unsigned? Sheesh!
	using ::ABMultiValueCount;

//	And if ABMultiValueCount returns an unsigned here, why is index an 'int' ????
	inline Owned<CFTypeRef> ABMultiValueCopyValueAtIndex ( ABMultiValueRef multiValue, int index ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFTypeRef result = ::ABMultiValueCopyValueAtIndex ( multiValue, index );
		return Owned<CFTypeRef>::Seize( result );
		}

	inline Owned<CFStringRef> ABMultiValueCopyLabelAtIndex ( ABMultiValueRef multiValue, int index ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result = ::ABMultiValueCopyLabelAtIndex ( multiValue, index );
		return Owned<CFStringRef>::Seize( result );
		}

	inline Owned<CFStringRef> ABMultiValueCopyPrimaryIdentifier ( ABMultiValueRef multiValue ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result = ::ABMultiValueCopyPrimaryIdentifier ( multiValue );
		return Owned<CFStringRef>::Seize( result );
		}

//	extern int ABMultiValueIndexForIdentifier ( ABMultiValueRef multiValue, CFStringRef identifier );
	using ::ABMultiValueIndexForIdentifier;
	
	inline Owned<CFStringRef> ABMultiValueCopyIdentifierAtIndex ( ABMultiValueRef multiValue, int index ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result = ::ABMultiValueCopyIdentifierAtIndex ( multiValue, index );
		return Owned<CFStringRef>::Seize( result );
		}

//	extern ABPropertyType ABMultiValuePropertyType ( ABMultiValueRef multiValue );
	using ::ABMultiValuePropertyType;
	
 	inline Owned<ABMultiValueRef> ABMultiValueCreateCopy ( ABMultiValueRef multiValue ) {
		OnlyOnce<RegisterAddressBookErrors>();
		ABMultiValueRef result = ::ABMultiValueCreateCopy ( multiValue );
		return Owned<ABMultiValueRef>::Seize( result );
		}

// --------------------------------------------------------------------------------
//      ABMutableMultiValue
// --------------------------------------------------------------------------------
 	inline Owned<ABMutableMultiValueRef> ABMultiValueCreateMutable ( void ) {
		OnlyOnce<RegisterAddressBookErrors>();
		ABMutableMultiValueRef result = ::ABMultiValueCreateMutable ();
		return Owned<ABMutableMultiValueRef>::Seize( result );
		}

	inline Owned<CFStringRef> ABMultiValueAdd ( ABMutableMultiValueRef multiValue, CFTypeRef value, CFStringRef label ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result;
		if ( !::ABMultiValueAdd ( multiValue, value, label, &result ))
			throw MemFullErr ();
		return Owned<CFStringRef>::Seize( result );
		}

	inline Owned<CFStringRef> ABMultiValueInsert ( ABMutableMultiValueRef multiValue, CFTypeRef value, CFStringRef label, int index ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result;
		if ( !::ABMultiValueInsert ( multiValue, value, label, index, &result ))
			throw MemFullErr ();
		return Owned<CFStringRef>::Seize( result );
		}

//	extern bool ABMultiValueRemove ( ABMutableMultiValueRef multiValue, int index );
	using ::ABMultiValueRemove;
	
//	extern bool ABMultiValueReplaceValue ( ABMutableMultiValueRef multiValue, CFTypeRef value, int index );
//	extern bool ABMultiValueReplaceLabel ( ABMutableMultiValueRef multiValue, CFStringRef label, int index );
	using ::ABMultiValueReplaceValue;
	using ::ABMultiValueReplaceLabel;

//	extern bool ABMultiValueSetPrimaryIdentifier (ABMutableMultiValueRef multiValue, CFStringRef identifier );
	using ::ABMultiValueReplaceLabel;
	
	inline Owned<ABMutableMultiValueRef> ABMultiValueCreateMutableCopy ( ABMultiValueRef multiValue ) {
		OnlyOnce<RegisterAddressBookErrors>();
		ABMutableMultiValueRef result = ::ABMultiValueCreateMutableCopy ( multiValue );
		return Owned<ABMutableMultiValueRef>::Seize( result );
		}

// --------------------------------------------------------------------------------
//      Localization of properties or labels
// --------------------------------------------------------------------------------
	inline Owned<CFStringRef> ABCopyLocalizedPropertyOrLabel ( CFStringRef labelOrProperty ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result = ::ABCopyLocalizedPropertyOrLabel ( labelOrProperty );
		return Owned<CFStringRef>::Seize( result );
		}

// --- Address formatting
	inline Owned<CFStringRef> ABCreateFormattedAddressFromDictionary ( ABAddressBookRef addressBook, CFDictionaryRef address ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result = ::ABCreateFormattedAddressFromDictionary ( addressBook, address );
		return Owned<CFStringRef>::Seize( result );
		}

	inline Owned<CFStringRef> ABCopyDefaultCountryCode ( ABAddressBookRef addressBook ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFStringRef result = ::ABCopyDefaultCountryCode ( addressBook );
		return Owned<CFStringRef>::Seize( result );
		}

// --------------------------------------------------------------------------------
//      Person Image Loading
// --------------------------------------------------------------------------------
//	extern bool ABPersonSetImageData ( ABPersonRef person, CFDataRef imageData );
	using ::ABPersonSetImageData;

	inline Owned<CFDataRef> ABPersonCopyImageData ( ABPersonRef person ) {
		OnlyOnce<RegisterAddressBookErrors>();
		CFDataRef result = ::ABPersonCopyImageData ( person );
		return Owned<CFDataRef>::Seize( result );
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
