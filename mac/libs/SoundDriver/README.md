SoundDriver
===========

The original Macintosh included the **Sound Driver**, an API for playing sounds.  It offers three modes of use:

  * **free-form** (sampled sounds at a specified playback rate)
  * **square-wave** (sequence of tones of variable amplitude, pitch, and duration)
  * **four-tone** (four-channel polyphonic synthesizer using wave tables)

Calls to the Sound Driver were traditionally implemented in vendor library code as writes to the `.Sound` driver.  Universal Interfaces no longer provides this API, though.

This project offers the header `SoundDriver.h`, which contains the necessary type definitions to form Sound Driver I/O requests.

Aside from the original `.Sound` driver, the Sound Driver API is implemented by [libsndtrack][] for [Advanced Mac Substitute][AMS].

[libsndtrack]:  <../../../sound/libsndtrack/README.md>

[AMS]:  <https://www.v68k.org/advanced-mac-substitute/>
