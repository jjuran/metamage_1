/*	===========
 *	PedChore.hh
 *	===========
 *	
 *	Implemented by PedChore.cc
 */

#ifndef PEDESTAL_CHORE_HH
#define PEDESTAL_CHORE_HH

// Standard C++
#include <set>

// Nitrogen core
#include "Nitrogen/Assert.h"


namespace Pedestal
{
	
	class Ped_Chore {
	public:
		virtual void Perform() = 0;
	};
	
	class PedChoreKeeper {
	public:
		void AddChore(Ped_Chore& chore);
		void RemoveChore(Ped_Chore& chore);
		void PerformAll();
	private:
		std::set<Ped_Chore*> myChores;
	};
	
	class PedChore : public Ped_Chore {
	public:
		PedChore() : myKeeper(NULL)  {}
		PedChore(const PedChore& other) : myKeeper(NULL)  {}
		virtual ~PedChore()  { if (myKeeper != NULL)  Unschedule(); }
		
		virtual void Perform() = 0;
		
		virtual void Schedule(PedChoreKeeper& keeper) {
			ASSERT(myKeeper == NULL);
			(myKeeper = &keeper)->AddChore(*this);
		}
		virtual void Unschedule() {
			ASSERT(myKeeper != NULL);
			myKeeper->RemoveChore(*this);
			myKeeper = NULL;
		}
		
	private:
		PedChoreKeeper* myKeeper;
	};
	
}

#endif

