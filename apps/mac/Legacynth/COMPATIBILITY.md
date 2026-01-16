Legacynth Compatibility Guide
=============================

Fully working
-------------

These applications have been tested with Legacynth in Mac OS 9.  They may have minor issues, but all are usable with sound.

* Cairo ShootOut (free-form, four-tone)
* Lode Runner (square-wave, four-tone)
* NetTrek 3.0 (free-form)
* Phraze Craze Plus (free-form, four-tone)
* Shufflepuck Café 14C (free-form)
* StuntCopter 1.5 (free-form, four-tone)
* Third Dimension (square-wave)

Functioning but unusable
------------------------

These applications technically work with Legacynth (in the sense that their sounds play when Legacynth is active), but are unusable for other reasons (e.g. excessively fast gameplay).

They could be patched on an individual basis to resolve usability issues.

* Shufflepuck (free-form)
* StuntCopter 1.2 (free-form, four-tone)
* Zero Gravity (free-form, four-tone)

Fractional success
------------------

These applications work in some configurations, but crash or refuse to run in others.

* Brickles 8.0 / Brickles Plus (square-wave)
* GunShy (square-wave)

Failing entirely
----------------

These applications are incompatible with Mac OS 9 itself.  They would have to be individually patched in order to be used at all.

* Battle Chess (free-form)
* Déjà Vu (free-form)
* The Fool's Errand (square-wave)

Food for thought
----------------

Wesleyan Tetris runs in Mac OS 9 without sound.  Since it writes directly to the sound buffer and VIA instead of calling the Sound Driver, Legacynth is of no help here.  Presumably, Wesleyan Tetris could be patched to call the Sound Manager instead.

Some other applications that directly target the sound hardware don't run in Mac OS 9, but if they were patched to do so, then they too could have sound patched in as well.
