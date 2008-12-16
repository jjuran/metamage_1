/*	========
 *	Views.hh
 *	========
 */

#ifndef GENIE_FILESYSTEM_VIEWS_HH
#define GENIE_FILESYSTEM_VIEWS_HH

// Standard C++
#include <memory>

// Genie
#include "Genie/FileSystem/FSTree.hh"


struct Rect;

namespace Pedestal
{
	
	class View;
	
}

namespace Genie
{
	
	bool ViewExists( const FSTree* parent, const std::string& name );
	
	void RemoveViewParameters( const FSTree* parent, const std::string& name );
	
	void RemoveAllViewParameters( const FSTree* parent );
	
	struct ViewFactory
	{
		virtual std::auto_ptr< Pedestal::View > operator()() const = 0;
	};
	
	void AddViewParameters( const FSTree*                            parent,
	                        const std::string&                       name,
	                        const boost::shared_ptr< ViewFactory >&  factory,
	                        const FSTreePtr&                         delegate );
	
	void AddViewFactory( const FSTree* parent, const std::string& name, const boost::shared_ptr< ViewFactory >& factory );
	
	void AddViewWindowKey( const FSTree* parent, const std::string& name, const FSTree* windowKey );
	
	const boost::shared_ptr< ViewFactory >& GetViewFactory( const FSTree* parent, const std::string& name );
	
	
	void AddViewDelegate( const FSTree* parent, const std::string& name, const FSTreePtr& delegate );
	
	const FSTreePtr& GetViewDelegate( const FSTree* parent, const std::string& name );
	
	const FSTree* GetViewWindowKey( const FSTree* parent, const std::string& name );
	
	
	class FSTree_View : public FSTree
	{
		public:
			FSTree_View( const FSTreePtr&    parent,
			             const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			const FSTree* ParentKey() const  { return Parent().get(); }
			
			bool IsDirectory() const  { return Exists(); }
			
			bool Exists() const;
			
			void SetTimes() const;
			
			void Delete() const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup( const std::string& name ) const;
			
			FSIteratorPtr Iterate() const;
			
			virtual void DeleteCustomParameters() const  {}
			
			virtual void AddCustomParameters( std::auto_ptr< Pedestal::View > view ) const  {}
	};
	
}

#endif

