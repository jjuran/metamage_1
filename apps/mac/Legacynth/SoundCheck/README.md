Sound Check
===========

This is a basic test of the [Sound Driver API][SoundDriver]'s free-form mode.

[SoundDriver]:  <../../../../mac/libs/SoundDriver/README.md>

For a complete build, add a resource of type `'_SND'`, id=128,
containing the data you'd pass in an I/O request to the driver:

  * two zero bytes (mode 0 selects the free-form synthesizer)
  * 32-bit big-endian fixed point playback rate (where 0x10000 represents 22254.[54] Hz)
  * a number of unsigned 8-bit audio samples

If the `'_SND'` resource is missing, then Sound Check will report OSErr -192 (resNotFound).
