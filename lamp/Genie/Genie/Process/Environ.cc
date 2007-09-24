/*	==========
 *	Environ.cc
 *	==========
 */

#include "Genie/Process/Environ.hh"


namespace Genie
{
	
	namespace Sh = ShellShock;
	
	Environ::Environ() : itsEnvironStorage( new Sh::VarArray() ),
	                     itsEnvironAddress( NULL )
	{
	}
	
	Environ::Environ( const Environ& other ) : itsEnvironStorage( new Sh::VarArray( other.itsEnvironStorage->GetPointer() ) ),
	                                           itsEnvironAddress( other.itsEnvironAddress )
	{
		// The environment data and the pointer to the fragment's environ
		// variable have been copied from the parent.
		// The calling fragment is now associated with the new process.
		// Update its environ to point to our storage.
		UpdateEnvironValue();
	}
	
	Environ::~Environ()
	{
	}
	
	void Environ::UpdateEnvironValue()
	{
		if ( itsEnvironAddress != NULL )
		{
			*itsEnvironAddress = itsEnvironStorage->GetPointer();
		}
	}
	
	void Environ::ResetEnviron( iota::environ_t envp, iota::environ_t* address )
	{
		// Reset the environment storage as a copy of envp (or empty).
		// As an optimization, if the pointers match, then envp must be the
		// current environ, and the data are already there.
		// In that case, we just update the new fragment's environ to point to us.
		
		itsEnvironAddress = address;
		
		if ( envp == NULL )
		{
			itsEnvironStorage.reset( new Sh::VarArray );
		}
		else if ( envp != itsEnvironStorage->GetPointer() )
		{
			itsEnvironStorage.reset( new Sh::VarArray( envp ) );
		}
		
		UpdateEnvironValue();
	}
	
	char* Environ::GetEnv( const char* name )
	{
		char* result = NULL;
		
		Sh::SVector::const_iterator it = itsEnvironStorage->Find( name );
		
		const char* var = *it;
		
		const char* end = Sh::EndOfVarName( var );
		
		// Did we find the right environment variable?
		if ( end != NULL  &&  *end == '='  &&  Sh::VarMatchesName( var, end, name ) )
		{
			itsLastEnv = var;
			itsLastEnv += "\0";  // make sure we have a trailing null
			
			std::size_t offset = end + 1 - var;
			
			result = &itsLastEnv[ offset ];
		}
		
		return result;
	}
	
	void Environ::SetEnv( const char* name, const char* value, bool overwrite )
	{
		Sh::SVector::iterator it = itsEnvironStorage->Find( name );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name );
		
		// If it doesn't match, we insert (otherwise, we possibly overwrite)
		bool inserting = !match;
		
		if ( inserting )
		{
			itsEnvironStorage->Insert( it, Sh::MakeVar( name, value ) );
			
			UpdateEnvironValue();
		}
		else if ( overwrite )
		{
			itsEnvironStorage->Overwrite( it, Sh::MakeVar( name, value ) );
		}
	}
	
	void Environ::PutEnv( const char* string )
	{
		std::string name = string;
		name.resize( name.find( '=' ) );
		
		Sh::SVector::iterator it = itsEnvironStorage->Find( name.c_str() );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name.c_str() );
		
		// If it doesn't match, we insert (otherwise, we possibly overwrite)
		bool inserting = !match;
		
		if ( inserting )
		{
			itsEnvironStorage->Insert( it, string );
			
			UpdateEnvironValue();
		}
		else
		{
			itsEnvironStorage->Overwrite( it, string );
		}
	}
	
	void Environ::UnsetEnv( const char* name )
	{
		Sh::SVector::iterator it = itsEnvironStorage->Find( name );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		bool match = Sh::VarMatchesName( var, Sh::EndOfVarName( var ), name );
		
		if ( match )
		{
			itsEnvironStorage->Remove( it );
		}
	}
	
	void Environ::ClearEnv()
	{
		itsEnvironStorage->Clear();
		
		if ( itsEnvironAddress )
		{
			*itsEnvironAddress = NULL;
		}
	}
	
}

