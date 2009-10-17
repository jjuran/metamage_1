/*	======================
 *	FSTree_sys_mac_unit.cc
 *	======================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FS/FSTree_sys_mac_unit.hh"

// Debug
#include "debug/assert.hh"

// Nucleus
#include "Nucleus/IndexedContainer.h"

// BitsAndBytes
#include "HexStrings.hh"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"


namespace Nitrogen
{
	
	struct UnitTable_Container_Specifics
	{
		typedef UInt16             size_type;
		typedef SInt16             difference_type;
		typedef AuxDCEHandle       value_type;
		typedef const value_type&  const_reference;
		typedef const value_type*  const_pointer;
		
		class Nothing {};
		
		typedef Nothing ConstIteratorState;
		typedef Nothing ConstContainerState;
		
		
		static size_type Size( ConstContainerState )
		{
			return LMGetUnitTableEntryCount();
		}
		
		static const_reference GetReference( ConstIteratorState state, size_type position )
		{
			AuxDCEHandle* base = (AuxDCEHandle*) LMGetUTableBase();
			
			return base[ position ];
		}
		
		static const_pointer GetPointer( ConstIteratorState state, size_type position )
		{
			return &GetReference( state, position );
		}
	};
	
	class UnitTable_Container: public Nucleus::ConstIndexedContainer< UnitTable_Container_Specifics >
	{
		friend UnitTable_Container UnitTable();
		
		private:
			UnitTable_Container()
			: Nucleus::ConstIndexedContainer< UnitTable_Container_Specifics >( UnitTable_Container_Specifics::Nothing() )
			{}
	};
	
	inline UnitTable_Container UnitTable()
	{
		return UnitTable_Container();
	}
	
	
	UnitTableDrivers_Container_Specifics::key_type
	//
	UnitTableDrivers_Container_Specifics::GetNextKey( key_type key )
	{
		const key_type end = end_key();
		
		while ( ++key < end  &&  *key == NULL )
		{
			continue;
		}
		
		return key;
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	AuxDCEHandle* GetUTableBase();
	
	
	struct UnitNumber_KeyName_Traits : Integer_KeyName_Traits< UnitNumber >
	{
	};
	
	struct sys_mac_unit_Details : public UnitNumber_KeyName_Traits
	{
		typedef Nitrogen::UnitTableDrivers_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::UnitTableDrivers(); }
		
		static Key KeyFromValue( Sequence::const_reference ref )  { return &ref - GetUTableBase(); }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_unit_Details > FSTree_sys_mac_unit;
	
	
	static UnitNumber GetKey( const FSTree* that )
	{
		return UnitNumber_KeyName_Traits::KeyFromName( that->ParentRef()->Name() );
	}
	
	
	AuxDCEHandle* GetUTableBase()
	{
		return (AuxDCEHandle*) LMGetUTableBase();
	}
	
	
	bool sys_mac_unit_Details::KeyIsValid( const Key& key )
	{
		AuxDCEHandle* base = GetUTableBase();
		
		const UInt16 count = LMGetUnitTableEntryCount();
		
		return key < count  &&  base[ key ] != NULL;
	}
	
	
	struct GetDriverFlags
	{
		static const bool hexEncoded = true;
		
		static const bool alwaysStringified = false;
		
		typedef short Result;
		
		static Result Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			return dceHandle[0]->dCtlFlags;
		}
	};
	
	const unsigned char* GetDriverName_WithinHandle( AuxDCEHandle dceHandle )
	{
		ASSERT( dceHandle != NULL );
		
		if ( dceHandle[0]->dCtlDriver != NULL )
		{
			const bool ramBased = dceHandle[0]->dCtlFlags & dRAMBasedMask;
			
			const Ptr drvr = dceHandle[0]->dCtlDriver;
			
			// Dereferences a handle if ramBased
			const DRVRHeaderPtr header = ramBased ? *(DRVRHeader **) drvr
			                                      :  (DRVRHeader * ) drvr;
			
			if ( header != NULL )
			{
				return header->drvrName;
			}
		}
		
		return "\p";
	}
	
	static N::Str255 GetDriverName( AuxDCEHandle dceHandle )
	{
		const unsigned char* name = GetDriverName_WithinHandle( dceHandle );
		
		// Safely copy Pascal string onto stack
		return N::Str255( name );
	}
	
	struct DriverName
	{
		static const bool hexEncoded = false;
		
		static const bool alwaysStringified = true;
		
		typedef N::Str255 Result;
		
		static Result Get( AuxDCEHandle dceHandle )
		{
			return GetDriverName( dceHandle );
		}
	};
	
	struct GetDriverSlot
	{
		static const bool hexEncoded = false;
		
		static const bool alwaysStringified = false;
		
		// dCtlSlot is defined as 'char', but we want integer conversion
		typedef UInt16 Result;
		
		static Result Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlSlot == 0 )
			{
				N::ThrowOSStatus( fnfErr );
			}
			
			return dceHandle[0]->dCtlSlot;
		}
	};
	
	struct GetDriverBase
	{
		static const bool hexEncoded = true;
		
		static const bool alwaysStringified = false;
		
		typedef long Result;
		
		static Result Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlDevBase == 0 )
			{
				N::ThrowOSStatus( fnfErr );
			}
			
			return dceHandle[0]->dCtlDevBase;
		}
	};
	
	template < class Accessor >
	struct sys_mac_unit_N_Property
	{
		private:
			typedef UnitNumber Key;
		
		public:
			static std::string Read( const FSTree* that, bool binary )
			{
				Key key = GetKey( that );
				
				if ( !sys_mac_unit_Details::KeyIsValid( key ) )
				{
					throw FSTree_Property::Undefined();
				}
				
				AuxDCEHandle dceHandle = GetUTableBase()[ key ];
				
				const typename Accessor::Result data = Accessor::Get( dceHandle );
				
				const bool raw = !Accessor::alwaysStringified  &&  binary;
				const bool hex =  Accessor::hexEncoded;
				
				using BitsAndBytes::EncodeAsHex;
				
				std::string result = raw ? std::string( (char*) &data, sizeof data )
				                   : hex ? EncodeAsHex(         &data, sizeof data )
				                   :       NN::Convert< std::string >( data );
				
				return result;
			}
	};
	
	
	extern const FSTree_Premapped::Mapping sys_mac_unit_N_Mappings[];
	
	FSTreePtr sys_mac_unit_Details::GetChildNode( const FSTreePtr&    parent,
		                                          const std::string&  name,
		                                          const Key&          key )
	{
		return Premapped_Factory< sys_mac_unit_N_Mappings >( parent, name );
	}
	
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_mac_unit_N_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_unit_N_Mappings[] =
	{
		{ "flags", &Property_Factory< GetDriverFlags > },
		{ "name",  &Property_Factory< DriverName  > },
		{ "slot",  &Property_Factory< GetDriverSlot  >, true },
		{ "base",  &Property_Factory< GetDriverBase  >, true },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_unit( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_sys_mac_unit( parent, name ) );
	}
	
}

#endif

