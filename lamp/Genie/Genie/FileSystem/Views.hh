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
	
	struct ViewFactory
	{
		virtual void operator()( Pedestal::UserWindow& window ) const = 0;
	};
	
	
	void AddViewFactory( const FSTree* key, const ViewFactory& factory );
	
	void RemoveViewFactory( const FSTree* key );
	
	const ViewFactory* GetViewFactory( const FSTree* key );
	
	
	void AddViewDelegate( const FSTree* key, const FSTreePtr& delegate );
	
	void RemoveViewDelegate( const FSTree* key );
	
	const FSTreePtr& GetViewDelegate( const FSTree* key );
	
}

#endif

