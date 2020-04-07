/*
	functions.cc
	------------
*/

#include "varyx/ed25519/functions.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// varyx-ed25519
#include "varyx/ed25519/lib/ed25519.hh"


namespace varyx
{
namespace ed25519
{
	
	using namespace vlib;
	
	
	static
	void check_ed25519_key_size( const plus::string& key )
	{
		if ( key.size() != 32 )
		{
			THROW( "Ed25519 keys must be 32 bytes" );
		}
	}
	
	static
	void check_ed25519_sig_size( const plus::string& sig )
	{
		if ( sig.size() != 64 )
		{
			THROW( "Ed25519 signatures must be 64 bytes" );
		}
	}
	
	static
	Value v_mkpub( const Value& v )
	{
		const plus::string& secret_key = v.string();
		
		check_ed25519_key_size( secret_key );
		
		return Packed( ::ed25519::publickey( secret_key ) );
	}
	
	static
	Value v_sign( const Value& v )
	{
		list_iterator args( v );
		
		const plus::string& key = args.use().string();
		const plus::string& msg = args.get().string();
		
		check_ed25519_key_size( key );
		
		return Packed( ::ed25519::sign( key, msg ) );
	}
	
	static
	Value v_verify( const Value& v )
	{
		list_iterator args( v );
		
		const plus::string& key = args.use().string();
		const plus::string& msg = args.use().string();
		const plus::string& sig = args.get().string();
		
		check_ed25519_key_size( key );
		check_ed25519_sig_size( sig );
		
		bool b = ::ed25519::verify( key, msg, sig );
		
		return Boolean( b );
	}
	
	static const Type string  = string_vtype;
	static const Type packed  = packed_vtype;
	
	static const Value bytes( string, Op_union, packed );
	static const Value sign( packed, bytes );
	static const Value verify( packed, Value( bytes, packed ) );
	
	enum
	{
		pure = Proc_pure,
	};
	
	#define ED "ed25519-"
	
	const proc_info proc_mkpub  = { ED "publickey", &v_mkpub,  &packed, pure };
	const proc_info proc_sign   = { ED "sign",      &v_sign,   &sign,   pure };
	const proc_info proc_verify = { ED "verify",    &v_verify, &verify, pure };
	
}
}
