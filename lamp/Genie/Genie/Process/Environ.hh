/*	==========
 *	Environ.hh
 *	==========
 */

#ifndef GENIE_PROCESS_ENVIRON_HH
#define GENIE_PROCESS_ENVIRON_HH

// Standard C++
#include <memory>
#include <string>

// Iota
#include "iota/environ.hh"

// ShellShock
#include "ShellShock/VarArray.hh"


namespace Genie
{
	
	typedef iota::environ_t*  EnvironDataPtr;
	
	class Environ
	{
		private:
			std::auto_ptr< ShellShock::VarArray > itsEnvironStorage;
			
			EnvironDataPtr itsEnvironAddress;
			
			std::string itsLastEnv;
		
		public:
			Environ();
			
			Environ( const Environ& other );
			
			~Environ();
			
			iota::environ_t GetEnviron() const  { return itsEnvironStorage->GetPointer(); }
			
			void UpdateEnvironValue();
			
			void ResetEnviron( iota::environ_t envp, iota::environ_t* address );
			
			char* GetEnv( const char* name );
			void SetEnv( const char* name, const char* value, bool overwrite );
			void PutEnv( const char* string );
			void UnsetEnv( const char* name );
			void ClearEnv();
	};
	
}

#endif

