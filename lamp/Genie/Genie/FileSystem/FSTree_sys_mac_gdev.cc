/*	======================
 *	FSTree_sys_mac_gdev.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_sys_mac_gdev.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_Virtual_Link.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static std::string PrintableBounds( const Rect& r )
	{
		std::string result;
		
		result += NN::Convert< std::string >( r.left );
		
		result += ",";
		
		result += NN::Convert< std::string >( r.top );
		
		result += "-";
		
		result += NN::Convert< std::string >( r.right );
		
		result += ",";
		
		result += NN::Convert< std::string >( r.bottom );
		
		return result;
	}
	
	static std::string PrintableSize( int x, int y )
	{
		std::string result;
		
		result += NN::Convert< std::string >( x );
		
		result += "x";
		
		result += NN::Convert< std::string >( y );
		
		return result;
	}
	
	inline std::string PrintableSize( Point size )
	{
		return PrintableSize( size.h, size.v );
	}
	
	inline std::string PrintableSize( const Rect& bounds )
	{
		return PrintableSize( bounds.right - bounds.left, bounds.bottom - bounds.top );
	}
	
	
	bool sys_mac_gdev_Details::KeyIsValid( const Key& key )
	{
		Sequence sequence = ItemSequence();
		
		return std::find( sequence.begin(), sequence.end(), key ) != sequence.end();
	}
	
	
	extern const Functional_Traits< GDHandle_KeyName_Traits::Key >::Mapping sys_mac_gdev_H_Mappings[];
	
	FSTreePtr sys_mac_gdev_Details::GetChildNode( const FSTreePtr&    parent,
		                                          const std::string&  name,
		                                          const Key&          key )
	{
		return Premapped_Factory< Key, sys_mac_gdev_H_Mappings >( parent, name, key );
	}
	
	
	struct GetGDBounds
	{
		typedef std::string Result;
		
		Result Get( N::GDHandle gdevice ) const
		{
			return PrintableBounds( Rect( gdevice[0]->gdRect ) );
		}
	};
	
	struct GetGDSize
	{
		typedef std::string Result;
		
		Result Get( N::GDHandle gdevice ) const
		{
			return PrintableSize( gdevice[0]->gdRect );
		}
	};
	
	template < class Accessor >
	class sys_mac_gdev_H_Query
	{
		private:
			typedef N::GDHandle Key;
			
			Key itsKey;
		
		public:
			sys_mac_gdev_H_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string Get() const
			{
				std::string output = NN::Convert< std::string >( Accessor().Get( itsKey ) ) + "\n";
				
				return output;
			}
	};
	
	static FSTreePtr Driver_Link_Factory( const FSTreePtr&              parent,
	                                      const std::string&            name,
	                                      GDHandle_KeyName_Traits::Key  key )
	{
		std::string unit = NN::Convert< std::string >( ~key[0]->gdRefNum );
		
		return FSTreePtr( new FSTree_Virtual_Link( parent, name, "/sys/mac/unit/" + unit ) );
	}
	
	template < class Accessor >
	static FSTreePtr Query_Factory( const FSTreePtr&               parent,
	                                const std::string&             name,
	                                GDHandle_KeyName_Traits::Key   key )
	{
		typedef sys_mac_gdev_H_Query< Accessor > Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return FSTreePtr( new QueryFile( parent, name, Query( key ) ) );
	}
	
	const Functional_Traits< GDHandle_KeyName_Traits::Key >::Mapping sys_mac_gdev_H_Mappings[] =
	{
		{ "driver", &Driver_Link_Factory },
		
		{ "bounds", &Query_Factory< GetGDBounds > },
		
		{ NULL, NULL }
	};
	
}

