/*
	fade.cc
	-------
*/

#include "amicus/fade.hh"


namespace amicus
{

const CGDisplayReservationInterval limit = kCGMaxDisplayReservationInterval;

const CGDisplayFadeInterval fade_out_seconds = 0.2;
const CGDisplayFadeInterval fade_in_seconds  = 0.2;

void transition_fader::release()
{
	if ( token )
	{
		CGReleaseDisplayFadeReservation( token );
		
		token = kCGDisplayFadeReservationInvalidToken;
	}
}

void transition_fader::acquire()
{
	release();
	
	CGAcquireDisplayFadeReservation( limit, &token );
}

void transition_fader::begin()
{
	acquire();
	
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
	
	release();
}

}
