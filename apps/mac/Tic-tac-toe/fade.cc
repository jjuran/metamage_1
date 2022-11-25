/*
	fade.cc
	-------
*/

#include "fade.hh"


#ifdef MAC_OS_X_VERSION_10_2

const CGDisplayReservationInterval limit = kCGMaxDisplayReservationInterval;

const CGDisplayFadeInterval fade_out_seconds = 0.125;
const CGDisplayFadeInterval fade_in_seconds  = 0.125;

void transition_fader::begin()
{
	CGAcquireDisplayFadeReservation( limit, &token );
	
	CGDisplayFade( token,
	               fade_out_seconds,
	               kCGDisplayBlendNormal,
	               kCGDisplayBlendSolidColor,
	               0, 0, 0,
	               true );
}

void transition_fader::end()
{
	CGDisplayFade( token,
	               fade_in_seconds,
	               kCGDisplayBlendSolidColor,
	               kCGDisplayBlendNormal,
	               0, 0, 0,
	               false );
	
	CGReleaseDisplayFadeReservation( token );
}

#endif
