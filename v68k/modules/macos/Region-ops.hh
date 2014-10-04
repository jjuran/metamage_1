/*
	Region-ops.hh
	-------------
*/

#ifndef REGIONOPS_HH
#define REGIONOPS_HH

struct MacRegion;

typedef void (*region_op_2)( const short*, const short*, short* );

void binary_region_op( MacRegion**  a,
                       MacRegion**  b,
                       MacRegion**  dst,
                       region_op_2  f );

#endif
