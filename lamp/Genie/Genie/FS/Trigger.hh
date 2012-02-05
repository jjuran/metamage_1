/*	==========
 *	Trigger.hh
 *	==========
 */

#ifndef GENIE_FILESYSTEM_TRIGGER_HH
#define GENIE_FILESYSTEM_TRIGGER_HH

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class Trigger_Base : public FSTree
	{
		public:
			Trigger_Base( const FSTreePtr&     parent,
			              const plus::string&  name,
			              mode_t               mode = S_IFCHR | 0200 );
			
			virtual void Invoke() const = 0;
			
			IOPtr Open( OpenFlags flags ) const;
	};
	
	template < class Action >
	class Trigger : public Trigger_Base
	{
		private:
			Action itsAction;
		
		public:
			Trigger( const FSTreePtr&     parent,
			         const plus::string&  name,
			         const Action&        action )
			:
				Trigger_Base( parent, name ),
				itsAction( action  )
			{
			}
			
			Trigger( const FSTreePtr&     parent,
			         const plus::string&  name )
			:
				Trigger_Base( parent, name ),
				itsAction()
			{
			}
			
			void Invoke() const;
	};
	
	template < class Action >
	void Trigger< Action >::Invoke() const
	{
		itsAction();
	}
	
}

#endif

