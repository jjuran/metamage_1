/*
	tracker.cc
	----------
*/

#include "vlib/tracker.hh"

// Standard C++
#include <vector>

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/iterator.hh"
#include "vlib/symbol.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	/*
		Reset (all roots and tracked symbols):
			clear -> PRUNE
			white -> PRUNE
			black -> white
		
		Mark (roots only):
			clear -> PRUNE
			white -> black
			black -> PRUNE
		
		Sweep (tracked symbols only):
			clear -> PRUNE
			white -> clear [removed]
			black -> PRUNE
	*/
	
	typedef std::vector< Value > tracked_set;
	
	static tracked_set tracked_symbols;
	static tracked_set tracked_roots;
	
	
	static
	bool is_tracked( const Symbol* sym )
	{
		typedef tracked_set::const_iterator Iter;
		
		const Iter end = tracked_symbols.end();
		
		for ( Iter it = tracked_symbols.begin();  it != end;  ++it )
		{
			if ( it->sym() == sym )
			{
				return true;
			}
		}
		
		return false;
	}
	
	void track_symbol( const Value& v )
	{
		ASSERT( is_symbol( v ) );
		
		Symbol* sym = v.sym();
		
		ASSERT( sym );
		
		if ( ! is_tracked( sym ) )
		{
			tracked_symbols.push_back( v );
			
			sym->set_mark( Mark_black );
		}
	}
	
	void add_root( const Value& v )
	{
		tracked_roots.push_back( v );
	}
	
	void del_root( const Value& v )
	{
		typedef tracked_set::iterator Iter;
		
		const Iter begin = tracked_roots.begin();
		
		Iter it = tracked_roots.end();
		
		while ( it > begin )
		{
			--it;
			
			if ( it->expr() == v.expr() )
			{
				tracked_roots.erase( it );
				
				break;
			}
		}
	}
	
	static
	void set_marks( const Value& src, mark_type old_mark, mark_type new_mark )
	{
		full_iterator it( src );
		
		while ( Symbol* sym = next_symbol( it ) )
		{
			if ( sym->mark() == old_mark )
			{
				sym->set_mark( new_mark );
				
				++it;
			}
			else
			{
				it.prune();
			}
		}
	}
	
	static
	void set_marks( tracked_set& set, mark_type old_mark, mark_type new_mark )
	{
		typedef tracked_set::iterator Iter;
		
		const Iter begin = set.begin();
		
		Iter it = set.end();
		
		while ( it > begin )
		{
			--it;
			
			set_marks( *it, old_mark, new_mark );
		}
	}
	
	static
	void sweep( const Value& src, tracked_set& garbage )
	{
		full_iterator it( src );
		
		while ( Symbol* sym = next_symbol( it ) )
		{
			if ( sym->mark() == Mark_white )
			{
				garbage.push_back( sym->get() );
				
				++it;
				
				sym->set_mark( Mark_none );
				
				sym->expire();
			}
			else
			{
				it.prune();
			}
		}
	}
	
	static
	void sweep( tracked_set& garbage )
	{
		typedef tracked_set::iterator Iter;
		
		const Iter begin = tracked_symbols.begin();
		
		Iter it = tracked_symbols.end();
		
		while ( it > begin )
		{
			--it;
			
			sweep( *it, garbage );
		}
		
		it = tracked_symbols.end();
		
		while ( it > begin )
		{
			--it;
			
			Symbol* sym = it->sym();
			
			ASSERT( sym );
			
			if ( sym->mark() == Mark_none )
			{
				if ( &*it != &tracked_symbols.back() )
				{
					swap( *it, tracked_symbols.back() );
				}
				
				tracked_symbols.pop_back();
			}
		}
	}
	
	static
	void cull_unreachable_objects( tracked_set& garbage )
	{
		// Reset
		set_marks( tracked_symbols, Mark_black, Mark_white );
		set_marks( tracked_roots,   Mark_black, Mark_white );
		
		// Mark
		set_marks( tracked_roots, Mark_white, Mark_black );
		
		sweep( garbage );
	}
	
	static
	void cull_unreachable_objects()
	{
		tracked_set garbage;
		
		cull_unreachable_objects( garbage );
		
		// dispose garbage
	}
	
	struct garbage_collector
	{
		~garbage_collector()
		{
			cull_unreachable_objects();
		}
	};
	
	static garbage_collector gc;
	
}
