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
	};
	
	class FSTree_Dynamic_N_Base : public FSTree
	{
		private:
			unsigned itsID;
		
		protected:
			unsigned ID() const  { return itsID; }
		
		public:
			FSTree_Dynamic_N_Base( const FSTreePtr&  parent,
			                       unsigned          id ) : FSTree( parent ),
			                                                itsID ( id     )
			{
			}
			
			std::string Name() const  { return Nucleus::Convert< std::string >( itsID ); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
	};
	
	
	template < class Handle >
	class FSTree_Dynamic_N : public FSTree_Dynamic_N_Base
	{
		public:
			FSTree_Dynamic_N( const FSTreePtr&  parent,
			                  unsigned          id ) : FSTree_Dynamic_N_Base( parent, id )
			{
			}
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				return GetDynamicElementByID< Handle >( ID() );
			}
	};
	
	
	template < class Handle >
	struct DynamicGroup_Details : public DynamicGroup_Details_Base
	{
		typedef typename FSTree_Dynamic_N< Handle > ChildNode;
		
		static bool KeyIsValid( const Key& key )
		{
			const Sequence& sequence = ItemSequence();
			
			return sequence.find( key ) != sequence.end();
		}
		
		static FSTreePtr GetChildNode( const FSTreePtr& parent, const Key& key )
		{
			return MakeFSTree( new ChildNode( parent, key ) );
		}
		
		static const Sequence& ItemSequence()  { return GetDynamicGroup< Handle >(); }
	};
	
}

#endif

