/*
	tracker.cc
	----------
*/

#include "vlib/tracker.hh"

// POSIX
#include <pthread.h>

// Standard C++
#include <vector>

// must
#include "must/pthread.h"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/namespace_info.hh"
#include "vlib/proc_info.hh"
#include "vlib/symbol.hh"
#include "vlib/throw.hh"
#include "vlib/value.hh"
#include "vlib/iterators/full_iterator.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/proc.hh"


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
	
	static size_t n_steps = 0;
	static size_t n_culls = 0;
	
#ifdef __RELIX__
	
	#define GC_MUTEX( op )  /**/
	
#else
	
	static pthread_mutex_t gc_mutex = PTHREAD_MUTEX_INITIALIZER;
	
	#define GC_MUTEX( op )  must_pthread_mutex_##op( &gc_mutex )
	
#endif
	
	class gc_lock
	{
		private:
			// non-copyable
			gc_lock           ( const gc_lock& );
			gc_lock& operator=( const gc_lock& );
		
		public:
			gc_lock()   { GC_MUTEX( lock   ); }
			~gc_lock()  { GC_MUTEX( unlock ); }
	};
	
	void gc_safe_overwrite( Value& dst, const Value& src )
	{
		gc_lock lock;
		
		dst = src;
	}
	
	static inline
	bool is_tracked( const Symbol* sym )
	{
		return sym->mark() != Mark_none;
	}
	
	void track_symbol( const Value& v )
	{
		ASSERT( is_symbol( v ) );
		
		Symbol* sym = v.sym();
		
		ASSERT( sym );
		
		gc_lock lock;
		
		if ( ! is_tracked( sym ) )
		{
			tracked_symbols.push_back( v );
			
			sym->set_mark( Mark_black );
		}
	}
	
	void add_root( const Value& v )
	{
		if ( v.is_cycle_free() )
		{
			return;
		}
		
		gc_lock lock;
		
		tracked_roots.push_back( v );
	}
	
	void del_root( const Value& v )
	{
		if ( v.is_cycle_free() )
		{
			return;
		}
		
		gc_lock lock;
		
		typedef tracked_set::iterator Iter;
		
		const Iter begin = tracked_roots.begin();
		
		Iter it = tracked_roots.end();
		
		while ( it > begin )
		{
			--it;
			
			if ( &it->dereference< int >() == &v.dereference< int >() )
			{
				tracked_roots.erase( it );
				
				break;
			}
		}
	}
	
	static
	Symbol* next_symbol( full_iterator& it )
	{
		while ( ! it.finished() )
		{
			++n_steps;
			
			if ( Symbol* sym = it.get().sym() )
			{
				return sym;
			}
			
			it.step();
		}
		
		return NULL;
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
				garbage.push_back( sym->vtype() );
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
	
	void cull_unreachable_objects()
	{
		tracked_set garbage;
		
		gc_lock lock;
		
		cull_unreachable_objects( garbage );
		
		++n_culls;
		
		// release lock
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
	
	static
	Value v_cull( const Value& )
	{
		cull_unreachable_objects();
		
		return nothing;
	}
	
	static const proc_info proc_cull = { "cull", &v_cull, &empty_list };
	
	static
	Value tracker_getter( const plus::string& name )
	{
		if ( name == "symbols" )
		{
			return Integer( tracked_symbols.size() );
		}
		
		if ( name == "roots" )
		{
			return Integer( tracked_roots.size() );
		}
		
		if ( name == "steps" )
		{
			return Integer( n_steps );
		}
		
		if ( name == "culls" )
		{
			return Integer( n_culls );
		}
		
		if ( name == "cull" )
		{
			return Proc( proc_cull );
		}
		
		THROW( "nonexistent tracker member" );
		
		return Value();
	}
	
	const namespace_info namespace_tracker =
	{
		"tracker",
		&tracker_getter,
	};
	
}
