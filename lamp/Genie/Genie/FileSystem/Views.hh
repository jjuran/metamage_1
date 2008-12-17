/*	========
 *	Views.hh
 *	========
 */

#ifndef GENIE_FILESYSTEM_VIEWS_HH
#define GENIE_FILESYSTEM_VIEWS_HH

// Standard C++
#include <memory>

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


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
	
	
	typedef std::auto_ptr< Pedestal::View > (*ViewFactory)( const FSTree* delegate );
	
	
	void AddViewParameters( const FSTree*       parent,
	                        const std::string&  name,
	                        const FSTreePtr&    delegate,
	                        ViewFactory         factory );
	
	void AddViewWindowKey( const FSTree* parent, const std::string& name, const FSTree* windowKey );
	
	
	std::auto_ptr< Pedestal::View > MakeView( const FSTree* parent, const std::string& name );
	
	
	bool InvalidateWindowForView( const FSTree* view );
	
	
	class FSTree_new_View : public FSTree
	{
		private:
			typedef const Singleton_Mapping* Mappings;
			
			typedef void (*Destructor)( const FSTree* delegate );
			
			ViewFactory  itsFactory;
			Mappings     itsMappings;
			Destructor   itsDestructor;
		
		public:
			FSTree_new_View( const FSTreePtr&    parent,
			                 const std::string&  name,
			                 ViewFactory         factory,
			                 Mappings            mappings,
			                 Destructor          dtor ) : FSTree( parent, name ),
			                                              itsFactory( factory ),
			                                              itsMappings( mappings ),
			                                              itsDestructor( dtor )
			{
			}
			
			void HardLink( const FSTreePtr& target ) const;
	};
	
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

