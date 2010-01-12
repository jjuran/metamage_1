/*	===================
 *	FSTree_IconSuite.cc
 *	===================
 */

#include "Genie/FS/FSTree_IconSuite.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	class FSTree_IconSuite : public FSTree
	{
		private:
			typedef NN::Shared< N::IconSuiteRef > Value;
			
			Value itsIconSuite;
		
		public:
			FSTree_IconSuite( const FSTreePtr&    parent,
			                  const std::string&  name,
			                  const Value&        iconSuite )
			:
				FSTree( parent, name ),
				itsIconSuite( iconSuite )
			{
			}
			
			void CopyFile( const FSTreePtr& destination ) const;
	};
	
	
	static NN::Shared< N::IconSuiteRef > gStoredIconSuite;
	
	void FSTree_IconSuite::CopyFile( const FSTreePtr& destination ) const
	{
		gStoredIconSuite = itsIconSuite;
		
		try
		{
			destination->Attach( shared_from_this() );
		}
		catch ( ... )
		{
			gStoredIconSuite.reset();
			
			throw;
		}
		
		gStoredIconSuite.reset();
	}
	
	
	FSTreePtr
	//
	New_FSTree_IconSuite( const FSTreePtr&                      parent,
			              const std::string&                    name,
			              const NN::Shared< N::IconSuiteRef >&  iconSuite )
	{
		return seize_ptr( new FSTree_IconSuite( parent, name, iconSuite ) );
	}
	
	const NN::Shared< N::IconSuiteRef >& Fetch_IconSuite()
	{
		return gStoredIconSuite;
	}
	
}

