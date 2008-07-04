/*	================
 *	DynamicGroups.hh
 *	================
 */

#ifndef GENIE_FILESYSTEM_DYNAMICGROUPS_HH
#define GENIE_FILESYSTEM_DYNAMICGROUPS_HH

// Nucleus
#include "Nucleus/Convert.h"

// Genie 
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/IO/DynamicGroup.hh"


namespace Genie
{
	
	struct DynamicGroup_Details_Base : public Integer_KeyName_Traits< unsigned >
	{
		typedef DynamicGroup Sequence;
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value.first; }
		
		static std::string GetChildName( const Sequence::value_type& child );
	};
	
	class FSTree_Dynamic_N_Base : public FSTree
	{
		private:
			unsigned itsID;
		
		protected:
			unsigned ID() const  { return itsID; }
		
		public:
			FSTree_Dynamic_N_Base( unsigned id ) : itsID( id )
			{
			}
			
			std::string Name() const  { return Nucleus::Convert< std::string >( itsID ); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
	};
	
	
	template < class Handle > struct IOHandle_Name_Traits   {};
	template < class Handle > struct IOHandle_Parent_Traits {};
	template < class Handle > struct IOHandle_FSTree_Traits {};
	
	
	template < class Handle >
	class FSTree_Dynamic_N : public FSTree_Dynamic_N_Base
	{
		public:
			FSTree_Dynamic_N( unsigned id ) : FSTree_Dynamic_N_Base( id )
			{
			}
			
			FSTreePtr Parent() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				return GetDynamicElementByID< Handle >( ID() );
			}
	};
	
	
	template < class Handle >
	FSTreePtr FSTree_Dynamic_N< Handle >::Parent() const
	{
		return GetSingleton< typename IOHandle_FSTree_Traits< Handle >::Tree >();
	}
	
	
	template < class Handle >
	struct DynamicGroup_Details : public DynamicGroup_Details_Base
	{
		typedef typename FSTree_Dynamic_N< Handle > ChildNode;
		
		static std::string Name()  { return IOHandle_Name_Traits< Handle >::Name(); }
		
		FSTreePtr Parent() const  { return GetSingleton< typename IOHandle_Parent_Traits< Handle >::Tree >(); }
		
		FSTreePtr Lookup( const std::string& name ) const;
		
		static bool KeyIsValid( const Key& key )
		{
			const Sequence& sequence = ItemSequence();
			
			return sequence.find( key ) != sequence.end();
		}
		
		FSTreePtr GetChildNode( const Key& key ) const
		{
			return FSTreePtr( new ChildNode( key ) );
		}
		
		const Sequence& ItemSequence() const  { return GetDynamicGroup< Handle >(); }
	};
	
	template < class Handle >
	FSTreePtr DynamicGroup_Details< Handle >::Lookup( const std::string& name ) const
	{
		Key key = KeyFromName( name );
		
		return FSTreePtr( new ChildNode( key ) );
	}
	
}

#endif

