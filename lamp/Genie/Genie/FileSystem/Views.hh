/*	========
 *	Views.hh
 *	========
 */

#ifndef GENIE_FILESYSTEM_VIEWS_HH
#define GENIE_FILESYSTEM_VIEWS_HH

// Pedestal
#include "Pedestal/UserWindow.hh"

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	class FSTree;
	
	bool ViewExists( const FSTree* key );
	
	void RemoveViewParameters( const FSTree* key );
	
	struct ViewFactory
	{
		virtual void operator()( Pedestal::UserWindow& window ) const = 0;
	};
	
	
	void AddViewFactory( const FSTree* key, const boost::shared_ptr< ViewFactory >& factory );
	
	const boost::shared_ptr< ViewFactory >& GetViewFactory( const FSTree* key );
	
	
	void AddViewDelegate( const FSTree* key, const FSTreePtr& delegate );
	
	const FSTreePtr& GetViewDelegate( const FSTree* key );
	
}

#endif

