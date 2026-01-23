ams-snd:  Advanced Mac Substitute Sound Manager
===============================================

Depends on:

  * [ams-io][]

Sound Manager
-------------

  * asynchronous playback via sound channels
    * flushCmd, quietCmd, callbackCmd
    * bufferCmd
      * MACE3 decompression
    * TODO: noteCmd, etc.
  * _SndPlay sync and async
  * _SndNewChannel, _SndDisposeChannel
    * limits channel count to --max-channels
      * multiple channels allowed, but output is serialized
        * because channel output is currently routed through Sound Driver
          * TODO: extend or replace [sndtrack][] for channel mixing
      * --max-channels 0 forces SndNewChannel() to fail
        * useful when called without trap check, but with .Sound fallback
      * --max-channels 1 recommended
        * apps know to interrupt one sound to play the next
  * _SndSoundManagerVersion shim returns 0
  * TODO: _SndPlayDoubleBuffer
  * TODO: get/set output volume
  * TODO: patch _SysBeep

[ams-io]:  <../ams-io/#readme>
[sndtrack]:  <../../../sound/sndtrack/#readme>
