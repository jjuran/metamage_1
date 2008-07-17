/*	=====================
 *	FSTree_sys_mac_vol.hh
 *	=====================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACVOL_HH
#define GENIE_FILESYSTEM_FSTREESYSMACVOL_HH

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	struct VRefNum_KeyName_Traits
	{
		typedef Nitrogen::FSVolumeRefNum Key;
		
		static std::string NameFromKey( const Key& key );
		
		static Key KeyFromName( const std::string& name );
		
		static bool KeyIsValid( const Key& key );
	};
	
	class FSTree_sys_mac_vol_N;
	
	
	struct sys_mac_vol_Details : public VRefNum_KeyName_Traits
	{
		typedef Nitrogen::Volume_Container Sequence;
		
		typedef FSTree_sys_mac_vol_N ChildNode;
		
		static std::string Name()  { return "vol"; }
		
		static FSTreePtr Parent();
		
		static const Sequence& ItemSequence()  { return Nitrogen::Volumes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static FSTreePtr GetChildNode( const Key& key );
	};
	
	typedef FSTree_Sequence< sys_mac_vol_Details > FSTree_sys_mac_vol;
	
	
	class FSTree_sys_mac_vol_N : public FSTree_Virtual,
	                             public VRefNum_KeyName_Traits
	{
		private:
			Key itsKey;
		
		public:
			FSTree_sys_mac_vol_N( const Key& key ) : itsKey( key )
			{
			}
			
			void Init();
			
			std::string Name() const  { return NameFromKey( itsKey ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_mac_vol >(); }
	};
	
	
	FSTreePtr Get_sys_mac_vol_N( Nitrogen::FSVolumeRefNum vRefNum );
	
}

#endif

