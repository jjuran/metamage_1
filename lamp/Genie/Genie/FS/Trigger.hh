/*	==========
 *	Trigger.hh
 *	==========
 */

#ifndef GENIE_FILESYSTEM_TRIGGER_HH
#define GENIE_FILESYSTEM_TRIGGER_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class Trigger_Base : public FSTree
	{
		public:
			Trigger_Base( const FSTreePtr&    parent,
			              const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			virtual void Invoke() const = 0;
			
			mode_t FileTypeMode() const;
			mode_t FilePermMode() const  { return 0200; }
			
			void SetTimes() const  { Invoke(); }
	};
	
	template < class Action >
	class Trigger : public Trigger_Base
	{
		private:
			Action itsAction;
		
		public:
			Trigger( const FSTreePtr&    parent,
			         const std::string&  name,
			         const Action&       action ) : Trigger_Base( parent, name ),
			                                        itsAction( action  )
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

