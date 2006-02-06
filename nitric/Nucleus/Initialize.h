// Make.h

#ifndef NUCLEUS_INITIALIZE_H
#define NUCLEUS_INITIALIZE_H

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

namespace Nucleus
{
	template < class R >
	struct Initializer;
	
	template < class R >
	R& Initialize( R& r )
	{
		return Initializer< R >()( r );
	}
	
	template < class R, class P0 >
	R& Initialize( R& r, P0 p0 )
	{
		return Initializer< R >()( r, p0 );
	}

	template < class R, class P0, class P1 >
	R& Initialize( R& r, P0 p0, P1 p1 )
	{
		return Initializer< R >()( r, p0, p1 );
	}

	template < class R, class P0, class P1, class P2 >
	R& Initialize( R& r, P0 p0, P1 p1, P2 p2 )
	{
		return Initializer< R >()( r, p0, p1, p2 );
	}

	template < class R, class P0, class P1, class P2, class P3 >
	R& Initialize( R& r, P0 p0, P1 p1, P2 p2, P3 p3 )
	{
		return Initializer< R >()( r, p0, p1, p2, p3 );
	}

	template < class R, class P0, class P1, class P2, class P3, class P4 >
	R& Initialize( R& r, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 )
	{
		return Initializer< R >()( r, p0, p1, p2, p3, p4 );
	}

	template < class R, class P0, class P1, class P2, class P3, class P4, class P5 >
	R& Initialize( R& r, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 )
	{
		return Initializer< R >()( r, p0, p1, p2, p3, p4, p5 );
	}

	template < class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6 >
	R& Initialize( R& r, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 )
	{
		return Initializer< R >()( r, p0, p1, p2, p3, p4, p5, p6 );
	}

	template < class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7 >
	R& Initialize( R& r, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 )
	{
		return Initializer< R >()( r, p0, p1, p2, p3, p4, p5, p6, p7 );
	}
}

#endif
