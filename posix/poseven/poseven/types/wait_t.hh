// wait_t.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_WAIT_T_HH
#define POSEVEN_TYPES_WAIT_T_HH

// Nucleus
#include "nucleus/convert.hh"
#include "nucleus/enumeration_traits.hh"

// poseven
#include "poseven/types/exit_t.hh"
#include "poseven/types/pid_t.hh"


namespace poseven
{
	
	enum wait_t
	{
		wait_t_min = -1,
		
		wait_t_max = nucleus::enumeration_traits< int >::max
	};
	
	inline int wtermsig( wait_t status )
	{
		return status & 0x7f;
	}
	
	inline bool wcoredump( wait_t status )
	{
		return status & 0x80;
	}
	
	inline exit_t wexitstatus( wait_t status )
	{
		return exit_t( (status >> 8) & 0xff );
	}
	
	inline bool wifexited( wait_t status )
	{
		return wtermsig( status ) == 0;
	}
	
	inline bool wifstopped( wait_t status )
	{
		return wtermsig( status ) == 0x7f;
	}
	
	inline bool wifsignaled( wait_t status )
	{
		return !wifexited( status ) && !wifstopped( status );
	}
	
	struct wait_result
	{
		pid_t   pid;
		wait_t  status;
		
		operator pid_t () const  { return pid;    }
		operator wait_t() const  { return status; }
	};
	
}

namespace nucleus
{
	
	template <>
	struct converter< poseven::exit_t, poseven::wait_t > : public std::unary_function< poseven::wait_t, poseven::exit_t >
	{
		poseven::exit_t operator()( poseven::wait_t status ) const
		{
			using namespace poseven;
			
			int result = wifexited  ( status ) ? wexitstatus( status )
			           : wifsignaled( status ) ? wtermsig   ( status ) + 128
			           :                         -1;
			
			return exit_t( result );
		}
	};
	
	template <>
	struct converter< poseven::exit_t, poseven::wait_result > : public converter< poseven::exit_t, poseven::wait_t > 
	{
	};
	
}

#endif

