/*	======================
 *	FSTree_sys_mac_unit.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACUNIT_HH
#define GENIE_FILESYSTEM_FSTREESYSMACUNIT_HH

// Mac OS Universal Interfaces
#ifndef __LOWMEM__
#include <LowMem.h>
#endif

// Nucleus
#include "Nucleus/AdvanceUntilFailureContainer.h"
#include "Nucleus/IndexedContainer.h"

// ClassicToolbox
#include "ClassicToolbox/Devices.h"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


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
	
	
	struct UnitTableDrivers_Container_Specifics
	{
		typedef UInt16             size_type;
		typedef SInt16             difference_type;
		typedef AuxDCEHandle       value_type;
		typedef const value_type&  const_reference;
		typedef const value_type*  const_pointer;
		typedef const_pointer      key_type;
		
		class EndOfEnumeration {};  // not used
		
		static key_type GetNextKey( key_type key )
		{
			const key_type end = end_key();
			
			while ( ++key < end  &&  *key == NULL )
			{
				continue;
			}
			
			return key;
		}
		
		static const_pointer GetPointer( key_type ptr )  { return ptr; }
		
		static key_type begin_key()  { return (AuxDCEHandle*) LMGetUTableBase(); }
		static key_type end_key()    { return begin_key() + LMGetUnitTableEntryCount(); }
	};
	
	class UnitTableDrivers_Container: public Nucleus::AdvanceUntilFailureContainer< UnitTableDrivers_Container_Specifics >
	{
		friend UnitTableDrivers_Container UnitTableDrivers();
		
		private:
			UnitTableDrivers_Container()
			: Nucleus::AdvanceUntilFailureContainer< UnitTableDrivers_Container_Specifics >( UnitTableDrivers_Container_Specifics() )
			{}
	};
	
	inline UnitTableDrivers_Container UnitTableDrivers()
	{
		return UnitTableDrivers_Container();
	}
	
}

namespace Genie
{
	
	AuxDCEHandle* GetUTableBase();
	
	struct UnitNumber_KeyName_Traits : Integer_KeyName_Traits< UnitNumber >
	{
	};
	
	class FSTree_sys_mac_unit_N;
	
	struct sys_mac_unit_Details : public UnitNumber_KeyName_Traits
	{
		typedef N::UnitTableDrivers_Container Sequence;
		
		typedef FSTree_sys_mac_unit_N ChildNode;
		
		static std::string Name()  { return "unit"; }
		
		static FSTreePtr Parent();
		
		static const Sequence& ItemSequence()  { return N::UnitTableDrivers(); }
		
		static Key KeyFromValue( Sequence::const_reference ref )  { return &ref - GetUTableBase(); }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr& parent, const Key& key );
	};
	
	typedef FSTree_Sequence< sys_mac_unit_Details > FSTree_sys_mac_unit;
	
	
	class sys_mac_unit_N_Details : public UnitNumber_KeyName_Traits
	{
		public:
			const Key itsKey;
		
		public:
			typedef FSTreePtr (*Function)( const FSTreePtr&, const std::string&, Key key );
			
			sys_mac_unit_N_Details( Key key ) : itsKey( key )
			{
			}
			
			FSTreePtr Invoke( Function f, const FSTreePtr& parent, const std::string& name ) const
			{
				return f( parent, name, itsKey );
			}
	};
	
	class FSTree_sys_mac_unit_N : public FSTree_Functional< sys_mac_unit_N_Details >
	{
		private:
			typedef FSTree_Functional< sys_mac_unit_N_Details > Base;
		
		public:
			FSTree_sys_mac_unit_N( const Key& key ) : Base( key )
			{
			}
			
			void Init();
			
			std::string Name() const  { return NameFromKey( itsKey ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_mac_unit >(); }
	};
	
}

#endif

