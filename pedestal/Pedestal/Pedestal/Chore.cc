/*	===========
 *	PedChore.cc
 *	===========
 */

#include "Pedestal/Chore.hh"


namespace Pedestal
{
	
	void
	PedChoreKeeper::AddChore(Ped_Chore& chore)
	{
		myChores.insert(&chore);
	}
	
	void
	PedChoreKeeper::RemoveChore(Ped_Chore& chore)
	{
		myChores.erase(&chore);
	}
	
	void
	PedChoreKeeper::PerformAll()
	{
		typedef std::set<Ped_Chore*>::iterator Iter;
		Iter it = myChores.begin();
		while (it != myChores.end()) {
			Ped_Chore* chore = *it;
			//try {
				Iter next = ++it;
				chore->Perform();
				it = next;
			//} catch (...) {
				// FIXME:  We should have a diagnostic message here.
				//set<Ped_Chore*>::iterator victim = it++;
				//(*victim)->Unschedule();
			//}
		}
	}
	
}

