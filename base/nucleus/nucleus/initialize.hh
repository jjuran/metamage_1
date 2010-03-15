// nucleus/initialize.hh
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_INITIALIZE_HH
#define NUCLEUS_INITIALIZE_HH

/*
	Initialize is just like Make, except that instead of returning a constructed
	object, you pass it a non-const reference as the first parameter, which it
	populates and returns back to you as the function result.
	
	FIXME:  Replace this with an Initializer example.
	
   Here's a sample extension of Make:
   
      template <>
      struct Maker< Point >
        {
         Point operator()( short v, short h ) const
           {
            Point result;
            result.v = v;
            result.h = h;
            return result;
           }
         
         Point operator()() const
           {
            return operator()( 0, 0 );
           }
        };
   
*/

namespace nucleus
{
	
	template < class R >
	struct initializer;
	
	template < class R >
	inline R& initialize( R& r )
	{
		return initializer< R >()( r );
	}
	
	template < class R, class P0 >
	inline R& initialize( R& r, P0 p0 )
	{
		return initializer< R >()( r, p0 );
	}

	template < class R, class P0, class P1 >
	inline R& initialize( R& r, P0 p0, P1 p1 )
	{
		return initializer< R >()( r, p0, p1 );
	}

	template < class R, class P0, class P1, class P2 >
	inline R& initialize( R& r, P0 p0, P1 p1, P2 p2 )
	{
		return initializer< R >()( r, p0, p1, p2 );
	}

	template < class R, class P0, class P1, class P2, class P3 >
	inline R& initialize( R& r, P0 p0, P1 p1, P2 p2, P3 p3 )
	{
		return initializer< R >()( r, p0, p1, p2, p3 );
	}

	template < class R, class P0, class P1, class P2, class P3, class P4 >
	inline R& initialize( R& r, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 )
	{
		return initializer< R >()( r, p0, p1, p2, p3, p4 );
	}

	template < class R, class P0, class P1, class P2, class P3, class P4, class P5 >
	inline R& initialize( R& r, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 )
	{
		return initializer< R >()( r, p0, p1, p2, p3, p4, p5 );
	}

	template < class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6 >
	inline R& initialize( R& r, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 )
	{
		return initializer< R >()( r, p0, p1, p2, p3, p4, p5, p6 );
	}

	template < class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7 >
	inline R& initialize( R& r, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 )
	{
		return initializer< R >()( r, p0, p1, p2, p3, p4, p5, p6, p7 );
	}
	
}

#endif
